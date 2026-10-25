// SPDX-License-Identifier: GPL-3.0-only
/**
 * Records reference traces from the web version of Space Invaders (insert-coin, TypeScript): the
 * game is played frame by frame with scripted controls, and every frame's whole state is written
 * as a short line with a hash (docs/03-traces.md, tools/traces/FORMAT.md). The C port replays the
 * same controls and must produce the same lines.
 *
 *   node tools/traces/record-space-invaders.mjs INSERT_COIN_DIR [OUT_DIR]   all the traces
 *   node tools/traces/record-space-invaders.mjs INSERT_COIN_DIR --dump NAME FRAME
 *                                               the full state string of one frame, to debug
 *
 * INSERT_COIN_DIR is a checkout of github.com/DarkHelmet67/insert-coin with its packages installed
 * (pnpm install). The TypeScript stays there: this script loads it through insert-coin's own Vite,
 * which compiles it and sets import.meta.env as in the browser.
 */
import { execFileSync } from 'node:child_process';
import { mkdirSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const [insertCoin, ...rest] = process.argv.slice(2);
if (!insertCoin) {
  console.error('usage: record-space-invaders.mjs INSERT_COIN_DIR [OUT_DIR | --dump NAME FRAME]');
  process.exit(1);
}
const gameDir = resolve(insertCoin, 'games/space-invaders/src');
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
const { FRAME } = await load('playfield.ts');
const { alienWidth } = await load('aliens.ts');
const { CANNON_WIDTH } = await load('cannon.ts');

// ---- the state as one canonical string (tools/traces/FORMAT.md)

const KIND = { squid: 's', crab: 'c', octopus: 'o', rolling: 'r', plunger: 'p', squiggly: 'q' };
const opt = (value, show) => (value === undefined ? '-' : show(value));
const hexRows = ({ width, height, pixels }) =>
  Array.from({ length: height }, (_, y) => {
    const bits = Array.from({ length: width }, (_, x) => (pixels[y * width + x] ? 1 : 0));
    const padded = [...bits, ...Array((4 - (width % 4)) % 4).fill(0)];
    return Array.from({ length: padded.length / 4 }, (_, i) =>
      parseInt(padded.slice(i * 4, i * 4 + 4).join(''), 2).toString(16),
    ).join('');
  }).join('');

const playingString = (p) =>
  [
    `r=${p.round}`,
    `s=${p.score}`,
    `l=${p.lives}`,
    `xl=${p.extraLifeAwarded ? 1 : 0}`,
    `sf=${p.shotsFired}`,
    `c=${p.cannon.x}`,
    `ce=${opt(p.cannonExplosion, String)}`,
    `sh=${opt(p.shot, (s) => `${s.x}:${s.y}`)}`,
    `fl=${p.fleet.cursor}:${p.fleet.direction}:${p.fleet.dropping ? 1 : 0}:${p.fleet.beat}:${p.fleet.beatCooldown}:` +
      p.fleet.aliens
        .map((a) => `${KIND[a.kind]}${a.row}.${a.column}@${a.x}:${a.y}:${a.frame}`)
        .join(','),
    `b=${p.bombs.turn}:${p.bombs.plungerIndex}:${p.bombs.squigglyIndex}:` +
      p.bombs.active.map((b) => `${KIND[b.kind]}@${b.x}:${b.y}:${b.steps}`).join(','),
    `sd=` + p.shields.map((s) => `${s.x}:${s.y}:${hexRows(s.bitmap)}`).join(','),
    `u=${p.ufo.countdown}:${opt(p.ufo.saucer, (s) => `${s.x}:${s.direction}:${s.age}`)}`,
    `e=` +
      p.effects
        .map((e) => `${e.kind}@${e.x}:${e.y}:${e.framesLeft}${e.points === undefined ? '' : `:${e.points}`}`)
        .join(','),
  ].join(';');

const stateString = (g) => {
  switch (g.screen) {
    case 'attract':
      return `A|hi=${g.hiScore}`;
    case 'playing':
      return `P|hi=${g.hiScore}|rec=${g.recordToBeat}|${playingString(g.playing)}`;
    case 'gameOver':
      return `O|hi=${g.hiScore}|left=${g.framesLeft}|${playingString(g.playing)}`;
  }
};

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
  const input = `${controls.direction < 0 ? 'L' : controls.direction > 0 ? 'R' : '-'}${controls.fire ? 'F' : '-'}${controls.coin ? 'C' : '-'}`;
  const p = g.screen === 'attract' ? undefined : g.playing;
  const numbers = p ? ` s=${p.score} l=${p.lives} r=${p.round} n=${p.fleet.aliens.length}` : '';
  return `${frame} ${input} ${g.screen[0].toUpperCase()} ${fnv1a(stateString(g))}${numbers}`;
};

// ---- the scripts: a function (frame, state) -> controls

const controlsOf = (direction = 0, fire = false, coin = false) => ({
  direction,
  fire,
  coin,
  mute: false,
  colorMode: false,
});

/** Nobody plays: a coin, then the bombs take the three cannons. */
const idle = (frame) => controlsOf(0, false, frame === 60);

/** A restless player: a coin, then left and right in turn, firing every 20 frames. */
const restless = (frame) =>
  controlsOf(Math.floor(frame / 90) % 2 === 0 ? 1 : -1, frame % 20 === 0, frame === 60);

/**
 * A player who aims and dodges: away from a bomb falling close above the cannon, else under the
 * invader nearest the cannon, firing when below it. Its moves depend on the state, so the trace
 * keeps them: the C port replays them, it does not need the bot.
 */
const aiming = (frame, g) => {
  if (frame === 60) return controlsOf(0, false, true);
  if (g.screen !== 'playing') return controlsOf();
  const p = g.playing;
  const center = p.cannon.x + Math.floor(CANNON_WIDTH / 2);
  const danger = p.bombs.active.find((b) => b.y > 120 && Math.abs(b.x + 1 - center) < 18);
  if (danger) return controlsOf(danger.x + 1 < center ? 1 : -1, false);
  const targets = p.fleet.aliens.map((a) => a.x + Math.floor(alienWidth(a) / 2));
  const nearest = (best, x) => (Math.abs(x - center) < Math.abs(best - center) ? x : best);
  const target = targets.reduce(nearest, targets[0] ?? center);
  const direction = target < center - 1 ? -1 : target > center + 1 ? 1 : 0;
  return controlsOf(direction, direction === 0);
};

const SCRIPTS = {
  idle: { script: idle, frames: 6000 },
  restless: { script: restless, frames: 6000 },
  aiming: { script: aiming, frames: 20000 },
};

/** Plays a script; returns the lines (and the full state strings, when asked for one frame). */
const play = ({ script, frames }, dumpFrame) => {
  let g = initialGameState;
  const lines = [`# initial ${fnv1a(stateString(g))}`];
  for (let frame = 1; frame <= frames; frame++) {
    const controls = script(frame, g);
    g = updateGame(g, controls, FRAME);
    if (frame === dumpFrame) return stateString(g);
    lines.push(frameLine(frame, controls, g));
  }
  return lines;
};

const commit = execFileSync('git', ['-C', insertCoin, 'rev-parse', 'HEAD']).toString().trim();

if (rest[0] === '--dump') {
  console.log(play(SCRIPTS[rest[1]], Number(rest[2])));
} else {
  const outDir = rest[0] ?? 'games/space-invaders/traces';
  mkdirSync(outDir, { recursive: true });
  for (const [name, entry] of Object.entries(SCRIPTS)) {
    const header = [
      `# Space Invaders reference trace "${name}", recorded from insert-coin ${commit}`,
      '# by tools/traces/record-space-invaders.mjs; format in tools/traces/FORMAT.md',
    ];
    const lines = play(entry);
    writeFileSync(join(outDir, `${name}.trace`), [...header, ...lines, ''].join('\n'));
    console.log(`${name}: ${lines.length - 1} frames, last: ${lines.at(-1)}`);
  }
}
await vite.close();
