// SPDX-License-Identifier: GPL-3.0-only
/**
 * Runs the installer's page in Chromium against the fake FM-1 of fake_fm1.mjs, put in place of the browser's
 * WebMIDI: the page lists the games, Install goes all the way to "done" with no page errors, and the site's
 * index links the installer. Saves a screenshot of the page in SHOTS_DIR. Needs Playwright and its Chromium.
 *
 *   node tools/installer/page-test.mjs SITE_DIR [SHOTS_DIR]
 *
 * SITE_DIR is a site made by tools/installer/bundle.py; the page is served over http, since it loads modules.
 */
import { spawn } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { createRequire } from 'node:module';
import { join } from 'node:path';

const [siteDir, shotsDir = 'build/installer'] = process.argv.slice(2);
if (!siteDir) {
  console.error('usage: page-test.mjs SITE_DIR [SHOTS_DIR]');
  process.exit(1);
}
const PORT = 8765;
const server = spawn('python3', ['-m', 'http.server', String(PORT), '--bind', '127.0.0.1', '--directory', siteDir], {
  stdio: 'ignore',
});
await new Promise((done) => setTimeout(done, 800));

// the fake FM-1 as a plain script: its one export becomes a global the stand-in WebMIDI calls
const fake = readFileSync(new URL('./fake_fm1.mjs', import.meta.url), 'utf8').replace('export const createFakeFm1', 'window.createFakeFm1');
const standIn = `
${fake}
navigator.requestMIDIAccess = async () => {
  const { logicalImage } = await import('/installer/vendor/fm1pkg.js');
  const [game] = await (await fetch('/installer/games.json')).json();
  const bytes = new Uint8Array(await (await fetch('/installer/' + game.pkg)).arrayBuffer());
  const fm1 = window.createFakeFm1(logicalImage(bytes), { after: game.product, loader: 'ota-' + game.product });
  window.fakeDevice = fm1.device;
  return fm1.access;
};`;

const { chromium } = createRequire(import.meta.url)('playwright');
const browser = await chromium.launch();
const failures = [];
try {
  const page = await browser.newPage({ viewport: { width: 700, height: 1100 }, locale: 'it-IT' }); // an Italian browser: the page must still start in English
  page.on('pageerror', (error) => failures.push(`page error: ${error.message}`));
  page.on('console', (message) => message.type() === 'error' && failures.push(`console: ${message.text()}`));
  await page.addInitScript(standIn);

  await page.goto(`http://127.0.0.1:${PORT}/`);
  if ((await page.locator('a[href="installer/"]').count()) !== 1) failures.push('the site index does not link the installer');

  await page.goto(`http://127.0.0.1:${PORT}/installer/`);
  await page.waitForFunction(() => !document.getElementById('go').disabled, null, { timeout: 5000 });
  const games = await page.locator('.game .name').allTextContents();
  if (games.length === 0) failures.push('no game in the list');

  await page.click('#go');
  await page.waitForFunction(() => /Done/.test(document.getElementById('status').textContent), null, { timeout: 60000 });
  const device = await page.evaluate(() => ({ bad: window.fakeDevice.bad, history: window.fakeDevice.history }));
  if (device.bad) failures.push(`${device.bad} wrong answers to the FM-1`);
  console.log(`games: ${games.join(', ')}; FM-1: ${device.history.join(' > ')}`);
  console.log(`status: ${await page.locator('#status').textContent()}`);
  await page.screenshot({ path: join(shotsDir, 'installer.png'), fullPage: true });
} catch (error) {
  failures.push(String(error));
} finally {
  await browser.close();
  server.kill();
}

for (const failure of failures) console.error(`FAIL ${failure}`);
console.log(failures.length ? `${failures.length} failed` : 'installer page ok');
process.exit(failures.length ? 1 : 0);
