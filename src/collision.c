/*
 * collision.c contains hitbox interaction operations.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */
#include <stdbool.h>
#include <math.h>
#include "collision.h"
#include "logic.h"
#include "common.h"
#include "shot.h"
#include "random.h"
#include "pointer.h"
#include "sounds.h"
#include "item.h"

/* FUNCTIONS */

// returns Boolean value on whether two circle hitboxes overlap
// dest: second circle hitbox (circlebox)
// src: first circle hitbox
bool circle_in_circle(struct circlebox dest, struct circlebox src)
{
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
	// formula without square root for optimized computability
	double dx = src.p.x - dest.p.x;
	double dy = src.p.y - dest.p.y;
	double distance = dx * dx + dy * dy;
	return distance <= pow(src.r + dest.r, 2);
}

// UNUSED: returns Boolean value on whether a point lies in a rectangle region
// x: x-position of the point
// y: y-position of the point
// obj: the rectangular region
bool _in_rect(double x, double y, struct rectbox obj)
{
	// https://silentmatt.com/rectangle-intersection/
	// conditions for being true
	if (obj.x <= x && x <= obj.x + obj.w
			&& obj.y <= y && y <= obj.y + obj.h)
		return true;
	// otherwise
	else
		return false;
}

// UNUSED: returns Boolean value on whether two rectangular hitboxes overlap
// obj_1: 1st rectangular hitbox
// obj_2: 2nd rectangular hitbox
bool _rect_in_rect(struct rectbox obj_1, struct rectbox obj_2)
{
	bool in_1 = obj_1.x <= obj_2.x + obj_2.w; // obj_2 next of obj_1
	bool in_2 = obj_1.y <= obj_2.y + obj_2.h; // obj_2 over obj_1
	bool in_3 = obj_1.x + obj_1.w >= obj_2.x; // obj_1 left of obt_2
	bool in_4 = obj_1.y + obj_1.h >= obj_2.y; // obj_2 over obj_1
	if (in_1 && in_2 && in_3 && in_4)
		return true;
	else
		return false;
}

// if player hits an enemy, then kill enemy, play sound & activate deathbomb
// 	window
void player_enemy_collision()
{
	int i, j;

	// loop through entire enemylist for each enemy
	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		// checks collision
		if (circle_in_circle(enemylist[i][j].motion.hb,
					player[curr_player].motion.hb)
				&& enemylist[i][j].active) {
			player[curr_player].deathbomb_active = true;
			player[curr_player].deathbomb_timer
				= MAX_DEATHBOMB_RELOAD;
			enemylist[i][j].active = false; // kill enemy
			player[curr_player].invincible = true; // iframe
			play_sfx("sfx/death.ogx"); // death sound
			// optimization: cancel entire loop if player is hit
			goto end_loop;
		}
		
		// grazing
		if (circle_in_circle(enemylist[i][j].motion.hb,
					player[curr_player].grazebox)
				&& enemylist[i][j].active
				&& !player[curr_player].invincible
				&& !enemylist[i][j].grazed) {
			player[curr_player].graze++;
			enemylist[i][j].grazed = true;
			play_sfx("sfx/graze.ogx"); // graze sound
			
			// optimization: cancel entire loop if player is hit
			goto end_loop;
		}
	}
	}
end_loop:
}

// if player hits an bullet, then kill bullet, play sound & activate deathbomb
// 	window
void player_bullet_collision()
{
	int i, j;
	
	// loop through entire bulletlist for each bullet multiple
	for (i = 0; i < MAX_BULLETS; i++) {
	for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
		// collision
		if (circle_in_circle(bulletlist[i][j].motion.hb,
					player[curr_player].motion.hb)
				&& bulletlist[i][j].active) {
			player[curr_player].deathbomb_active = true;
			player[curr_player].deathbomb_timer
				= MAX_DEATHBOMB_RELOAD;
			bulletlist[i][j].active = false; // kill bullet
			player[curr_player].invincible = true; // iframe
			play_sfx("sfx/death.ogx"); // death sound
			// optimization: cancel entire loop if player is hit
			goto end_loop;
		}

		// grazing
		if (circle_in_circle(bulletlist[i][j].motion.hb,
					player[curr_player].grazebox)
				&& bulletlist[i][j].active
				&& !player[curr_player].invincible
				&& !bulletlist[i][j].grazed) {
			player[curr_player].graze++;
			bulletlist[i][j].grazed = true;
			play_sfx("sfx/graze.ogx"); // graze sound
			
			// optimization: cancel entire loop if player is hit
			goto end_loop;
		}
	}
	}
end_loop:
}

// if player hits an item, then delete item & increase stats
void player_item_collision()
{
	int i, j;
	
	// loop through entire itemlist for each item multiple
	for (i = 0; i < MAX_ITEMS; i++) {
	for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
		if (circle_in_circle(itemlist[i][j].motion.hb,
					player[curr_player].itembox)
				&& itemlist[i][j].active) {
			itemlist[i][j].active = false;
			switch (i) {
			// increases player's power on collecting powerups
			case ITEM_POWERUP:
				player[curr_player].power
					+= itemlist[i][j].value;
				break;
			// increases player's points on collecting powerus
			case ITEM_SCORE:
				player[curr_player].score
					+= itemlist[i][j].value;
				break;
			default:
				break;
			}
			
			// optimization: cancel entire loop on collision 
			goto end_loop;
		}
	}
	}
end_loop:
}

// if shot hits an enemy, then kill shot, deplete enemy health
void shot_enemy_collision()
{
	int i, j, k, l;
	
	// player bullet hitting enemies
	for (i = 0; i < MAX_SHOTS; i++) {
	for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
	for (k = 0; k < MAX_ENEMIES; k++) {
	for (l = 0; l < MAX_ENEMY_MULTIPLE; l++) {
		// player bullet hits fairy
		// fairy takes damage
		if (shotlist[i][j].active && enemylist[k][l].active
				&&
				circle_in_circle(enemylist[k][l].motion.hb,
					shotlist[i][j].motion.hb)) {
//				printf("PLAYER BULLET HTIS FAIRY\n");

			// damage dealt based on player bullet power
			enemylist[k][l].health -= shotlist[i][j].power;

			// unload bullet
			shotlist[i][j].active = false;

			// enemy dies, drops item
			if (enemylist[k][l].health == 0) {
				enemylist[k][l].active = false;
				// item drop
				create_item_pile(enemylist[k][l]);
			}

			// optimization: cancel entire loop if enemy is hit
			goto end_loop;
		}
	}
	}
	}
	}
end_loop:
}

// main collision detection logic
// TODO: quadtree hitbox detection: github.com/arpit2297/Collision-Detection-using-Quad-Trees
void do_collision()
{
	/* player hitting bullet or enemy */
	
	// if not invincible, do collision
	// questions/21650246/sdl-2-collision-detetection
	if (!player[curr_player].invincible) {
		// player -- bullets
		player_bullet_collision();
		// player -- enemies
		player_enemy_collision();
	// if player is invincible, count down iframes timer and deathbomb timer
	} else if (player[curr_player].iframes > 0) {	// decrease iframe
		// invincibility timer
		player[curr_player].iframes--;
		
		// if deathbomb timer runs out, kill player
		if (player[curr_player].deathbomb_timer < 0 && 
				player[curr_player].deathbomb_active) {
			player[curr_player].health--; // decrease HP
			reset_player_position(); // put player at origin
			
			// resets deathbomb
			player[curr_player].deathbomb_timer
				= MAX_DEATHBOMB_RELOAD;
			player[curr_player].deathbomb_active = false;
		// count down deathbomb timer
		} else {
			player[curr_player].deathbomb_timer--;
		}
	// if iframe is emptied: reset iframe
	} else {
		player[curr_player].invincible = false;
		player[curr_player].iframes = MAX_IFRAMES;
	}

	/* player collects items */

	// player -- items
	player_item_collision();

	/* player bullet hitting enemies */
	
	// shot -- enemy
	shot_enemy_collision();
}

