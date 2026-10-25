# SPDX-License-Identifier: GPL-3.0-only
"""Puts the web installer and the firmware's packages on the site.

    python3 tools/installer/bundle.py OUT_DIR PACKAGE.fwsc...

Each package is checked as Dinghy's installer checks it (platform/tools/fm1_install.py: identity, Felucca's
update loader inside, header and files with their CRCs) and copied to OUT_DIR/firmware/PACKAGE.fwsc. The page
(installer/) goes to OUT_DIR/installer/ with games.json, the list the page offers: name, the games inside,
identity, file, SHA-256 and size of each package. Then OUT_DIR/index.html is written again, now with the
installer's link.

PACKAGE is the package's file name without .fwsc, as tools/build.py writes it (build/insert-coin/insert-coin.fwsc);
its name and games come from launcher/firmware.json.
"""
import hashlib
import importlib.util
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PAGE_FILES = ["index.html", "style.css", "installer.js", "package.js", "texts.js", "vendor/fm1ota.js", "vendor/fm1pkg.js"]


def module(name, path):
    """a Python file of the repository, loaded as a module (the tools are scripts, not a package)"""
    spec = importlib.util.spec_from_file_location(name, ROOT / path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


fm1_install = module("fm1_install", "platform/tools/fm1_install.py")
sim_bundle = module("sim_bundle", "tools/sim/bundle.py")


def entry(fwsc, out):
    """checks one package, copies it to out/firmware/ and returns its line of games.json"""
    game = fwsc.stem
    info = json.loads((ROOT / "launcher" / "firmware.json").read_text())
    try:
        product, _ = fm1_install.load_package(str(fwsc), False)
    except fm1_install.InstallError as e:
        sys.exit(f"{fwsc}: {e}")
    raw = fwsc.read_bytes()
    shutil.copyfile(fwsc, out / "firmware" / f"{game}.fwsc")
    return {
        "game": game,
        "name": info["screen_name"],
        "games": list(info["games"].values()),
        "product": product,
        "pkg": f"../firmware/{game}.fwsc",
        "sha256": hashlib.sha256(raw).hexdigest(),
        "size": len(raw),
    }


def main(out_dir, packages):
    out = Path(out_dir)
    (out / "firmware").mkdir(parents=True, exist_ok=True)
    (out / "installer" / "vendor").mkdir(parents=True, exist_ok=True)
    games = sorted((entry(Path(p), out) for p in packages), key=lambda g: g["game"])
    for name in PAGE_FILES:
        shutil.copyfile(ROOT / "installer" / name, out / "installer" / name)
    (out / "installer" / "games.json").write_text(json.dumps(games, indent=2) + "\n")
    (out / "index.html").write_text(sim_bundle.index(out))
    for g in games:
        print(f"installer: {g['game']} {g['product']} {g['size']} B")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2:])
