// SPDX-License-Identifier: GPL-3.0-only
/**
 * A fake FM-1 behind a fake WebMIDI, for the installer's tests: it answers the identity query and, after the
 * upgrade message, asks for pieces of the package as the real one does (after FakeFM1 in
 * platform/tests/install_test.py). Step 1 (the running firmware) reads a few pieces and asks for the "check"
 * finish, then the FM-1 comes back as the update loader under another USB name; step 2 (the loader) reads the
 * whole image upward and asks for the "write" finish, then the FM-1 restarts with the new identity.
 *
 * Plain JavaScript with no imports: tools/installer/test.mjs imports it in Node, tools/installer/page-test.mjs
 * puts it in the browser page before the installer runs.
 */

const HS_QUERY = [0xf0, 0x00, 0x32, 0x45, 0x00, 0x00, 0x00, 0x40, 0x7f, 0xf7];
const UPGRADE = [0xf0, 0x22, 0x24, 0x35, 0x7f, 0xf7];
const FINISH_CHECK = 0xe0000000;
const FINISH_WRITE = 0xf0000000;

const pack7 = (data) => {
  const out = [];
  let acc = 0;
  let bits = 0;
  for (const byte of data) {
    acc |= byte << bits;
    bits += 8;
    while (bits >= 7) {
      out.push(acc & 0x7f);
      acc >>>= 7;
      bits -= 7;
    }
  }
  if (bits) out.push(acc & 0x7f);
  return out;
};

const unpack7 = (data) => {
  const out = [];
  let acc = 0;
  let bits = 0;
  for (const byte of data) {
    acc |= (byte & 0x7f) << bits;
    bits += 7;
    while (bits >= 8) {
      out.push(acc & 0xff);
      acc >>>= 8;
      bits -= 8;
    }
  }
  return out;
};

const same = (a, b) => a.length === b.length && a.every((x, i) => x === b[i]);

/**
 * One fake FM-1 and the MIDIAccess that sees it.
 * @param {Uint8Array} image the logical image of the package it should be sent (to check every answer)
 * @param {object} [options]
 * @param {string} [options.identity] what it runs at first ("FM-1_015": the official V15)
 * @param {string} [options.name] its USB name at first
 * @param {string} [options.loader] the update loader's identity
 * @param {string} [options.after] the identity once the package is written
 * @param {number} [options.unplugAfter] pulls the cable after this many answers of the loader...
 * @param {number} [options.replugMs] ...and plugs it back after this long (the loader again)
 * @param {boolean} [options.windowsNames] output names as Windows gives them ("MIDIOUT2 (Felucca)")
 * @returns {{ access: object, device: object }}
 */
export const createFakeFm1 = (image, options = {}) => {
  const device = {
    identity: options.identity ?? "FM-1_015",
    name: options.name ?? "FM-1",
    connected: true,
    served: 0,
    bad: 0,
    upgrades: 0,
    unplugged: 0,
    history: [],
    queue: [],
    waiting: null,
    ports: null,
  };

  const deliver = (packet) => {
    const input = device.ports?.input;
    setTimeout(() => {
      if (input && input.state === "connected" && input.onmidimessage) input.onmidimessage({ data: Uint8Array.from(packet) });
    }, 1);
  };

  const ask = () => {
    const next = device.queue.shift();
    if (!next) return;
    device.waiting = next;
    const [addr, n] = next;
    const body = [0, 0x59, 0x30, 0, 0, 0, 0, addr & 0xff, (addr >>> 8) & 0xff, (addr >>> 16) & 0xff, addr >>> 24, n & 0xff, n >> 8, 0];
    body.push(~body.slice(6, 14).reduce((s, x) => s + x, 0) & 0xff);
    deliver([0xf0, ...pack7(body), 0xf7]);
  };

  const unplug = () => {
    device.connected = false;
    device.unplugged++;
    if (device.ports) {
      device.ports.input.state = "disconnected";
      device.ports.output.state = "disconnected";
    }
    device.ports = null;
  };

  /** USB re-enumeration: the old ports go, new ones with the new name come. */
  const boot = (identity, name) => {
    unplug();
    device.unplugged--;
    device.identity = identity;
    device.name = name;
    device.connected = true;
    device.queue = [];
    device.waiting = null;
    device.history.push(identity);
    const input = { name, state: "connected", onmidimessage: null, open: async () => input };
    const output = {
      name: options.windowsNames ? `MIDIOUT2 (${name})` : name,
      state: "connected",
      open: async () => output,
      send: (bytes) => {
        if (output.state !== "connected") throw new Error("disconnected");
        receive(Array.from(bytes));
      },
    };
    device.ports = { input, output };
  };

  const receive = (packet) => {
    if (same(packet, HS_QUERY)) {
      const text = [...device.identity].map((c) => c.charCodeAt(0));
      deliver([0xf0, ...pack7([0, 0x59, 0x11, 0, 0, 0, ...text, ...new Array(28 - text.length).fill(0)]), 0xf7]);
      return;
    }
    if (same(packet, UPGRADE)) {
      device.upgrades++;
      const loader = device.identity.startsWith("ota-");
      const pieces = [];
      if (loader) for (let addr = 0; addr < image.length; addr += 512) pieces.push([addr, Math.min(512, image.length - addr)]);
      else pieces.push([0, 64], [0x40, 160], [0x1000, 512]);
      device.queue = [...pieces, [loader ? FINISH_WRITE : FINISH_CHECK, 8]];
      ask();
      return;
    }
    if (!device.waiting) return;
    const [addr, n] = device.waiting;
    const answer = unpack7(packet.slice(1, -1));
    const finish = addr >= FINISH_CHECK;
    const want = finish ? [...new TextEncoder().encode("success"), 0] : Array.from(image.subarray(addr, addr + n));
    if (answer.length !== 15 + want.length || !same(answer.slice(14, 14 + want.length), want)) device.bad++;
    device.waiting = null;
    device.served++;
    if (device.identity.startsWith("ota-") && options.unplugAfter && device.served === options.unplugAfter) {
      unplug();
      const identity = device.identity;
      const name = device.name;
      setTimeout(() => boot(identity, name), options.replugMs ?? 500);
      return;
    }
    if (addr === FINISH_CHECK) setTimeout(() => boot(options.loader ?? "ota-FM-1_950", "Felucca Update"), 50);
    else if (addr === FINISH_WRITE) setTimeout(() => boot(options.after ?? "FM-1_950", "Felucca"), 50);
    else ask();
  };

  boot(device.identity, device.name);
  device.history = [device.identity];

  /** A MIDIAccess: the ports there are now, and a piano that is always plugged in (never probed). */
  const piano = {
    input: { name: "Digital Piano", state: "connected", onmidimessage: null, open: async () => piano.input },
    output: { name: "Digital Piano", state: "connected", open: async () => piano.output, send: () => {} },
  };
  const ports = (kind) => {
    const list = [[piano.input.name, piano[kind]]];
    if (device.ports) list.push([`fm1-${kind}`, device.ports[kind]]);
    return new Map(list);
  };
  const access = {
    get inputs() {
      return ports("input");
    },
    get outputs() {
      return ports("output");
    },
  };
  return { access, device };
};
