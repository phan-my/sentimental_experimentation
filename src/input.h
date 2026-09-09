/*
 * input.h is the headerfile for input.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef INPUT_H
#define INPUT_H

// includes
#pragma once
#include <SDL2/SDL.h>

// variables
extern int reload;
extern int nth_player_bullet;

// functions
int do_input();
int do_main_menu_input();

#endif /* INPUT_H */
