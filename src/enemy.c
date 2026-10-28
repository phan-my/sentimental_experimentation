/*
 * enemy.c contains all functions for enemies.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include "enemy.h"
#include <stdarg.h>
#include "collision.h"
#include "stage.h"
#include "pointer.h"

/* GLOBALS */

struct enemy fairy;
struct enemy fairy_blue;

/* FUNCTIONS */

/* initialization */

// initializes enemylist with enemyIDs
void initialize_enemies()
{
	int i, j;
	
	// red fairy 
	fairy.id = ENEMY_FAIRY;
	fairy.motion.hb.r = 10.;
	fairy.health = 2;
	fairy.item = ITEM_POWERUP;
	fairy.item_multiple = 1;
	enemylist[fairy.id][0] = fairy;
	
	// blue fairy
	fairy_blue.id = ENEMY_FAIRY_BLUE;
	fairy_blue.motion.hb.r = 10.;
	fairy_blue.health = 2;
	fairy_blue.item = ITEM_SCORE;
	fairy_blue.item_multiple = 1;
	enemylist[fairy_blue.id][0] = fairy_blue;
	
	// create multiple of all unique enemies
	for (i = 0; i < MAX_ENEMIES; i++) {
		// create multiples per enemyID
		for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
			enemylist[i][j] = enemylist[i][0];
			enemylist[i][j].active = false;
			enemylist[i][j].grazed = false;
		}

		// set smart pointers to 0
		nth_enemy[i] = 0;
	}
	
}

// unloads entities if they go off-screen (automatic)
void unload_offscreen_enemies()
{
	int i, j;

	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		if (in_unload_zone(enemylist[i][j].motion))
			enemylist[i][j].active = false;
	}
	}
}

// update all positional values of all active enemies
void update_enemy_motion()
{
	int i, j;
	
	// loop through and update all enemies
	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		// v -> p
		update_motion(&enemylist[i][j].motion);
	}
	}
	
	// if enemy falls off-screen, unload
	unload_offscreen_enemies();
}

void do_enemy_shot()
{
}

// includes all operations of enemies
void do_enemy()
{
	update_enemy_motion();
}

/* deletion functions */

// deletes all enemies in circle at given coordinates with radius
// xcoord: x-position of the point
// ycoord: y-position of the point
// radius: radius (in pixels) of the circular field to clear enemies 
void delete_enemies_in_circle(double xcoord, double ycoord,
		double radius)
{
	int i, j;
	struct circlebox field;
	field.p.x = xcoord;
	field.p.y = ycoord;
	field.r = radius;

	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		// if enemy is within the circle and is active
		if (circle_in_circle(enemylist[i][j].motion.hb, field)
			&& enemylist[i][j].active) {
			// deactivate enemies in circle
			enemylist[i][j].active = false;
			// item drop
			create_item_pile(enemylist[i][j]);
		}
	}
	}
}

/* enemy starters (no position specified) */

// sets velocities of a single enemy based on given angle for constant motion
// *enemy: passing struct enemy by reference
// speed: scalar speed of the enemy in px/fr
// radians: measure angle between positive x-axis and direction of velocity
void enemy_start_straight(struct enemy *enemy, double speed, double radians)
{
	// straight enemy: use appropriate operations
	enemy -> motion.curved = false;

	// set enemy speed, with individual v_x and v_y vectors
	set_velocity(&(*enemy).motion.v0, speed * cos(radians),
			speed * sin(radians));
	set_velocity(&(*enemy).motion.v, speed * cos(radians),
			speed * sin(radians));
	// constant motion, no acceleration
	set_acceleration(&(*enemy).motion.a0, 0, 0);
	set_acceleration(&(*enemy).motion.a, 0, 0);
	enemy -> motion.v_max = speed;
	
	// set initial time
	enemy -> motion.time_0 = stage_progress_frames;
	
	// activate enemy
	enemy -> grazed = false;
	enemy -> active = true;
}

// sets velocities of a single enemy based on given angle for constant motion
// *enemy: passing struct enemy by reference
// speed: scalar speed of the enemy in px/fr
// acceleration: scalar acceleration of the enemy in px/fr^2
// max_speed: maximum speed of enemy (regardless of acceleration)
// radians: measure angle between positive x-axis and direction of velocity
void enemy_start_accelerated(struct enemy *enemy, double speed, double acceleration,
		double max_speed, double radians)
{
	// straight enemy: use appropriate operations
	enemy -> motion.curved = false;
	
	// set enemy speed, with individual v_x and v_y vectors
	set_velocity(&(*enemy).motion.v0, speed * cos(radians),
			speed * sin(radians));
	// TODO: optimize by creating copy_velocity(b, a)
	set_velocity(&(*enemy).motion.v, speed * cos(radians),
			speed * sin(radians));
	// set constant acceleration, with individual a_x and a_y vectors
	set_acceleration(&(*enemy).motion.a0,
			acceleration * cos(radians),
			acceleration * sin(radians));
	set_acceleration(&(*enemy).motion.a, acceleration * cos(radians),
			acceleration * sin(radians));
	
	// set maximum speed that nullifies acceleration
	enemy -> motion.v_max = max_speed;
	
	// set initial time
	enemy -> motion.time_0 = stage_progress_frames;
	
	// activate enemy
	enemy -> active = true;
	enemy -> grazed = false;
}

/* simple groups */

// creates an enemy moving in specified direction 
// type: enemy type as specified in enemy.h, e.g. ENEMY_FAIRY_BLUE
// xcoord: origin position on x-axis
// ycoord: origin position on y-axis
// speed: scalar speed of enemy in px/fr
// angle: the radians clockwise from the x-axis that the enemy heads to
void enemy_launch_straight(int type, double xcoord, double ycoord, double speed,
		double angle)
{
	// cycles pointer to next inactive element of enemy array
	cycle_enemy_smart_pointer(type);

	// set spawner position of the enemies
	set_position(&enemylist[type][nth_enemy[type]].motion.o,
			xcoord, ycoord);
	// set current position of enemy
	set_position(&enemylist[type][nth_enemy[type]].motion.hb.p,
			xcoord, ycoord);

	// launch each individual enemy
	enemy_start_straight(&enemylist[type][nth_enemy[type]], speed, angle);
}


