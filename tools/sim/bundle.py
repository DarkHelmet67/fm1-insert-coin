# SPDX-License-Identifier: GPL-3.0-only
"""Builds the browser simulator's page of a game: one HTML file with everything inside
(style, script, and the game's WebAssembly as base64), so it also opens from disk.

    python3 tools/sim/bundle.py GAME WASM OUT_DIR

Writes OUT_DIR/GAME/index.html and OUT_DIR/index.html, the list of every game in OUT_DIR (and the link to the
installer, when tools/installer/bundle.py has put it in OUT_DIR).
"""
import base64
import html
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

HELP = (
    "Synth keys: OP1-OP3 left, OP4-OP5 fire, OP6-PIT-GLO right, PLAY to start, "
    "FX colour or black and white. From a computer: arrows or A/D, space, Enter, V."
)


def page(game, wasm):
    info = json.loads((ROOT / "games" / game / "firmware.json").read_text())
    shell = (ROOT / "sim" / "shell.html").read_text()
    parts = {
        "{{TITLE}}": html.escape(info["screen_name"]),
        "{{GAME}}": game,
        "{{HELP}}": html.escape(HELP),
        "{{STYLE}}": (ROOT / "sim" / "style.css").read_text(),
        "{{SCRIPT}}": (ROOT / "sim" / "sim.js").read_text(),
        "{{WASM}}": base64.b64encode(Path(wasm).read_bytes()).decode(),
    }
    for key, value in parts.items():
        shell = shell.replace(key, value)
    return shell


def index(out):
    games = sorted(p.parent.name for p in out.glob("*/index.html") if (ROOT / "games" / p.parent.name).is_dir())
    items = "\n".join(
        '<li><a href="%s/">%s</a></li>'
        % (g, html.escape(json.loads((ROOT / "games" / g / "firmware.json").read_text())["screen_name"]))
        for g in games
    )
    # the installer is on the site once tools/installer/bundle.py has put it there (with the packages)
    installer = (
        '<p><a href="installer/">Install a game on your FM-1</a>: from the browser, with a USB cable.</p>\n'
        if (out / "installer" / "index.html").exists()
        else ""
    )
    return f"""<!doctype html>
<html lang="en">
<head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>fm1-insert-coin</title>
<style>
:root {{ --page: #0e0f11; --text: #e8e8ea; --muted: #9a9ba3; --link: #7fd17f; }}
body {{ margin: 0; padding: 32px 16px; background: var(--page); color: var(--text); font: 16px/1.5 system-ui, sans-serif; }}
main {{ max-width: 640px; margin: 0 auto; }}
a {{ color: var(--link); }}
p {{ color: var(--muted); }}
</style></head>
<body><main>
<h1>fm1-insert-coin</h1>
<p>1980s arcade games as firmware for the M-Vave FM-1 synth. Here they run in the browser,
with the same C code as the firmware, compiled to WebAssembly.</p>
<ul>
{items}
</ul>
{installer}<p><a href="https://github.com/DarkHelmet67/fm1-insert-coin">The source code on GitHub</a></p>
</main></body></html>
"""


def main(game, wasm, out_dir):
    out = Path(out_dir)
    (out / game).mkdir(parents=True, exist_ok=True)
    (out / game / "index.html").write_text(page(game, wasm))
    (out / "index.html").write_text(index(out))


if __name__ == "__main__":
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
