// SPDX-License-Identifier: GPL-3.0-only
/**
 * A game's package as the installer gets it: the .fwsc from the site, checked against games.json before the synth
 * sees a byte of it. Plain functions with no page and no MIDI in them, so tools/installer/test.mjs runs them in Node.
 */
import { logicalImage, productOf, sha256hex } from "./vendor/fm1pkg.js";

/**
 * One game of games.json (written by tools/installer/bundle.py).
 * @typedef {object} GameEntry
 * @property {string} game the folder in games/ ("space-invaders")
 * @property {string} name the name on the synth's screen ("SPACE INVADERS")
 * @property {string} product the package identity ("FM-1_950"), what the FM-1 reports once it is installed
 * @property {string} pkg where the .fwsc is, from the installer's page ("../firmware/space-invaders.fwsc")
 * @property {string} sha256 the .fwsc's digest: a damaged or stale download is never offered
 * @property {number} size the .fwsc's size in bytes
 */

/** An error whose code has a sentence in texts.js. */
const fail = (code, detail = "") => Object.assign(new Error(`${code} ${detail}`.trim()), { code, detail });

/**
 * The image the synth reads during the update, from a downloaded .fwsc, once it is the one games.json names.
 * @param {Uint8Array} bytes the .fwsc
 * @param {GameEntry} entry the game it should be
 * @returns {Promise<Uint8Array>} the logical image (the .fwsc without its marker bytes)
 */
export const checkedImage = async (bytes, entry) => {
  if (bytes.length !== entry.size || (await sha256hex(bytes)) !== entry.sha256) throw fail("badpkg");
  if (productOf(bytes) !== entry.product) throw fail("badpkg");
  return logicalImage(bytes);
};

/**
 * Downloads a game's .fwsc and checks it. The browser's copy is asked about first (no-cache): a new build keeps
 * the file's name, and an old copy would not match the digest.
 * @param {GameEntry} entry the game
 * @param {typeof fetch} get fetch, or a stand-in in the tests
 * @returns {Promise<Uint8Array>} the logical image
 */
export const loadImage = async (entry, get = fetch) => {
  const response = await get(entry.pkg, { cache: "no-cache" });
  if (!response.ok) throw fail("loadfail", `${entry.pkg}: HTTP ${response.status}`);
  return checkedImage(new Uint8Array(await response.arrayBuffer()), entry);
};

/**
 * Whether a loader (an FM-1 in update mode) is one the installer may write to: Felucca's loader, the one in our
 * packages, says "ota-FM-1_9NN". The official firmware's loader may lay the flash out differently, so an update
 * it started is finished with M-VAVE's updater (as SLOOP's installer does).
 * @param {{ text: string }} id the identity the FM-1 reports
 * @returns {boolean}
 */
export const isOurLoader = (id) => /^ota-FM-1_9\d\d$/i.test(id.text);
