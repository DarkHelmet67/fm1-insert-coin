# SPDX-License-Identifier: GPL-3.0-only
# Everything that runs on a computer, no FM-1 and no JieLi toolchain needed:
#   make test                    every *_test.c of libs/ and games/, built and run
#   make host GAME=space-invaders the game on a fake FM-1 (Dinghy's host mode): screens as PNG, sound as WAV
#   make format / format-check   clang-format on our sources (platform/ is Dinghy's and keeps its style)
#   make traces INSERT_COIN=dir  the reference traces, recorded again from a checkout of insert-coin
#   make sim GAME=space-invaders  the browser simulator: build/site/<game>/index.html (needs clang and wasm-ld)
#   make sounds INSERT_COIN=dir  the sound effects of C and web side by side, measured (docs/06-sounds.md)
#   make installer               the web installer and the built packages on the site: build/site/installer/
#   make installer-test          the installer against a fake FM-1 (needs node)
# The firmware itself (.fwsc): python3 tools/build.py GAME (see README.md).

CC ?= cc
CFLAGS := -std=c99 -O1 -Wall -Wextra -Werror -Ilibs
GAME ?= space-invaders
SCRIPT ?= games/$(GAME)/scripts/demo.txt
OUT := build/host

# macOS: a Mach-O section attribute wants "segment,section", and Dinghy's gfx.c (like our buffers in the memory
# pool) says ".pool". On the host the pool means nothing: rename it so the same sources compile.
ifeq ($(shell uname -s),Darwin)
HOST_DEFS := -D'section(x)=section("__DATA,__pool")'
endif

# our code only: platform/ is Dinghy's, kept as it comes
SOURCES := $(shell find libs games -name '*.[ch]')
TESTS := $(shell find libs games -name '*_test.c')

.PHONY: test host format format-check traces sounds sim installer installer-test clean

test: $(TESTS:%.c=$(OUT)/%)
	@set -e; for t in $^; do echo "== $$t"; $$t; done

# the shared modules: every test links them
LIB_SOURCES := $(shell find libs -name '*.c' ! -name '*_test.c')

# a test links the shared modules and, in a game, the other modules of its folder (not main.c)
$(OUT)/%_test: %_test.c $(SOURCES)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $< $(LIB_SOURCES) \
		$(filter-out $(dir $<)main.c %_test.c $(LIB_SOURCES),$(wildcard $(dir $<)*.c))

# the game on the host: main.c includes platform.c, which includes platform/tests/host.c
host:
	@mkdir -p $(OUT)
	$(CC) -O2 -Wall -Wno-unused-function -DDINGHY_HOST $(HOST_DEFS) \
		-Iplatform/firmware/hal -Iplatform/firmware/src -Iplatform/firmware/gen -Ilibs \
		-o $(OUT)/$(GAME) games/$(GAME)/src/main.c \
		$(filter-out %/main.c %_test.c,$(wildcard games/$(GAME)/src/*.c) $(shell find libs -name '*.c'))
	$(OUT)/$(GAME) $(OUT)/$(GAME) $(SCRIPT)
	python3 tools/ppm_to_png.py $(OUT)/$(GAME)

# the browser simulator: the game's C (not main.c) for WebAssembly, then one self-contained page
WASM_CC ?= clang
sim:
	@mkdir -p build/sim
	$(WASM_CC) --target=wasm32 -std=c99 -O2 -Wall -Wextra -Werror -nostdlib -ffreestanding \
		-Ilibs -Igames/$(GAME)/src -Wl,--no-entry -Wl,-z,stack-size=131072 \
		-o build/sim/$(GAME).wasm games/$(GAME)/sim/sim.c sim/libc.c \
		$(filter-out %/main.c %_test.c,$(wildcard games/$(GAME)/src/*.c)) $(LIB_SOURCES)
	python3 tools/sim/bundle.py $(GAME) build/sim/$(GAME).wasm build/site

# the web installer (installer/) with every package built by tools/build.py (docs/09-installer.md)
FWSC ?= $(wildcard build/*/*.fwsc)
installer:
	python3 tools/installer/bundle.py build/site $(FWSC)

installer-test:
	node tools/installer/test.mjs

# every sound effect of the game from the C synthesizer and from the web version, then compared
sounds:
	@mkdir -p build/sounds/c build/sounds/web
	$(CC) $(CFLAGS) -Igames/$(GAME)/src -o build/sounds/render_sounds tools/sounds/render_sounds.c \
		$(filter-out %/main.c %_test.c,$(wildcard games/$(GAME)/src/*.c)) $(LIB_SOURCES)
	build/sounds/render_sounds build/sounds/c
	node tools/sounds/render-web-sounds.mjs $(INSERT_COIN) build/sounds/web
	python3 tools/sounds/compare.py build/sounds/web build/sounds/c

traces:
	node tools/traces/record-space-invaders.mjs $(INSERT_COIN)

format:
	clang-format -i $(SOURCES)

format-check:
	clang-format --dry-run --Werror $(SOURCES)

clean:
	rm -rf build
