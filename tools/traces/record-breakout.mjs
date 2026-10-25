// SPDX-License-Identifier: GPL-3.0-only
/**
 * Records reference traces from the web version of Breakout (insert-coin, TypeScript): the game
 * is played frame by frame with scripted controls, and every frame's whole state is written as a
 * short line with a hash (docs/03-traces.md, tools/traces/FORMAT.md). The C port replays the same
 * controls and must produce the same lines.
 *
 *   node tools/traces/record-breakout.mjs INSERT_COIN_DIR [OUT_DIR]   all the traces
 *   node tools/traces/record-breakout.mjs INSERT_COIN_DIR --dump NAME FRAME
 *                                          the full state string of one frame, to debug
 *
 * INSERT_COIN_DIR is a checkout of github.com/DarkHelmet67/insert-coin with its packages installed
 * (pnpm install). The TypeScript stays there: this script loads it through insert-coin's own Vite.
 */
import { execFileSync } from 'node:child_process';
import { mkdirSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const [insertCoin, ...rest] = process.argv.slice(2);
if (!insertCoin) {
  console.error('usage: record-breakout.mjs INSERT_COIN_DIR [OUT_DIR | --dump NAME FRAME]');
  process.exit(1);
}
const gameDir = resolve(insertCoin, 'games/breakout/src');
const { createServer } = await import(
  pathToFileURL(resolve(insertCoin, 'node_modules/vite/dist/node/index.js')).href
);
const vite = await createServer({
  root: resolve(insertCoin),
  logLevel: 'error',
  server: { middlewareMode: true, hmr: false },
  appType: 'custom',
});
const load = (name) => vite.ssrLoadModule(join(gameDir, name));
const { initialGameState, updateGame } = await load('game.ts');
const { BALL_WIDTH } = await load('playfield.ts');

// ---- the state as one canonical string (tools/traces/FORMAT.md)

/**
 * A position of the ball, in fifths of a pixel: the speeds are multiples of 0.2, so the C port
 * counts fifths, exactly. Rounding hides the tiny errors of the floating point additions.
 */
const fifths = (value) => Math.round(value * 5);

/**
 * The state with the ball put back on exact fifths of a pixel. The web version adds 0.2 steps in
 * floating point, and the error grows: 218.79999999999998 + 1.2 gives 219.99999999999997, so a
 * ball that should touch the right wall (x + 4 >= 224) misses it by a hair and keeps going. The
 * C port is exact, so the recorder removes that drift after every frame: a deliberate,
 * documented difference (tools/traces/FORMAT.md), the game the TypeScript means to play.
 */
const snapped = (g) => {
  const ball = g.play.phase === 'inPlay' ? g.play.ball : undefined;
  if (!ball) return g;
  const snap = (v) => fifths(v) / 5;
  return { ...g, play: { ...g.play, ball: { ...ball, x: snap(ball.x), y: snap(ball.y) } } };
};

/** The wall as hex digits, four bricks a digit, row by row from the top left. */
const wallHex = (wall) =>
  Array.from({ length: wall.length / 4 }, (_, i) =>
    parseInt(wall.slice(i * 4, i * 4 + 4).map((b) => (b ? 1 : 0)).join(''), 2).toString(16),
  ).join('');

const playString = (play) => {
  switch (play.phase) {
    case 'ready':
      return 'r';
    case 'serving':
      return `s:${play.framesLeft}:${play.pressedAt}`;
    case 'inPlay': {
      const b = play.ball;
      return (
        `i:${fifths(b.x)}:${fifths(b.y)}:${b.dirX}:${b.dirY}:${b.hits}:` +
        `${b.fast ? 1 : 0}:${b.outer ? 1 : 0}:${b.canHitBrick ? 1 : 0}`
      );
    }
    case 'gameOver':
      return 'o';
  }
};

const stateString = (g) =>
  [
    `${g.attract ? 'A' : 'P'}`,
    `pl=${playString(g.play)}`,
    `w=${wallHex(g.wall)}`,
    `s=${g.score}`,
    `b=${g.ball}`,
    `sh=${g.shrunk ? 1 : 0}`,
    `rf=${g.refilled ? 1 : 0}`,
    `p=${g.paddle.x}:${g.paddle.width}`,
    `clk=${g.clock}`,
    `hi=${g.hiScore}`,
    `rec=${g.recordToBeat}`,
    `t=${g.ticks.owed}:${g.ticks.played}:${g.ticks.wait}`,
  ].join('|');

/** FNV-1a, 32 bits, over the string's bytes (ASCII). */
const fnv1a = (text) => {
  let hash = 0x811c9dc5;
  for (let i = 0; i < text.length; i++) {
    hash ^= text.charCodeAt(i);
    hash = Math.imul(hash, 0x01000193) >>> 0;
  }
  return hash.toString(16).padStart(8, '0');
};

/** The short line of one frame: number, controls, screen, hash, and a few numbers to read. */
const frameLine = (frame, controls, g) => {
  const input = `${controls.direction < 0 ? 'L' : controls.direction > 0 ? 'R' : '-'}${controls.serve ? 'F' : '-'}-`;
  const numbers = ` s=${g.score} b=${g.ball} ${g.play.phase}`;
  return `${frame} ${input} ${g.attract ? 'A' : 'P'} ${fnv1a(stateString(g))}${numbers}`;
};

// ---- the scripts: a function (frame, state) -> controls

const controlsOf = (direction = 0, serve = false) => ({
  direction,
  pointerX: undefined,
  serve,
  mute: false,
  colorMode: false,
});

/** Serves at frame 60 (the game starts), then whenever the ball is waiting, every 50 frames. */
const serving = (frame, g) =>
  frame === 60 || (!g.attract && g.play.phase === 'ready' && frame % 50 === 0);

/** Nobody plays: the game starts, every ball is served and lost, back to the attract mode. */
const idle = (frame, g) => controlsOf(0, serving(frame, g));

/** A restless player: left and right in turn, every 70 frames. */
const restless = (frame, g) => controlsOf(Math.floor(frame / 70) % 2 === 0 ? 1 : -1, serving(frame, g));

/**
 * A player who follows the ball with the keys, aiming at five parts of the paddle in turn so
 * that every segment gets hit: it breaks through the wall, speeds up and gets the second wall.
 * Its moves depend on the state, so the trace keeps them: the C port replays them, it does not
 * need the bot.
 */
const tracker = (frame, g) => {
  const serve = serving(frame, g);
  if (g.attract || g.play.phase !== 'inPlay') return controlsOf(0, serve);
  const ball = g.play.ball.x + BALL_WIDTH / 2;
  const offset = ((Math.floor(frame / 377) % 5) - 2) * (g.paddle.width / 6);
  const aim = g.paddle.x + g.paddle.width / 2 + offset;
  const direction = ball < aim - 1 ? -1 : ball > aim + 1 ? 1 : 0;
  return controlsOf(direction, serve);
};

const SCRIPTS = {
  idle: { script: idle, frames: 6000 },
  restless: { script: restless, frames: 6000 },
  tracker: { script: tracker, frames: 45000 },
};

/** Plays a script; returns the lines (and the full state string, when asked for one frame). */
const play = ({ script, frames }, dumpFrame) => {
  let g = initialGameState;
  const lines = [`# initial ${fnv1a(stateString(g))}`];
  for (let frame = 1; frame <= frames; frame++) {
    const controls = script(frame, g);
    g = snapped(updateGame(g, controls));
    if (frame === dumpFrame) return stateString(g);
    lines.push(frameLine(frame, controls, g));
  }
  return lines;
};

const commit = execFileSync('git', ['-C', insertCoin, 'rev-parse', 'HEAD']).toString().trim();

if (rest[0] === '--dump') {
  console.log(play(SCRIPTS[rest[1]], Number(rest[2])));
} else {
  const outDir = rest[0] ?? 'games/breakout/traces';
  mkdirSync(outDir, { recursive: true });
  for (const [name, entry] of Object.entries(SCRIPTS)) {
    const header = [
      `# Breakout reference trace "${name}", recorded from insert-coin ${commit}`,
      '# by tools/traces/record-breakout.mjs; format in tools/traces/FORMAT.md',
    ];
    const lines = play(entry);
    writeFileSync(join(outDir, `${name}.trace`), [...header, ...lines, ''].join('\n'));
    console.log(`${name}: ${lines.length - 1} frames, last: ${lines.at(-1)}`);
  }
}
await vite.close();
