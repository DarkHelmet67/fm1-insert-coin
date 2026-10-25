// SPDX-License-Identifier: GPL-3.0-only
/**
 * The installer's page: the list of games, the Install button and what the synth is doing.
 *
 * The update itself is Felucca's USB-MIDI protocol, as Skiff's web installer speaks it (vendor/fm1ota.js): the
 * firmware running on the FM-1 (the official one or one of ours) checks the package and starts the update loader,
 * the loader reads the whole package and writes it, the FM-1 restarts with the new game. In both steps the synth
 * asks for pieces of the package and the page answers. Nothing is sent anywhere but to the synth.
 */
import { IS_OTA, Updater, sameModel } from "./vendor/fm1ota.js";
import { isOurLoader, loadImage } from "./package.js";
import { TEXTS } from "./texts.js";

const $ = (id) => document.getElementById(id);

/** The page's state: the language (English until the language button is pressed), what the status line says (to say it again in the other language) and whether it writes. */
const state = { lang: "en", status: null, games: [], writing: false };

const t = (key) => TEXTS[state.lang][key];

/**
 * Shows a sentence of texts.js on the status line.
 * @param {string} key the sentence
 * @param {string} [detail] what follows it (an identity, a percentage)
 * @param {boolean} [error] shown as an error
 */
const say = (key, detail = "", error = false) => {
  state.status = { key, detail, error };
  $("status").textContent = t(key) + detail;
  $("status").classList.toggle("error", error);
};

/** Adds a line to the technical details. */
const log = (line) => {
  $("log").textContent += `${line}\n`;
};

/** Writes every text of the page in the current language. */
const applyLanguage = () => {
  document.documentElement.lang = state.lang;
  document.querySelectorAll("[data-t]").forEach((element) => {
    element.textContent = t(element.dataset.t);
  });
  $("lang").textContent = t("other");
  renderGames();
  if (state.status) say(state.status.key, state.status.detail, state.status.error);
};

/** The radio button of each game, with its links. The first one is chosen. */
const renderGames = () => {
  const chosen = selectedGame()?.game ?? state.games[0]?.game;
  $("games").querySelectorAll(".game").forEach((row) => row.remove());
  for (const entry of state.games) {
    const row = document.createElement("label");
    row.className = "game";
    row.innerHTML = `<input type="radio" name="game"><span class="name"></span>
      <span class="meta"><span class="product"></span> ·
      <a class="play"></a> · <a class="download" download></a></span>`;
    row.querySelector("input").value = entry.game;
    row.querySelector("input").checked = entry.game === chosen;
    row.querySelector(".name").textContent = entry.name;
    row.querySelector(".product").textContent = entry.product;
    row.querySelector(".play").href = `../${entry.game}/`;
    row.querySelector(".play").textContent = t("play");
    row.querySelector(".download").href = entry.pkg;
    row.querySelector(".download").textContent = t("download");
    $("games").append(row);
  }
};

/** The game whose radio button is on, or undefined. */
const selectedGame = () => {
  const input = $("games").querySelector("input:checked");
  return input ? state.games.find((entry) => entry.game === input.value) : undefined;
};

/** The Install button is off while writing, and with no browser MIDI or no game. */
const lock = (writing) => {
  state.writing = writing;
  $("go").disabled = writing || !navigator.requestMIDIAccess || state.games.length === 0;
  $("games").disabled = writing;
};

/**
 * What the update reports, on the bar and the status line: the running firmware's check takes the first 5 %,
 * the loader's write (a percentage that only goes forward, vendor/fm1ota.js WriteMeter) the rest.
 */
const onStep = (key, value) => {
  log(`${key} ${value ?? ""}`);
  if (key === "verify") $("bar").value = Math.min(5, (value * 5) / 80);
  if (key === "write") $("bar").value = 5 + value * 0.95;
  if (key === "done") $("bar").value = 100;
  if (key === "write") say("write", ` ${value}%`);
  else if (key === "resume") say("resume", `${value}`);
  else if (key === "done") say("done", value);
  else if (TEXTS.en[key]) say(key);
};

/**
 * One install: find the FM-1, then either the whole update or, when an earlier one was cut off and the synth
 * waits in our loader, the rest of the write.
 * @param {MIDIAccess} midi the browser's MIDI, with SysEx
 * @param {import("./package.js").GameEntry} entry the game
 */
const install = async (midi, entry) => {
  const updater = new Updater(midi, log);
  const device = await updater.find();
  if (!device) return say("notfound", "", true);
  device.link.close();
  log(`found ${device.id.text} on "${device.name}"`);
  const model = entry.product.split("_")[0];
  if (!sameModel(device.id.model, model)) return say("model", device.id.text, true);
  if (IS_OTA(device.id) && !isOurLoader(device.id)) return say("foreign", device.id.text, true);
  const image = await loadImage(entry);
  if (IS_OTA(device.id)) {
    const finished = await updater.resume(image, onStep, entry.product);
    if (!finished) say("stopped", "", true);
    return;
  }
  await updater.install(image, entry.product, onStep);
};

$("go").addEventListener("click", async () => {
  const entry = selectedGame();
  if (state.writing || !entry) return;
  lock(true);
  $("bar").value = 0;
  try {
    let midi;
    try {
      midi = await navigator.requestMIDIAccess({ sysex: true });
    } catch (_) {
      return say("denied", "", true);
    }
    await install(midi, entry);
  } catch (error) {
    if (error?.code && TEXTS.en[error.code]) say(error.code, error.detail || "", true);
    else say("error", error?.message ?? String(error), true);
    log(String(error?.stack ?? error));
  } finally {
    lock(false);
  }
});

$("lang").addEventListener("click", () => {
  state.lang = state.lang === "it" ? "en" : "it";
  applyLanguage();
});

window.addEventListener("beforeunload", (event) => {
  if (!state.writing) return;
  event.preventDefault();
  event.returnValue = t("leave");
});

/** Loads games.json (the games and their packages) and gets the page ready. */
const start = async () => {
  applyLanguage();
  say("loading");
  try {
    const response = await fetch("games.json", { cache: "no-cache" });
    if (!response.ok) throw new Error(`games.json: HTTP ${response.status}`);
    state.games = await response.json();
    renderGames();
    if (navigator.requestMIDIAccess) say("ready");
    else say("nomidi", "", true); /* the games and their files stay there, for fm1_install.py */
  } catch (error) {
    say("loadfail", error.message, true);
  }
  lock(false);
};

start();
