// SPDX-License-Identifier: GPL-3.0-only
/**
 * Plays the browser simulator for a few seconds in Chromium and checks it works: the page loads
 * from disk with no errors, frames run at 60 Hz, PLAY starts a game and the keys fire. Saves
 * screenshots of the screen in SHOTS_DIR (build/sim by default). Needs Playwright and its Chromium.
 *
 *   node tools/sim/test.mjs build/site/space-invaders/index.html [SHOTS_DIR]
 */
import { createRequire } from 'node:module';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const [pagePath, shotsDir = 'build/sim'] = process.argv.slice(2);
if (!pagePath) {
  console.error('usage: test.mjs PAGE');
  process.exit(1);
}
const { chromium } = createRequire(import.meta.url)('playwright');
const browser = await chromium.launch();
const page = await browser.newPage({ viewport: { width: 800, height: 900 } });
const errors = [];
page.on('pageerror', (error) => errors.push(error.message));
page.on('console', (message) => message.type() === 'error' && errors.push(message.text()));
await page.goto(pathToFileURL(resolve(pagePath)).href);

const pause = (ms) => page.waitForTimeout(ms);
const shot = (name) => page.locator('#screen').screenshot({ path: join(shotsDir, `sim-${name}.png`) });
const lit = () =>
  page.evaluate(() => {
    const canvas = document.getElementById('screen');
    const data = canvas.getContext('2d').getImageData(0, 0, 240, 240).data;
    let n = 0;
    for (let i = 0; i < data.length; i += 4) n += data[i] + data[i + 1] + data[i + 2] > 0 ? 1 : 0;
    return n;
  });

await pause(1000);
const frames = await page.evaluate(() => window.simFrames());
await shot('attract');
await page.keyboard.press('Enter');
await pause(1200);
await page.keyboard.down('ArrowRight');
for (let i = 0; i < 6; i++) {
  await page.keyboard.press('Space');
  await pause(400);
}
await page.keyboard.up('ArrowRight');
await shot('playing');
const playingPixels = await lit();
await page.keyboard.press('KeyV');
await pause(200);
await shot('mono');
await browser.close();

const failures = [
  ...errors.map((e) => `page error: ${e}`),
  ...(frames < 40 || frames > 75 ? [`${frames} frames in the first second, expected about 60`] : []),
  ...(playingPixels < 3000 ? [`only ${playingPixels} pixels lit while playing`] : []),
];
console.log(`frames in 1 s: ${frames}, pixels lit while playing: ${playingPixels}`);
if (failures.length) {
  failures.forEach((f) => console.error(`FAIL ${f}`));
  process.exit(1);
}
console.log('simulator ok');
