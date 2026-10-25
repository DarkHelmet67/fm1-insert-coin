# SPDX-License-Identifier: GPL-3.0-only
"""Writes a small fake .fwsc for the installer's tests: a valid header, a file list with one file and its CRC,
Felucca's loader marker and the identity in the marker bytes, as fm1pkg_make writes a package (after
make_package in platform/tests/install_test.py). It installs nothing real: the fake FM-1 of the tests reads it.

    python3 tools/installer/fake_package.py PRODUCT OUT.fwsc [SIZE]
"""
import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("fm1_install", ROOT / "platform/tools/fm1_install.py")
I = importlib.util.module_from_spec(spec)
spec.loader.exec_module(I)


def package(product, size=0x6000):
    """the .fwsc bytes: the logical image with one marker byte after each of the first 20 blocks"""
    img = bytearray((i * 7) & 0xFF for i in range(size))
    img[0x1800:0x1800 + 16] = I.LOADER_MARK
    data = bytes(img[0x400:])
    e = bytearray(0x50)
    e[4:6] = I._crc16(data).to_bytes(2, "little")
    e[8:12] = (0x400).to_bytes(4, "little")
    e[12:16] = len(data).to_bytes(4, "little")
    e[0x40:0x49] = b"flash.bin"
    lst = I._dec(bytes(e))
    hdr = bytearray(0x40)
    hdr[4:8] = size.to_bytes(4, "little")
    hdr[8:10] = (1).to_bytes(2, "little")
    hdr[2:4] = I._crc16(lst).to_bytes(2, "little")
    hdr[0:2] = I._crc16(bytes(hdr[2:0x40])).to_bytes(2, "little")
    img[0:0x40] = I._dec(bytes(hdr))
    img[0x40:0x90] = lst
    raw = bytearray()
    for i in range(I.BLOCKS):
        mark = (ord(product[i]) + i + 1) & 0xFF if i < len(product) else 0x7D
        raw += img[i * I.KEEP:(i + 1) * I.KEEP] + bytes([mark])
    raw += img[I.BLOCKS * I.KEEP:]
    return bytes(raw)


if __name__ == "__main__":
    if len(sys.argv) not in (3, 4):
        sys.exit(__doc__)
    raw = package(sys.argv[1], int(sys.argv[3], 0) if len(sys.argv) == 4 else 0x6000)
    Path(sys.argv[2]).parent.mkdir(parents=True, exist_ok=True)
    Path(sys.argv[2]).write_bytes(raw)
    assert I.product_of(raw) == sys.argv[1] and I.package_damage(I.deinterleave(raw)[0]) is None
