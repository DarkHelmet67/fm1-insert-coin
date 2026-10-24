#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Turn the host mode's screens (PREFIX-*.ppm) into PNGs at twice the size, as Dinghy's tests/run_host.sh does.

  tools/ppm_to_png.py PREFIX
"""
import sys
from pathlib import Path

from PIL import Image

prefix = Path(sys.argv[1])
for ppm in sorted(prefix.parent.glob(prefix.name + "-*.ppm")):
    png = ppm.with_suffix(".png")
    Image.open(ppm).resize((480, 480), Image.NEAREST).save(png)
    ppm.unlink()
    print("screen ->", png)
