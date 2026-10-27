/*
 * enemy.c contains all functions for enemies.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include "enemy.h"
#include "collision.h"

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
	fairy.health = 4;
	fairy.item = ITEM_POWERUP;
	fairy.item_multiple = 1;
	enemylist[fairy.id][0] = fairy;
	
	// blue fairy
	fairy_blue.id = ENEMY_FAIRY_BLUE;
	fairy_blue.motion.hb.r = 10.;
	fairy_blue.health = 4;
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
		if (circle_in_circle(enemylist[i][j].motion.hb, field))
			// deactivate enemies in circle
			enemylist[i][j].active = false;
	}
	}
}


