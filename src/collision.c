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

// table for connecting enemy to item drops



// returns Boolean value if two circle hitboxes overlap
// dest: second circle hitbox (circlebox)
// src: first circle hitbox
// TODO: Pythagorean expression optimizable?
bool circle_in_circle(struct circlebox dest, struct circlebox src)
{
	// https://silentmatt.com/rectangle-intersection/
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
	double dx = src.p.x - dest.p.x;
	double dy = src.p.y - dest.p.y;
	double distance = dx * dx + dy * dy;
	return distance <= pow(src.r + dest.r, 2);
}

// UNUSED: returns Boolean value if a point lies within a rectangle
// x: x-position of the point
// y: y-position of the point
// obj: the rectangular region
bool _in_rect(double x, double y, struct rectbox obj)
{
	if (obj.x <= x && x <= obj.x + obj.w
			&& obj.y <= y && y <= obj.y + obj.h)
		return true;
	else
		return false;
}

// UNUSED: returns Boolean value if two rectangular hitboxes overlap
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

// if player hits an enemy, then kill both, deplete player health, & reset
// position
void player_enemy_collision()
{
	int i, j;

	// loop through entire enemylist
	for (i = 0; i < MAX_ENEMIES; i++) {
		for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
			// checks collision
			if (circle_in_circle(enemylist[i][j].motion.hb,
						player[curr_player].motion.hb)
					&& enemylist[i][j].active) {
				enemylist[i][j].active = false; // kill enemy
				player[curr_player].health--; // decrease HP
				player[curr_player].invincible = true; // iframe
				reset_player_position(); // put player at origin
			}
		}
	}
}

// if player hits an bullet, then kill both, deplete player health, & reset
// position
void player_bullet_collision()
{
	int i, j;
	// loop through entire bulletlist
	for (i = 0; i < MAX_BULLETS; i++) {
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
			// check player-bullet collision
			if (circle_in_circle(bulletlist[i][j].motion.hb,
						player[curr_player].motion.hb)
					&& bulletlist[i][j].active) {
				bulletlist[i][j].active = false; // kill bullet
				player[curr_player].health--; // decrease HP
				player[curr_player].invincible = true; // iframe
				reset_player_position(); // put player at origin
			}
		}
	}
}

// sets velocities of a single item and spawn it at given position
// *item: item structure by reference
// xcoord: x-position of the spawner
// ycoord: y-position of the spawner
int create_item(struct item *item, int xcoord, int ycoord)
{
	// set initial and current positions
	set_position(&(*item).motion.o, xcoord, ycoord);
	set_position(&(*item).motion.hb.p, xcoord, ycoord);
	
	// set velocity
	double speed = 2.;
	set_velocity(&(*item).motion.v, 0, -speed);
	
	// item accelerates downwards
	set_acceleration(&(*item).motion.a, 0, 0.05);
	
	
	// activate the item
	item -> active = true;
}

// spawns a pile of items at enemy & returns item smart pointer
// entity: the enemy from which the items spawn
// multiple: how much of the item is spawned
int create_item_pile(struct enemy entity, int multiple)
{
	int i;
	
	// x-axis interval for items to	
	int left = entity.motion.hb.p.x - entity.motion.hb.r;
	int right = entity.motion.hb.p.x + entity.motion.hb.r;
	
	for (i = 0; i < multiple; i++) {
		// cycle through smart pointer
		cycle_item_smart_pointer(entity.item);
		// spawn item
		create_item(&itemlist[entity.item][nth_item[entity.item]],
				randint(left, right),
				entity.motion.hb.p.y);
	}
}

// main collision detection logic
// TODO: quadtree hitbox detection: github.com/arpit2297/Collision-Detection-using-Quad-Trees
void do_collision()
{
	int i, j, k, l;

	/* player hitting bullet or enemy */
	
	// if not invincible, do collision
	// questions/21650246/sdl-2-collision-detetection
	if (!player[curr_player].invincible) {
		// player -- bullets
		player_bullet_collision();
		// player -- enemies
		player_enemy_collision();
	// if player is invincible
	} else if (player[curr_player].iframes > 0) {	// decrease iframe
		player[curr_player].iframes--;
	// if iframe is emptied: reset iframe
	} else {
		player[curr_player].invincible = false;
		player[curr_player].iframes = MAX_IFRAMES;
	}

	/* player collects items */

	// player -- powerup
	for (i = 0; i < MAX_ITEMS; i++) {
		for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
			if (circle_in_circle(itemlist[i][j].motion.hb,
						player[curr_player].itembox)
					&& itemlist[i][j].active) {
				itemlist[i][j].active = false;
				switch (i) {
				case ITEM_POWERUP:
					player[curr_player].power += itemlist[i][j].value;
					printf("%lf\n", player[curr_player].power);
					break;
				case ITEM_SCORE:
					player[curr_player].score += itemlist[i][j].value;
					break;
				default:
					break;
				}
			}
		}
	}


	// TODO: optimize nested for loop
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

			// TODO: make it so only red fairies drop powerups
			// enemy dies, drops item
			if (enemylist[k][l].health == 0) {
				enemylist[k][l].active = false;
				// item drop
				create_item_pile(enemylist[k][l], 2);
			}
		}
	}
	}
	}
	}
}

