// SPDX-License-Identifier: GPL-3.0-only
/**
 * Renders the sound effects of the web version of Space Invaders (insert-coin) to WAV files, with
 * the web version's own synthesizer (packages/audio/src/synth.ts) running in Chromium's
 * OfflineAudioContext. compare.py then measures the C synthesizer against these files
 * (docs/06-suoni.md).
 *
 *   node tools/sounds/render-web-sounds.mjs INSERT_COIN_DIR OUT_DIR
 *
 * INSERT_COIN_DIR is a checkout of insert-coin with its packages installed; Playwright and its
 * Chromium must be installed where Node can find them.
 */
import { mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { createRequire } from 'node:module';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const [insertCoin, outDir = 'build/sounds/web'] = process.argv.slice(2);
if (!insertCoin) {
  console.error('usage: render-web-sounds.mjs INSERT_COIN_DIR [OUT_DIR]');
  process.exit(1);
}
const root = resolve(insertCoin);
const require = createRequire(join(root, 'package.json'));
const ts = require('typescript');
const { chromium } = createRequire(import.meta.url)('playwright');

// ---- the sound data, through insert-coin's own Vite (as the trace recorder does)
const { createServer } = await import(
  pathToFileURL(join(root, 'node_modules/vite/dist/node/index.js')).href
);
const vite = await createServer({
  root,
  logLevel: 'error',
  server: { middlewareMode: true, hmr: false },
  appType: 'custom',
});
const { sounds } = await vite.ssrLoadModule(join(root, 'games/space-invaders/src/sound-bank.ts'));
await vite.close();

// ---- the synthesizer: sound.ts and synth.ts with their types removed, in one script
const plain = (file) =>
  ts
    .transpileModule(readFileSync(join(root, 'packages/audio/src', file), 'utf8'), {
      compilerOptions: { target: ts.ScriptTarget.ES2022, module: ts.ModuleKind.ESNext },
    })
    .outputText.replace(/^import[\s\S]*?;\s*$/gm, '')
    .replace(/^export /gm, '');
const synth = `${plain('sound.ts')}\n${plain('synth.ts')}\nwindow.playEffect = playEffect;`;

// ---- render every effect, 1.2 s at 44.1 kHz, one channel
const RATE = 44100;
const browser = await chromium.launch();
const page = await browser.newPage();
await page.addScriptTag({ content: synth });
mkdirSync(outDir, { recursive: true });
for (const [name, effect] of Object.entries(sounds)) {
  const samples = await page.evaluate(
    async ({ effect, rate }) => {
      const ctx = new OfflineAudioContext(1, Math.round(rate * 1.2), rate);
      window.playEffect(ctx, effect);
      const buffer = await ctx.startRendering();
      return Array.from(buffer.getChannelData(0));
    },
    { effect, rate: RATE },
  );
  writeFileSync(join(outDir, `${name}.wav`), wav(samples, RATE));
  console.log(`${name}: ${samples.length} samples`);
}
await browser.close();

/** A 16-bit mono WAV file of samples between -1 and 1. */
function wav(samples, rate) {
  const data = Buffer.alloc(samples.length * 2);
  samples.forEach((s, i) => data.writeInt16LE(Math.round(Math.max(-1, Math.min(1, s)) * 32767), i * 2));
  const header = Buffer.alloc(44);
  header.write('RIFF', 0);
  header.writeUInt32LE(36 + data.length, 4);
  header.write('WAVEfmt ', 8);
  header.writeUInt32LE(16, 16);
  header.writeUInt16LE(1, 20);
  header.writeUInt16LE(1, 22);
  header.writeUInt32LE(rate, 24);
  header.writeUInt32LE(rate * 2, 28);
  header.writeUInt16LE(2, 32);
  header.writeUInt16LE(16, 34);
  header.write('data', 36);
  header.writeUInt32LE(data.length, 40);
  return Buffer.concat([header, data]);
}
