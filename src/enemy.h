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

// macros
#define ENEMY_FAIRY 0
#define ENEMY_FAIRY_RED ENEMY_FAIRY
#define ENEMY_FAIRY_BLUE 1

// globals
extern struct enemy fairy;
extern struct enemy fairy_blue;

// functions
void initialize_enemies();
void do_enemy();
void delete_enemies_in_circle(double xcoord, double ycoord,
		double radius);
void enemy_start_accelerated_start_straight(struct enemy *enemy, double speed, double radians);
void enemy_start_accelerated(struct enemy *enemy, double speed, double acceleration,
		double max_speed, double radians);
void enemy_launch_straight(int type, double xcoord, double ycoord, double speed,
		double angle);

#endif /* ENEMY_H */

