/*
 * player.h is the headerfile for player.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef PLAYER_H
#define PLAYER_H

#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "logic.h"

// macros
#define PLAYER_REIMU 0
#define PLAYER_MARISA 1

#define MAX_IFRAMES 240 // in fr
#define MAX_DEATHBOMB_RELOAD 8 // in fr

// player spawn position
#define PLAYER_ORIGIN_X FIELD_WIDTH / 2. + FIELD_OFFSET_X
#define PLAYER_ORIGIN_Y FIELD_HEIGHT * 0.9 + FIELD_OFFSET_Y

// https://en.touhouwiki.net/wiki/User:Arcorann/Character_Speeds#Massive_chart
#define REIMU_DEFAULT_SPEED 4.5
#define REIMU_FOCUS_SPEED 2.0
#define REIMU_FOCUS_FACTOR 0.4444444

#define MARISA_DEFAULT_SPEED 5.0
#define MARISA_FOCUS_FACTOR 0.4
#define MARISA_FOCUS_SPEED 2.

// structs

// globals
extern int curr_player;


// functions
void initialize_players();
void do_player();

#endif /* PLAYER_H */
