// SPDX-License-Identifier: GPL-3.0-only
// The FM-1 in the browser: keys, buttons, screen and sound around a game compiled to WebAssembly
// (sim/sim.h lists what every game exports). The page knows nothing about the game.

/** The 27 note keys, F3 to G5: which are black, and the labels printed on the panel. */
const KEY_COUNT = 27;
const BLACK = new Set([1, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25]);
const LABELS = { 1: 'OP1', 3: 'OP2', 5: 'OP3', 8: 'OP4', 10: 'OP5', 13: 'OP6', 15: 'PIT', 17: 'GLO', 20: 'MONO', 22: 'POLY' };

/** The buttons shown, with their ids in fm1_in.buttons (platform/docs/API.md). */
const BUTTONS = [
  { id: 2, label: 'FX' },
  { id: 12, label: 'PLAY' },
];

/**
 * The computer's keys, for playing without touching the screen: each one presses a key of the
 * synth (note) or a button. Arrows and A/D steer, space and W fire, Enter and C start (C is
 * the coin of the web version), V switches the colours (as on the web).
 */
const COMPUTER_KEYS = {
  ArrowLeft: { note: 1 }, KeyA: { note: 3 },
  Space: { note: 8 }, ArrowUp: { note: 10 }, KeyW: { note: 10 },
  ArrowRight: { note: 13 }, KeyD: { note: 15 },
  Enter: { button: 12 }, KeyC: { button: 12 }, Digit5: { button: 12 },
  KeyV: { button: 2 },
};

const FRAME_MS = 1000 / 60;
const MAX_CATCH_UP = 4;
const RECORD_KEY = `fm1-insert-coin/${GAME_ID}/hi-score`;

/** The keys and buttons held, and those pressed since the last frame. */
const input = { notes: 0, notePresses: 0, buttons: new Set(), buttonPresses: 0 };

const pressNote = (k) => { input.notes |= 1 << k; input.notePresses |= 1 << k; };
const releaseNote = (k) => { input.notes &= ~(1 << k); };
const pressButton = (id) => { input.buttons.add(id); input.buttonPresses |= 1 << id; };
const releaseButton = (id) => { input.buttons.delete(id); };

const readRecord = () => {
  try { return Number(localStorage.getItem(RECORD_KEY)) || 0; } catch { return 0; }
};
const writeRecord = (score) => {
  try { localStorage.setItem(RECORD_KEY, String(score)); } catch { /* private window: not saved */ }
};

/** The game: the WebAssembly module and its exports. */
const loadGame = async () => {
  const bytes = Uint8Array.from(atob(WASM_BASE64), (c) => c.charCodeAt(0));
  const { instance } = await WebAssembly.instantiate(bytes, {});
  return instance.exports;
};

/** Draws the keyboard: white keys side by side, black keys between them. */
const buildKeyboard = (element) => {
  const whites = [...Array(KEY_COUNT).keys()].filter((k) => !BLACK.has(k));
  const width = 100 / whites.length;
  const keys = [];
  let white = 0;
  for (let k = 0; k < KEY_COUNT; k++) {
    const key = document.createElement('div');
    const black = BLACK.has(k);
    key.className = `key ${black ? 'black' : 'white'}`;
    key.textContent = LABELS[k] ?? '';
    key.style.width = `${black ? width * 0.62 : width}%`;
    key.style.left = `${black ? white * width - width * 0.31 : white * width}%`;
    if (!black) white++;
    key.dataset.note = String(k);
    element.append(key);
    keys[k] = key;
  }
  return keys;
};

/** Draws the buttons. */
const buildButtons = (element) =>
  Object.fromEntries(
    BUTTONS.map(({ id, label }) => {
      const button = document.createElement('button');
      button.className = 'button';
      button.textContent = label;
      button.dataset.button = String(id);
      element.append(button);
      return [id, button];
    }),
  );

/** Fingers and the mouse on keys and buttons: every pointer holds what it went down on. */
const followPointers = (root, onFirstTouch) => {
  const held = new Map();
  const target = (event) => document.elementFromPoint(event.clientX, event.clientY)?.closest('[data-note], [data-button]');
  const press = (el) => (el.dataset.note ? pressNote(Number(el.dataset.note)) : pressButton(Number(el.dataset.button)));
  const release = (el) => (el.dataset.note ? releaseNote(Number(el.dataset.note)) : releaseButton(Number(el.dataset.button)));
  root.addEventListener('pointerdown', (event) => {
    const el = target(event);
    if (!el) return;
    event.preventDefault();
    onFirstTouch();
    held.set(event.pointerId, el);
    press(el);
  });
  root.addEventListener('pointermove', (event) => {
    const was = held.get(event.pointerId);
    const el = target(event);
    if (!was || !el || el === was) return;
    release(was);
    held.set(event.pointerId, el);
    press(el);
  });
  const up = (event) => {
    const was = held.get(event.pointerId);
    if (was) release(was);
    held.delete(event.pointerId);
  };
  root.addEventListener('pointerup', up);
  root.addEventListener('pointercancel', up);
};

/** The computer keyboard. */
const followKeys = (onFirstTouch) => {
  window.addEventListener('keydown', (event) => {
    const key = COMPUTER_KEYS[event.code];
    if (!key) return;
    event.preventDefault();
    onFirstTouch();
    if (event.repeat) return;
    if (key.note !== undefined) pressNote(key.note);
    else pressButton(key.button);
  });
  window.addEventListener('keyup', (event) => {
    const key = COMPUTER_KEYS[event.code];
    if (!key) return;
    if (key.note !== undefined) releaseNote(key.note);
    else releaseButton(key.button);
  });
};

/** Sound: the browser only starts it after a touch or a key, so it begins at the first one. */
const startSound = (game) => {
  let context;
  return () => {
    if (context) return;
    context = new AudioContext({ sampleRate: 44100 });
    const node = context.createScriptProcessor(1024, 0, 1);
    node.onaudioprocess = (event) => {
      const out = event.outputBuffer.getChannelData(0);
      const samples = new Int32Array(game.memory.buffer, game.sim_audio(out.length), out.length);
      for (let i = 0; i < out.length; i++) out[i] = Math.max(-1, Math.min(1, samples[i] / 32768));
    };
    node.connect(context.destination);
    document.getElementById('sound').classList.add('on');
  };
};

const main = async () => {
  const game = await loadGame();
  const keys = buildKeyboard(document.getElementById('keyboard'));
  const buttons = buildButtons(document.getElementById('buttons'));
  const canvas = document.getElementById('screen');
  const screen = canvas.getContext('2d');
  const image = screen.createImageData(240, 240);
  const gameKeys = game.sim_game_keys();
  const coin = game.sim_coin_button();
  const color = game.sim_color_button();
  const sound = startSound(game);
  followPointers(document.querySelector('.synth'), sound);
  followKeys(sound);

  let saved = readRecord();
  game.sim_init(saved);
  let last = performance.now();
  let frames = 0;

  const lights = (now) => {
    keys.forEach((key, k) => {
      const used = (gameKeys >> k) & 1;
      key.classList.toggle('glow', Boolean(used));
      key.classList.toggle('lit', Boolean(used && (input.notes >> k) & 1));
      key.classList.toggle('down', Boolean((input.notes >> k) & 1));
    });
    Object.entries(buttons).forEach(([id, button]) => {
      const n = Number(id);
      button.classList.toggle('glow', n === coin || n === color);
      button.classList.toggle('lit', n === coin && game.sim_waiting() && now % 1000 < 500);
      button.classList.toggle('down', input.buttons.has(n));
    });
  };

  const loop = (now) => {
    const due = Math.floor((now - last) / FRAME_MS);
    for (let i = 0; i < Math.min(due, MAX_CATCH_UP); i++) {
      game.sim_frame(input.notes, input.notePresses, input.buttonPresses);
      input.notePresses = 0;
      input.buttonPresses = 0;
      frames++;
    }
    if (due > 0) {
      last += due * FRAME_MS;
      const record = game.sim_record_to_save(saved);
      if (record >= 0) { saved = record; writeRecord(record); }
      image.data.set(new Uint8ClampedArray(game.memory.buffer, game.sim_screen(), 240 * 240 * 4));
      screen.putImageData(image, 0, 0);
    }
    lights(now);
    requestAnimationFrame(loop);
  };
  requestAnimationFrame(loop);
  window.simFrames = () => frames; // for the automated test (tools/sim/test.mjs)
};

main();
