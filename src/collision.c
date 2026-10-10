/*
 * collision.c contains hitbox interaction operations.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */
#include <stdbool.h>
#include "collision.h"
#include "logic.h"
#include "common.h"


// circle-circle collision
// TODO: Pythagorean expression optimizable?
bool circle_in_circle(struct circlebox dest, struct circlebox src)
{
	// https://silentmatt.com/rectangle-intersection/
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
	double dx = src.p.x + src.r - (dest.p.x + dest.r);
	double dy = src.p.y + src.r - (dest.p.y + dest.r);
	double distance = sqrt(dx * dx + dy * dy);
	return distance < src.r + dest.r;
}

// checks if position is in rectbox
bool _in_rect(double x, double y, struct rectbox obj)
{
	if (obj.x <= x && x <= obj.x + obj.w
			&& obj.y <= y && y <= obj.y + obj.h)
		return true;
	else
		return false;
}

// rectangle-rectangle collision
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

// player hits enemies
void player_enemy_collision()
{
	int i, j;

	// loop through entire enemylist
	for (i = 0; i < MAX_ENEMIES; i++) {
		for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
			// 
			if (circle_in_circle(enemylist[i][j].motion.hb,
						player[curr_player].motion.hb) &&
					enemylist[i][j].active) {
//				printf("player-fairy HIT  \n");
				enemylist[i][j].active = false; // kill enemy
				player[curr_player].invincible = true; // iframe
				player[curr_player].health--; // decrease HP
				reset_player_position(); // put player at origin
			}
		}
	}
}

// main collision detection logic
// TODO: quadtree hitbox detection: github.com/arpit2297/Collision-Detection-using-Quad-Trees
void do_collision()
{
	int i, j, k;

	/* player hitting things */
	
	// if not invincible, do collision
	// questions/21650246/sdl-2-collision-detetection
	if (!player[curr_player].invincible) {
		// player -- enemy bullet
		for (i = 0; i < MAX_BULLETS; i++) {
			for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
				if (circle_in_circle(bulletlist[i][j].motion.hb,
							player[curr_player].motion.hb)
						&& bulletlist[i][j].active) {
//					printf("%d: HIT\n", i);
					bulletlist[i][j].active = false;
					player[curr_player].invincible = true;
					reset_player_position();
				}
			}
		}

		// player -- enemies
		player_enemy_collision();
	} else if (player[curr_player].iframes > 0) {	// decrease iframe
		player[curr_player].iframes--;
	} else { // if iframe is emptied: reset iframe
		player[curr_player].invincible = false;
		player[curr_player].iframes = MAX_IFRAMES;
	}

	// player -- powerup
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (circle_in_circle(powerup[i].motion.hb,
					player[curr_player].itembox)
				&& powerup[i].active) {
			powerup[i].active = false;
			player[curr_player].power += powerup[i].value;
		}
	}


	// player bullet hitting things
	for (i = 0; i < MAX_PLAYER_BULLETS; i++) {
		for (j = 0; j < MAX_ENEMIES; j++) {
			for (k = 0; k < MAX_ENEMY_MULTIPLE; k++)
{
			// player bullet hits fairy
			// fairy takes damage
			if (player_bullets[i].active && enemylist[j][k].active
					&&
					circle_in_circle(enemylist[j][k].motion.hb,
						player_bullets[i].motion.hb)) {
//				printf("PLAYER BULLET HTIS FAIRY\n");

				// damage dealt based on player bullet power
				enemylist[j][k].health -= player_bullets[i].power;

				// unload bullet
				player_bullets[i].active = false;

				// fairy dies
				if (enemylist[j][k].health == 0) {
					nth_powerup = cycle_item_smart_pointer(nth_powerup, &powerup, MAX_POWERUPS);
					enemylist[j][k].active = 0;
					// item drop
					powerup[nth_powerup].active = true;
//					powerup[nth_powerup].motion.o.x = enemylist[j][k].motion.hb.p.x;
//					powerup[nth_powerup].motion.o.y = enemylist[j][k].motion.hb.p.y;
					powerup[nth_powerup].motion.hb.p.x = enemylist[j][k].motion.hb.p.x;
					powerup[nth_powerup].motion.hb.p.y = enemylist[j][k].motion.hb.p.y;
					nth_powerup = cycle_item_smart_pointer(nth_powerup, &powerup, MAX_POWERUPS);
				}
			}
}
		}
	}
}
