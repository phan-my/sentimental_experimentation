/*
 * shot.h is the headerfile for shot.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef SHOT_H
#define SHOT_H

// includes
#pragma once

// macros
#define MAX_SHOTS 2
#define MAX_SHOT_MULTIPLE 2048

#define SHOT_REIMU 0
#define SHOT_MARISA 1

#define MAX_SHOT_RELOAD 3
#define MAX_BOMB_RELOAD 300 // 5s

#define BOMB_RADIUS 256

// globals
extern int shot_reload;
extern int bomb_reload;

// functions
void activate_reimu_shot();

#endif /* SHOT_H */
