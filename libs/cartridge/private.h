/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * How three games live in one firmware without their names clashing.
 */
#ifndef CARTRIDGE_PRIVATE_H
#define CARTRIDGE_PRIVATE_H

/**
 * Three games in one firmware means three `game_step`s, three `render_game`s, three `CONTROLS`:
 * the same names, since every game follows its own web version. So each game is compiled as a
 * single file (its unity.c includes all its modules, with GAME_AMALGAMATION defined) where its
 * functions and tables are `static`: invisible to the other games, which can use the same names.
 * Only its cartridge is public. Outside unity.c (the tests, one module at a time) everything is
 * public as usual.
 *
 * GAME_PRIVATE goes before a function declared in a game's header and before a table defined in
 * a game's .c file; GAME_EXTERN before a table declared in a header.
 */
#ifdef GAME_AMALGAMATION
#define GAME_PRIVATE static
#define GAME_EXTERN static
#else
#define GAME_PRIVATE
#define GAME_EXTERN extern
#endif

#endif
