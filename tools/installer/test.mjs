// SPDX-License-Identifier: GPL-3.0-only
/**
 * The web installer's tests in Node, against the fake FM-1 of fake_fm1.mjs: the package checks of
 * installer/package.js and whole installs with the protocol of installer/vendor/fm1ota.js (from the official
 * V15, with Windows' port names, after a pulled cable, from an update cut off earlier, on the wrong device).
 *
 *     node tools/installer/test.mjs        (about a minute: the protocol waits seconds between its steps)
 */
import { execFileSync } from "node:child_process";
import { mkdtempSync, readFileSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { Updater } from "../../installer/vendor/fm1ota.js";
import { logicalImage, sha256hex } from "../../installer/vendor/fm1pkg.js";
import { checkedImage, isOurLoader } from "../../installer/package.js";
import { createFakeFm1 } from "./fake_fm1.mjs";

let failures = 0;
const ok = (condition, what) => {
  console.log(`${condition ? "ok  " : "FAIL"} ${what}`);
  if (!condition) failures++;
};

const PRODUCT = "FM-1_950";
const dir = mkdtempSync(join(tmpdir(), "fm1-installer-"));
const fwsc = join(dir, "insert-coin.fwsc");
execFileSync("python3", ["tools/installer/fake_package.py", PRODUCT, fwsc]);
const bytes = new Uint8Array(readFileSync(fwsc));
const entry = { game: "insert-coin", name: "INSERT COIN", games: ["SPACE INVADERS"], product: PRODUCT, pkg: fwsc, sha256: await sha256hex(bytes), size: bytes.length };

// package checks
const image = await checkedImage(bytes, entry);
ok(image.length === bytes.length - 20 && image.every((x, i) => x === logicalImage(bytes)[i]), "checkedImage: the logical image");
const damaged = bytes.slice();
damaged[0x2000] ^= 1;
ok(await checkedImage(damaged, entry).then(() => false, (e) => e.code === "badpkg"), "checkedImage: a damaged file is refused");
ok(await checkedImage(bytes, { ...entry, product: "FM-1_960" }).then(() => false, (e) => e.code === "badpkg"), "checkedImage: another game's file is refused");
ok(isOurLoader({ text: "ota-FM-1_950" }) && !isOurLoader({ text: "ota-FM-1_015" }) && !isOurLoader({ text: "FM-1_950" }), "isOurLoader: Felucca's loader only");

/** Every byte of the image was asked for and answered right. */
const wroteAll = (device) => device.bad === 0 && device.history.at(-1) === PRODUCT;

// from the official firmware
{
  const { access, device } = createFakeFm1(image);
  const steps = [];
  const result = await new Updater(access).install(image, PRODUCT, (k, v) => steps.push([k, v]));
  ok(result === PRODUCT && wroteAll(device), "install from the official V15: written, the FM-1 reports FM-1_950");
  ok(device.history.join(" > ") === "FM-1_015 > ota-FM-1_950 > FM-1_950", `install: firmware > loader > game (${device.history.join(" > ")})`);
  const percents = steps.filter(([k]) => k === "write").map(([, v]) => v);
  ok(percents.length > 0 && percents.every((p, i) => i === 0 || p >= percents[i - 1]), "install: the write percentage only goes forward");
}

// Windows names the output "MIDIOUT2 (Felucca)": paired by name all the same (our change to fm1ota.js)
{
  const { access, device } = createFakeFm1(image, { identity: "FM-1_950", name: "Felucca", windowsNames: true });
  const result = await new Updater(access).install(image, PRODUCT, () => {});
  ok(result === PRODUCT && wroteAll(device), "install with Windows' port names");
}

// the cable pulled halfway through the write: the loader comes back and the write carries on
{
  const { access, device } = createFakeFm1(image, { unplugAfter: 20, replugMs: 1500 });
  const steps = [];
  const result = await new Updater(access, () => {}, { resumeWaitMs: 10000 }).install(image, PRODUCT, (k) => steps.push(k));
  ok(result === PRODUCT && wroteAll(device) && device.unplugged === 1, "install: a pulled cable, then the write resumes");
  ok(steps.includes("dropped") && steps.includes("resume"), "install: says it dropped and resumed");
}

// the page opened with the FM-1 already in our loader (an install cut off earlier)
{
  const { access, device } = createFakeFm1(image, { identity: "ota-FM-1_950", name: "Felucca Update" });
  const finished = await new Updater(access).resume(image, () => {}, PRODUCT);
  ok(finished && wroteAll(device), "resume: an update cut off earlier is finished");
}

// another keyboard: nothing is written
{
  const { access, device } = createFakeFm1(image, { identity: "SMK-37 Pro_016", name: "SMK-37 Pro" });
  const error = await new Updater(access).install(image, PRODUCT, () => {}).then(() => null, (e) => e);
  ok(error?.code === "model" && device.upgrades === 0, "install on an SMK-37: refused before anything is sent");
}

console.log(failures ? `${failures} failed` : "all passed");
process.exit(failures ? 1 : 0);
