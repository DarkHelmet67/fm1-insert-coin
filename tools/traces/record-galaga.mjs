// SPDX-License-Identifier: GPL-3.0-only
/**
 * Records reference traces from the web version of Galaga (insert-coin, TypeScript): the game is
 * played frame by frame with scripted controls, and every frame's whole state is written as a
 * short line with a hash (docs/03-traces.md, tools/traces/FORMAT.md). The C port replays the same
 * controls and must produce the same lines.
 *
 *   node tools/traces/record-galaga.mjs INSERT_COIN_DIR [OUT_DIR]   all the traces
 *   node tools/traces/record-galaga.mjs INSERT_COIN_DIR --dump NAME FRAME
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
  console.error('usage: record-galaga.mjs INSERT_COIN_DIR [OUT_DIR | --dump NAME FRAME]');
  process.exit(1);
}
const gameDir = resolve(insertCoin, 'games/galaga/src');
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
const { createGame, updateGame } = await load('game.ts');
const { FIGHTER, FIRST_BOMB, objectAt } = await load('objects.ts');

// ---- the state as one canonical string (tools/traces/FORMAT.md)

const hex = (n) => n.toString(16);
const bit = (b) => (b ? 1 : 0);
const bits = (list) => list.reduce((value, b, i) => value | (bit(b) << i), 0);

/** The sounds, in the order of sound-names.ts, as one bit each. */
const SOUND_NAMES = [
  'shot',
  'beeHit',
  'butterflyHit',
  'bossHit',
  'bossDamaged',
  'capturedFighterHit',
  'fighterExplosion',
  'dive',
  'startTheme',
  'challengeStart',
  'badge',
  'pulse',
  'extraLife',
  'challengeEnd',
  'perfect',
  'bonusBee',
];

/** One object: disposition, counter, code, palette, x, y, flips and sizes, hit. */
const objectString = (o) =>
  [o.disposition, o.counter, o.code, o.color, o.x, o.y, bits([o.flipX, o.flipY, o.big, o.wide]), o.hit]
    .map(hex)
    .join('.');

const objectsString = (objects) => objects.map(objectString).join(',');

const messagesString = (messages) =>
  messages.map((m) => `${m.text}@${m.column}:${m.row}:${m.palette}`).join(';');

const timersString = (timers) => timers.join(',');

const slotString = (s) =>
  s === undefined
    ? '-'
    : [
        s.y,
        s.x,
        s.angle,
        s.targetY,
        s.targetX,
        s.pointer,
        s.speedOdd,
        s.speedEven,
        s.turn,
        s.timer,
        s.bombTimer,
        s.bombBits,
        s.object,
        s.offsetX,
        s.offsetY,
        bits([s.diveCheck, s.homing, s.mirrored]),
      ]
        .map(hex)
        .join('.');

const phaseString = (p) => {
  switch (p.step) {
    case 'splash':
      return `splash:${p.then}`;
    case 'ending':
      return `ending:${p.script}:${p.line}:${bit(p.waiting)}`;
    default:
      return p.step;
  }
};

const tasksString = (t) =>
  bits([t.sway, t.breathing, t.attacks, t.stick, t.fire, t.clones, t.fighterHits]);

const tokenString = (t) =>
  t === 'start' ? 's' : t === 'end' ? 'e' : `${hex(t.control)}.${hex(t.object)}`;

const paramsString = (p) =>
  [
    p.bombEnable,
    p.bossTimer,
    p.redTimer,
    p.yellowTimer,
    p.maxBombers,
    p.maxBombersLater,
    p.beamSpeed,
    p.continuousBombing,
    bit(p.harderEntries),
    bit(p.harderAttacks),
    p.cloneAttack,
  ].join(',');

/** The looks by object, in the order of the objects. */
const looksString = (looks) =>
  [...looks.entries()]
    .sort(([a], [b]) => a - b)
    .map(([id, l]) => `${hex(id)}:${hex(l.code)}:${l.color}:${bit(l.bombs)}`)
    .join(',');

const attacksString = (a) =>
  [
    a.countdown.join(','),
    a.queue.map((q) => `${hex(q.object)}:${bit(q.mirrored)}:${hex(q.program)}`).join(','),
    a.bossToggle,
    a.bossScores.map((s) => `${s.hundreds}:${hex(s.code)}`).join(','),
    hex(a.bonusBee),
    a.bonusBeeTimer,
    hex(a.bonusBeeLook),
    a.bonusBeeColor,
  ].join('/');

const captureString = (c) =>
  [
    bit(c.busy),
    hex(c.object),
    c.slot,
    c.beamX,
    hex(c.step),
    c.countdown,
    bit(c.pulling),
    bit(c.showText),
    c.beamSpeed,
    bit(c.connected),
    bits(c.beamRows),
    bits([c.tasks.approach, c.tasks.beam, c.tasks.pull, c.tasks.carry, c.tasks.rescue]),
  ].join(',');

const formationString = (f) =>
  [f.offsets.join(','), f.spriteCoords.join(','), f.direction, f.bitmap.join(',')].join('/');

const starControlString = (c) =>
  [bit(c.shipOnScreen), bit(c.backwards), c.target, c.current, c.remainder].join(',');

const playingString = (p) =>
  [
    `f=${p.frame}`,
    `t=${timersString(p.timers)}`,
    `ph=${phaseString(p.phase)}`,
    `tk=${tasksString(p.tasks)}`,
    `o=${objectsString(p.objects)}`,
    `sl=${p.slots.map(slotString).join(',')}`,
    `bd=${p.bombDrift.map((d) => `${d.rate}:${d.accumulator}`).join(',')}`,
    `rp=${p.rocketPaths.map((r) => `${r.dx}:${r.dy}`).join(',')}`,
    `stt=${bit(p.stepToggle)}`,
    `du=${bit(p.dual)}`,
    `fm=${formationString(p.formation)}`,
    `sf=${p.stars.phase}:${p.stars.speed}`,
    `sc=${starControlString(p.starControl)}`,
    `stg=${p.stage}`,
    `rk=${p.rank}`,
    `pa=${paramsString(p.params)}`,
    `lk=${looksString(p.looks)}`,
    `la=${p.launcher.tokens.map(tokenString).join(',')}/${p.launcher.next}/${p.launcher.waveCount}/${bit(p.launcher.running)}`,
    `ebb=${p.entryBombBits}`,
    `bi=${p.bombInterval}`,
    `at=${attacksString(p.attacks)}`,
    `mb=${p.maxBombers}`,
    `fh=${p.flyingHits}`,
    `gl=${p.groupLeft}`,
    `cp=${captureString(p.capture)}`,
    `tl=${p.tripleLeft}`,
    `tb=${p.tripleBonus.hundreds}:${hex(p.tripleBonus.code)}`,
    `en=${bit(p.enemiesEnabled)}`,
    `we=${bit(p.wavesEnabled)}`,
    `ss=${bit(p.swayStops)}`,
    `al=${p.alive}`,
    `cn=${p.counting}`,
    `rs=${bit(p.restart)}`,
    `rv=${p.reserve}`,
    `bg=${p.badges}`,
    `ms=${messagesString(p.messages)}`,
    `sco=${p.score}`,
    `nb=${p.nextBonus}`,
    `sh=${p.shots}`,
    `hi=${p.hits}`,
    `snd=${hex(bits(SOUND_NAMES.map((name) => p.sounds.includes(name))))}`,
  ].join('|');

const attractString = (a) =>
  [
    `f=${a.frame}`,
    `sf=${a.stars.phase}:${a.stars.speed}`,
    `t=${timersString(a.timers)}`,
    `st=${a.step}`,
    `n=${a.shown}`,
    `ms=${messagesString(a.messages)}`,
    `o=${objectsString(a.objects)}`,
  ].join('|');

const stateString = (g) =>
  g.screen === 'attract'
    ? `A|hi=${g.hiScore}|${attractString(g.attract)}`
    : `P|hi=${g.hiScore}|${playingString(g.playing)}`;

/** FNV-1a, 32 bits, over the string's characters (all below 256: the © is 0xA9). */
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
  const p = g.screen === 'playing' ? g.playing : undefined;
  const numbers = p ? ` s=${p.score} st=${p.stage} r=${p.reserve} ${phaseString(p.phase)}` : '';
  return `${frame} ${input} ${g.screen === 'attract' ? 'A' : 'P'} ${fnv1a(stateString(g))}${numbers}`;
};

// ---- the scripts: a function (frame, state) -> controls

const controlsOf = (direction = 0, fire = false, coin = false) => ({
  direction,
  fire,
  coin,
  mute: false,
});

/** Nobody plays: a coin, then the aliens take the three fighters, and the attract screen. */
const idle = (frame) => controlsOf(0, false, frame === 60);

/** A restless player: a coin, then left and right in turn, firing every 12 frames. */
const restless = (frame) =>
  controlsOf(Math.floor(frame / 100) % 2 === 0 ? 1 : -1, frame % 12 === 0, frame === 60);

/** Whether object `id` is an alien, a captured fighter or a bomb (not a fighter or a rocket). */
const isEnemy = (id) => id < 0x60 || id >= FIRST_BOMB;

/** Dispositions of things that move: launched, shot, entering, flying. */
const MOVING = [3, 6, 7, 9];

/**
 * A player who dodges and aims: away from the lowest bomb or alien coming down within `look`
 * pixels above the fighter and `width` pixels to its side, else under the nearest alien (not a
 * captured fighter), firing every other frame. Its moves depend on the state, so the trace keeps
 * them: the C port replays them, it does not need the bot.
 */
const dodger =
  ({ look, width }) =>
  (frame, g) => {
    if (frame === 60) return controlsOf(0, false, true);
    if (g.screen !== 'playing') return controlsOf();
    const p = g.playing;
    const fighter = objectAt(p.objects, FIGHTER);
    if (fighter.x === 0) return controlsOf();
    const fire = frame % 2 === 0;
    const threats = p.objects.filter(
      (o, i) =>
        o.x !== 0 &&
        isEnemy(i * 2) &&
        MOVING.includes(o.disposition) &&
        o.y > fighter.y - look &&
        o.y < fighter.y + 12 &&
        Math.abs(o.x - fighter.x) < width,
    );
    if (threats.length > 0) {
      const lowest = threats.reduce((a, b) => (b.y > a.y ? b : a));
      const away = lowest.x <= fighter.x ? 1 : -1;
      const cornered = (away > 0 && fighter.x > 0xd8) || (away < 0 && fighter.x < 0x1a);
      return controlsOf(cornered ? -away : away, fire);
    }
    const targets = p.objects.filter(
      (o, i) =>
        i * 2 >= 0x08 && i * 2 < 0x60 && o.x !== 0 && o.disposition !== 4 && o.disposition !== 5 && o.color !== 7,
    );
    const nearest = targets.reduce(
      (best, o) =>
        best === undefined || Math.abs(o.x - fighter.x) < Math.abs(best.x - fighter.x) ? o : best,
      undefined,
    );
    const aim = nearest ? nearest.x : 0x7a;
    return controlsOf(aim < fighter.x - 2 ? -1 : aim > fighter.x + 2 ? 1 : 0, fire);
  };

const SCRIPTS = {
  idle: { script: idle, frames: 9000 },
  restless: { script: restless, frames: 9000 },
  /** Gets captured, rescues the fighter (double fighter), meets a bonus bee, reaches stage 5. */
  rescuer: { script: dodger({ look: 150, width: 22 }), frames: 19000 },
  /** Lasts longer: two challenging stages, stage 6. */
  survivor: { script: dodger({ look: 60, width: 22 }), frames: 21000 },
};

/** Plays a script; returns the lines (and the full state string, when asked for one frame). */
const play = ({ script, frames }, dumpFrame) => {
  let g = createGame(0);
  const lines = [`# initial ${fnv1a(stateString(g))}`];
  for (let frame = 1; frame <= frames; frame++) {
    const controls = script(frame, g);
    g = updateGame(g, controls);
    if (frame === dumpFrame) return stateString(g);
    lines.push(frameLine(frame, controls, g));
  }
  return lines;
};

const commit = execFileSync('git', ['-C', insertCoin, 'rev-parse', 'HEAD']).toString().trim();

if (rest[0] === '--dump') {
  console.log(play(SCRIPTS[rest[1]], Number(rest[2])));
} else {
  const outDir = rest[0] ?? 'games/galaga/traces';
  mkdirSync(outDir, { recursive: true });
  for (const [name, entry] of Object.entries(SCRIPTS)) {
    const header = [
      `# Galaga reference trace "${name}", recorded from insert-coin ${commit}`,
      '# by tools/traces/record-galaga.mjs; format in tools/traces/FORMAT.md',
    ];
    const lines = play(entry);
    writeFileSync(join(outDir, `${name}.trace`), [...header, ...lines, ''].join('\n'));
    console.log(`${name}: ${lines.length - 1} frames, last: ${lines.at(-1)}`);
  }
}
await vite.close();
