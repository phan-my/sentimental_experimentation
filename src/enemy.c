/*
 * enemy.c contains all functions for enemies.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include "enemy.h"

/* GLOBALS */

struct enemy fairy;
struct enemy fairy_blue;

/* FUNCTIONS */
// initializes enemylist with enemyIDs
void initialize_enemies()
{
	int i, j;
	
	// red fairy 
	fairy.id = ENEMY_FAIRY;
	fairy.active = false;
	fairy.motion.hb.r = 10.;
	fairy.health = 4;
	fairy.item = ITEM_POWERUP;
	fairy.item_multiple = 1;
	enemylist[fairy.id][0] = fairy;
	
	// blue fairy
	fairy_blue.id = ENEMY_FAIRY_BLUE;
	fairy_blue.active = false;
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
}

// includes all operations of enemies
void do_enemy()
{
	update_enemy_motion();
}



