// SPDX-License-Identifier: GPL-3.0-only
/**
 * A quick check of the firmware's WebAssembly with Node alone, no browser (GitHub Actions runs
 * it): power on in the menu, turn SELECT through every game and back, then for Space Invaders and
 * for Galaga (whose sound is the WSG chip) press PLAY to enter the game and PLAY again to start
 * it, fire, and expect a lit screen, a game in progress and some sound; holding PLAY goes back to
 * the menu.
 *
 *   node tools/sim/smoke.mjs build/sim/insert-coin.wasm
 */
import { readFileSync } from 'node:fs';

const [wasm] = process.argv.slice(2);
const { instance } = await WebAssembly.instantiate(readFileSync(wasm), {});
const game = instance.exports;
const PLAY = 1 << 12;
const SELECT = 0;
const fire = 1 << 8; // OP4
const LEAVE_FRAMES = 120; // PLAY held this long goes back to the menu (launcher/src/config.h)
const failures = [];

/** Plays `frames` frames of a game, firing at frame 60; returns the loudest sample heard. */
const playFrames = (frames) => {
  let loudest = 0;
  for (let frame = 0; frame < frames; frame++) {
    game.sim_frame(frame === 60 ? fire : 0, frame === 60 ? fire : 0, 0, 0);
    const sound = new Int32Array(game.memory.buffer, game.sim_audio(735), 735);
    loudest = sound.reduce((m, s) => Math.max(m, Math.abs(s)), loudest);
  }
  return loudest;
};

/** Enters the game chosen in the menu, starts it and checks the screen and the sound. */
const checkGame = (name, frames) => {
  game.sim_frame(0, 0, PLAY, PLAY); // into the game: its attract screen
  game.sim_frame(0, 0, 0, 0);
  game.sim_frame(0, 0, PLAY, PLAY); // the coin: a game
  const loudest = playFrames(frames);
  const pixels = new Uint32Array(game.memory.buffer, game.sim_screen(), 240 * 240);
  const lit = pixels.filter((p) => (p & 0xffffff) !== 0).length;
  console.log(`${name}: ${lit} pixels lit, loudest sample ${loudest}`);
  if (lit < 500) failures.push(`${name}: only ${lit} pixels lit`);
  if (game.sim_waiting()) failures.push(`${name}: PLAY did not start a game`);
  if (loudest === 0) failures.push(`${name}: no sound`);
  for (let frame = 0; frame < LEAVE_FRAMES; frame++) game.sim_frame(0, 0, PLAY, 0);
  if (!game.sim_waiting()) failures.push(`${name}: holding PLAY did not go back to the menu`);
};

game.sim_init();
if (!game.sim_waiting()) failures.push('PLAY does not blink in the menu');
for (let i = 0; i < 8; i++) {
  game.sim_turn(SELECT, 1);
  game.sim_frame(0, 0, 0, 0);
}
game.sim_turn(SELECT, -8); // back to the first game, Space Invaders
game.sim_frame(0, 0, 0, 0);
checkGame('Space Invaders', 120);
game.sim_turn(SELECT, 2); // Galaga, the third
game.sim_frame(0, 0, 0, 0);
checkGame('Galaga', 120);

console.log(wasm);
if (failures.length) {
  failures.forEach((f) => console.error(`FAIL ${f}`));
  process.exit(1);
}
