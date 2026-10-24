# SPDX-License-Identifier: GPL-3.0-only
# Everything that runs on a computer, no FM-1 and no JieLi toolchain needed:
#   make test                    every *_test.c of libs/ and games/, built and run
#   make host GAME=space-invaders the game on a fake FM-1 (Dinghy's host mode): screens as PNG, sound as WAV
#   make format / format-check   clang-format on our sources (platform/ is Dinghy's and keeps its style)
#   make traces INSERT_COIN=dir  the reference traces, recorded again from a checkout of insert-coin
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

.PHONY: test host format format-check traces clean

test: $(TESTS:%.c=$(OUT)/%)
	@set -e; for t in $^; do echo "== $$t"; $$t; done

# a test links its own module (keymap_test.c -> keymap.c) and the other modules of its folder
$(OUT)/%_test: %_test.c $(SOURCES)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $< $(filter-out $(dir $<)main.c %_test.c,$(wildcard $(dir $<)*.c))

# the game on the host: main.c includes platform.c, which includes platform/tests/host.c
host:
	@mkdir -p $(OUT)
	$(CC) -O2 -Wall -Wno-unused-function -DDINGHY_HOST $(HOST_DEFS) \
		-Iplatform/firmware/hal -Iplatform/firmware/src -Iplatform/firmware/gen -Ilibs \
		-o $(OUT)/$(GAME) games/$(GAME)/src/main.c \
		$(filter-out %/main.c %_test.c,$(wildcard games/$(GAME)/src/*.c) $(shell find libs -name '*.c'))
	$(OUT)/$(GAME) $(OUT)/$(GAME) $(SCRIPT)
	python3 tools/ppm_to_png.py $(OUT)/$(GAME)

traces:
	node tools/traces/record-space-invaders.mjs $(INSERT_COIN)

format:
	clang-format -i $(SOURCES)

format-check:
	clang-format --dry-run --Werror $(SOURCES)

clean:
	rm -rf build
