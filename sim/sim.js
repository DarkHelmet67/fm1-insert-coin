// SPDX-License-Identifier: GPL-3.0-only
// The FM-1 in the browser: keys, buttons, encoders, screen and sound around the firmware compiled to
// WebAssembly (launcher/sim/sim.c lists what it exports). The page knows nothing about the games.

/** The 27 note keys, F3 to G5: which are black, and the labels printed on the panel. */
const KEY_COUNT = 27;
const BLACK = new Set([1, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25]);
const LABELS = { 1: 'OP1', 3: 'OP2', 5: 'OP3', 8: 'OP4', 10: 'OP5', 13: 'OP6', 15: 'PIT', 17: 'GLO', 20: 'MONO', 22: 'POLY' };

/** The buttons shown, with their ids in fm1_in.buttons (platform/docs/API.md). */
const BUTTONS = [
  { id: 2, label: 'FX' },
  { id: 12, label: 'PLAY' },
];

/** The encoders shown, with their ids for fm1_enc_take(): the three left of the screen, the four knobs. */
const ENCODERS = [
  { id: 0, label: 'SELECT' },
  { id: 1, label: 'ALGORITHM' },
  { id: 6, label: 'PRESETS' },
  { id: 2, label: 'KNOB 1' },
  { id: 3, label: 'KNOB 2' },
  { id: 4, label: 'KNOB 3' },
  { id: 5, label: 'KNOB 4' },
];

/** Dragging an encoder by this many pixels turns it one click. */
const DRAG_PIXELS_PER_CLICK = 12;

/**
 * The computer's keys, for playing without touching the screen: each one presses a key of the
 * synth (note), a button, or turns an encoder (one click a frame while held). Arrows and A/D
 * steer (and turn KNOB 1, Breakout's paddle), space and W fire, Up and Down turn SELECT (the
 * menu), Enter and C press PLAY (C is the coin of the web version), V presses FX.
 */
const COMPUTER_KEYS = {
  ArrowLeft: { note: 1, encoder: 2, clicks: -1 }, KeyA: { note: 3, encoder: 2, clicks: -1 },
  Space: { note: 8 }, KeyW: { note: 10 },
  ArrowRight: { note: 13, encoder: 2, clicks: 1 }, KeyD: { note: 15, encoder: 2, clicks: 1 },
  ArrowUp: { encoder: 0, clicks: -1, once: true }, ArrowDown: { encoder: 0, clicks: 1, once: true },
  Enter: { button: 12 }, KeyC: { button: 12 }, Digit5: { button: 12 },
  KeyV: { button: 2 },
};

const FRAME_MS = 1000 / 60;
const MAX_CATCH_UP = 4;
const RECORDS_KEY = 'fm1-insert-coin/records';

/**
 * The keys and buttons held, those pressed since the last frame, the encoder clicks not yet sent
 * and the encoders turning while a computer key is held.
 */
const input = { notes: 0, notePresses: 0, buttons: new Set(), buttonPresses: 0, turns: new Map(), spinning: new Map() };

const turn = (id, clicks) => input.turns.set(id, (input.turns.get(id) ?? 0) + clicks);
const buttonBits = () => [...input.buttons].reduce((bits, id) => bits | (1 << id), 0);

const pressNote = (k) => { input.notes |= 1 << k; input.notePresses |= 1 << k; };
const releaseNote = (k) => { input.notes &= ~(1 << k); };
const pressButton = (id) => { input.buttons.add(id); input.buttonPresses |= 1 << id; };
const releaseButton = (id) => { input.buttons.delete(id); };

/** The best scores kept in the browser, as [tag, score] pairs. */
const readRecords = () => {
  try {
    const records = JSON.parse(localStorage.getItem(RECORDS_KEY) ?? '[]');
    return Array.isArray(records) ? records.filter((r) => Array.isArray(r) && r.length === 2) : [];
  } catch {
    return [];
  }
};
const writeRecords = (game) => {
  const records = [...Array(game.sim_record_count()).keys()].map((i) => [game.sim_record_tag(i), game.sim_record_score(i)]);
  try { localStorage.setItem(RECORDS_KEY, JSON.stringify(records)); } catch { /* private window: not saved */ }
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

/** Draws the encoders: a knob with its name under it. */
const buildEncoders = (element) =>
  ENCODERS.forEach(({ id, label }) => {
    const encoder = document.createElement('div');
    encoder.className = 'encoder';
    encoder.dataset.encoder = String(id);
    encoder.innerHTML = '<span class="knob"><span class="mark"></span></span><span class="name"></span>';
    encoder.querySelector('.name').textContent = label;
    element.append(encoder);
  });

/** Turns the knob drawn for encoder `id` by one click, so it moves when it is turned. */
const showTurn = (id, clicks) => {
  const knob = document.querySelector(`[data-encoder="${id}"] .knob`);
  if (!knob) return;
  const angle = (Number(knob.dataset.angle) || 0) + clicks * 18;
  knob.dataset.angle = String(angle);
  knob.style.transform = `rotate(${angle}deg)`;
};

/** The mouse wheel and dragging, up or right clockwise, turn the encoders. */
const followEncoders = (element, onFirstTouch) => {
  const encoderOf = (event) => event.target.closest('[data-encoder]');
  element.addEventListener('wheel', (event) => {
    const encoder = encoderOf(event);
    if (!encoder) return;
    event.preventDefault();
    onFirstTouch();
    const clicks = event.deltaY < 0 ? 1 : -1;
    turn(Number(encoder.dataset.encoder), clicks);
    showTurn(encoder.dataset.encoder, clicks);
  }, { passive: false });
  const drags = new Map();
  element.addEventListener('pointerdown', (event) => {
    const encoder = encoderOf(event);
    if (!encoder) return;
    event.preventDefault();
    onFirstTouch();
    encoder.setPointerCapture(event.pointerId);
    drags.set(event.pointerId, { id: Number(encoder.dataset.encoder), x: event.clientX, y: event.clientY });
  });
  element.addEventListener('pointermove', (event) => {
    const drag = drags.get(event.pointerId);
    if (!drag) return;
    const moved = event.clientX - drag.x - (event.clientY - drag.y);
    const clicks = Math.trunc(moved / DRAG_PIXELS_PER_CLICK);
    if (clicks === 0) return;
    turn(drag.id, clicks);
    showTurn(drag.id, clicks);
    drag.x = event.clientX;
    drag.y = event.clientY;
  });
  const up = (event) => drags.delete(event.pointerId);
  element.addEventListener('pointerup', up);
  element.addEventListener('pointercancel', up);
};

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
    if (key.button !== undefined) pressButton(key.button);
    if (key.encoder !== undefined && key.once) {
      turn(key.encoder, key.clicks);
      showTurn(key.encoder, key.clicks);
    } else if (key.encoder !== undefined) {
      input.spinning.set(event.code, key);
    }
  });
  window.addEventListener('keyup', (event) => {
    const key = COMPUTER_KEYS[event.code];
    if (!key) return;
    if (key.note !== undefined) releaseNote(key.note);
    if (key.button !== undefined) releaseButton(key.button);
    input.spinning.delete(event.code);
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
  buildEncoders(document.getElementById('encoders'));
  const canvas = document.getElementById('screen');
  const screen = canvas.getContext('2d');
  const image = screen.createImageData(240, 240);
  const play = 12;
  const sound = startSound(game);
  followPointers(document.querySelector('.synth'), sound);
  followEncoders(document.getElementById('encoders'), sound);
  followKeys(sound);

  readRecords().forEach(([tag, score]) => game.sim_set_record(tag, score));
  game.sim_init();
  let last = performance.now();
  let frames = 0;

  const lights = (now) => {
    const gameKeys = game.sim_keys();
    const used = game.sim_buttons();
    keys.forEach((key, k) => {
      const used = (gameKeys >> k) & 1;
      key.classList.toggle('glow', Boolean(used));
      key.classList.toggle('lit', Boolean(used && (input.notes >> k) & 1));
      key.classList.toggle('down', Boolean((input.notes >> k) & 1));
    });
    Object.entries(buttons).forEach(([id, button]) => {
      const n = Number(id);
      button.classList.toggle('glow', n === play || Boolean((used >> n) & 1));
      button.classList.toggle('lit', n === play && game.sim_waiting() && now % 1000 < 500);
      button.classList.toggle('down', input.buttons.has(n));
    });
  };

  const loop = (now) => {
    const due = Math.floor((now - last) / FRAME_MS);
    for (let i = 0; i < Math.min(due, MAX_CATCH_UP); i++) {
      input.spinning.forEach(({ encoder, clicks }) => { turn(encoder, clicks); showTurn(encoder, clicks); });
      input.turns.forEach((clicks, id) => game.sim_turn(id, clicks));
      input.turns.clear();
      game.sim_frame(input.notes, input.notePresses, buttonBits(), input.buttonPresses);
      input.notePresses = 0;
      input.buttonPresses = 0;
      frames++;
    }
    if (due > 0) {
      last += due * FRAME_MS;
      if (game.sim_records_changed()) writeRecords(game);
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
