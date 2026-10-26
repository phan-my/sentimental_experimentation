/*
 * enemy.h is the headerfile for enemy.h.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef ENEMY_H
#define ENEMY_H

// includes 
#include "logic.h"

// globals
extern struct enemy fairy;
extern struct enemy fairy_blue;

// functions
void initialize_enemies();
void do_enemy();

#endif /* ENEMY_H */

