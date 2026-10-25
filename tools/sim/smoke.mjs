// SPDX-License-Identifier: GPL-3.0-only
/**
 * A quick check of a game's WebAssembly with Node alone, no browser (GitHub Actions runs it):
 * power on, press PLAY, fire, and expect a lit screen, a game in progress and some sound.
 *
 *   node tools/sim/smoke.mjs build/sim/space-invaders.wasm
 */
import { readFileSync } from 'node:fs';

const [wasm] = process.argv.slice(2);
const { instance } = await WebAssembly.instantiate(readFileSync(wasm), {});
const game = instance.exports;
game.sim_init(0);
game.sim_frame(0, 0, 1 << game.sim_coin_button());
const fire = 1 << 8; // OP4
for (let frame = 0; frame < 120; frame++) game.sim_frame(frame === 60 ? fire : 0, frame === 60 ? fire : 0, 0);
const pixels = new Uint32Array(game.memory.buffer, game.sim_screen(), 240 * 240);
const lit = pixels.filter((p) => (p & 0xffffff) !== 0).length;
const sound = new Int32Array(game.memory.buffer, game.sim_audio(4096), 4096);
const loudest = sound.reduce((m, s) => Math.max(m, Math.abs(s)), 0);
const failures = [
  ...(lit < 3000 ? [`only ${lit} pixels lit`] : []),
  ...(game.sim_waiting() ? ['PLAY did not start a game'] : []),
  ...(loudest === 0 ? ['no sound'] : []),
];
console.log(`${wasm}: ${lit} pixels lit, loudest sample ${loudest}`);
if (failures.length) {
  failures.forEach((f) => console.error(`FAIL ${f}`));
  process.exit(1);
}
