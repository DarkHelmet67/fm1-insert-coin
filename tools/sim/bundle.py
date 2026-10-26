# SPDX-License-Identifier: GPL-3.0-only
"""Builds the browser simulator's page: one HTML file with everything inside (style, script, and the firmware's
WebAssembly as base64), so it also opens from disk.

    python3 tools/sim/bundle.py WASM OUT_DIR

Writes OUT_DIR/simulator/index.html and OUT_DIR/index.html, the site's front page with the games (and the link to
the installer, when tools/installer/bundle.py has put it in OUT_DIR).
"""
import base64
import html
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

HELP = (
    "Menu: turn SELECT, PRESETS or ALGORITHM to choose, PLAY to start; hold PLAY in a game to come back. "
    "Space Invaders: OP1-OP3 left, OP4-OP5 fire, OP6-PIT-GLO right, FX colour or black and white. "
    "Breakout: any knob. Turn a knob with the mouse wheel or by dragging it. "
    "From a computer: Up and Down choose, arrows or A/D steer, space fires, Enter is PLAY, V is FX."
)

#: The folder of the simulator on the site.
SIMULATOR = "simulator"


def firmware_info():
    """the launcher's firmware.json: the package name and its screen name"""
    return json.loads((ROOT / "launcher" / "firmware.json").read_text())


def page(wasm):
    shell = (ROOT / "sim" / "shell.html").read_text()
    parts = {
        "{{TITLE}}": html.escape(firmware_info()["screen_name"]),
        "{{HELP}}": html.escape(HELP),
        "{{STYLE}}": (ROOT / "sim" / "style.css").read_text(),
        "{{SCRIPT}}": (ROOT / "sim" / "sim.js").read_text(),
        "{{WASM}}": base64.b64encode(Path(wasm).read_bytes()).decode(),
    }
    for key, value in parts.items():
        shell = shell.replace(key, value)
    return shell


def index(out):
    games = firmware_info()["games"]
    items = "\n".join("<li>%s</li>" % html.escape(name) for name in games.values())
    simulator = (
        f'<p><a href="{SIMULATOR}/">Play them in the browser</a>: the simulator runs the firmware, menu included.</p>\n'
        if (out / SIMULATOR / "index.html").exists()
        else ""
    )
    # the installer is on the site once tools/installer/bundle.py has put it there (with the packages)
    installer = (
        '<p><a href="installer/">Install the games on your FM-1</a>: from the browser, with a USB cable.</p>\n'
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
<p>1980s arcade games as firmware for the M-Vave FM-1 synth: one firmware, a menu, these games.
In the browser they run with the same C code as the firmware, compiled to WebAssembly.</p>
<ul>
{items}
</ul>
{simulator}{installer}<p><a href="https://github.com/DarkHelmet67/fm1-insert-coin">The source code on GitHub</a></p>
</main></body></html>
"""


def main(wasm, out_dir):
    out = Path(out_dir)
    (out / SIMULATOR).mkdir(parents=True, exist_ok=True)
    (out / SIMULATOR / "index.html").write_text(page(wasm))
    (out / "index.html").write_text(index(out))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(*sys.argv[1:])
