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
#define SHOT_REIMU 0
#define SHOT_MARISA 1

#define MAX_SHOT_RELOAD 3
#define MAX_BOMB_RELOAD 300 // 5s

#define BOMB_RADIUS 300

// globals
extern struct bullet shot_reimu;
extern struct bullet shot_marisa;
extern int shot_reload;
extern int bomb_reload;


// functions
void initialize_shot();
void activate_reimu_shot();
void activate_marisa_shot();
void do_shot();

#endif /* SHOT_H */
