/*
 * logic.c contains the logical core of the game.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */

#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "logic.h"
#include "render.h"
#include "random.h"
#include "input.h"
#include "collision.h"


/* VARIABLES */

struct player player[MAX_PLAYERS];
int curr_player = 0;
struct bullet player_bullets[MAX_PLAYER_BULLETS];
int reload = MAX_RELOAD;
int nth_player_bullet = 0;

// bullets
struct bullet bulletlist[MAX_BULLETS][MAX_BULLET_MULTIPLE];
struct bullet ball_8x8;
int nth_ball_8x8;

// generic stage enemies
struct enemy enemylist[MAX_ENEMIES][MAX_ENEMY_MULTIPLE];
struct enemy fairies;

// items
struct item powerup[MAX_POWERUPS];
int nth_powerup;

// overlay
struct overlay main_menu;
struct overlay border; // UI
struct overlay loading;


/* FUNCTIONS */

// sets player at the main start position
void reset_player_position()
{
	player[curr_player].motion.p.x = PLAYER_ORIGIN_X;
	player[curr_player].motion.p.y = PLAYER_ORIGIN_Y;
//	update_player_render(&player[curr_player]); // subpixel hitbox -> integer SDL_Rect

}

// full initialization of structs
void initialize_logic()
{
	int i, j;	
	
	// numerical array pointer for power items
	nth_powerup = 0;
	
	// set initial values common to all players
	for (i = 0; i < MAX_PLAYERS; i++) {
		player[i].invincible = false;
		player[i].iframes = MAX_IFRAMES;
		player[i].power = 0.;
		player[i].core.r = 2.;
		// TODO: redraw to deal with grazing
		player[i].grazebox.r = 8.;
		player[i].itembox.r = player[i].grazebox.r * 2.;
	}

	// set initial values for each individual player
	player[0].speed = REIMU_DEFAULT_SPEED;
	player[0].focus = REIMU_FOCUS_SPEED;
	player[0].health = 2;
	player[0].bombs = 2;
	
	reset_player_position();
	update_player_motion();
	
	// player bullet initialization
	for (i = 0; i < MAX_PLAYER_BULLETS; i++) {
		player_bullets[i].active = false;
/*
		player_bullets[i].hitbox.p.x = player[curr_player].core.p.x;
		player_bullets[i].hitbox.p.y = reimu.core.p.y;
*/
		player_bullets[i].hitbox.r = 8.;
		player_bullets[i].power = 1;
		player_bullets[i].motion.p.x = player[curr_player].motion.p.x;
		player_bullets[i].motion.p.y = player[curr_player].motion.p.y;
	}
	
	// initialzie enemylist with enemyIDs
	fairies.id = ENEMY_FAIRY;
	fairies.active = false;
	fairies.hitbox.r = 8.;
	fairies.health = 4;
	enemylist[fairies.id][0] = fairies;
	
	// create multiple of all unique enemies
	for (i = 0; i < MAX_ENEMIES; i++) {
		for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
			enemylist[i][j] = enemylist[i][0];
		}
	}
	
	// initialize bulletlist with bulletIDs
	ball_8x8.id = BULLET_BALL_8x8;
	ball_8x8.active = false;
	ball_8x8.hitbox.r = 3.8;
	bulletlist[ball_8x8.id][0] = ball_8x8;
	
	// create multiple of all unique bullets
	for (i = 0; i < MAX_BULLETS; i++) {
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
			bulletlist[i][j] = bulletlist[i][0];
		}
	}
}

// translate velocity into position in struct kinematics
void update_position_by_velocity(struct kinematics *motion)
{
	// increment pointer to p.x by v.x
	motion -> p.x += motion -> v.x;
	motion -> p.y += motion -> v.y;
}

// update all positional values of the player
void update_player_motion()
{
	// v -> p
	update_position_by_velocity(&player[curr_player].motion);

	// p -> core hitbox
	player[curr_player].core.p.x = player[curr_player].motion.p.x;
	player[curr_player].core.p.y = player[curr_player].motion.p.y;

	// p -> graze hitbox
	player[curr_player].grazebox.p.x = player[curr_player].motion.p.x;
	player[curr_player].grazebox.p.y = player[curr_player].motion.p.y;
	
	// p -> item collection htibox
	player[curr_player].itembox.p.x = player[curr_player].motion.p.x;
	player[curr_player].itembox.p.y = player[curr_player].motion.p.y;
}

// update all positional values of all active enemies
void update_enemy_motion()
{
	int i, j;
	
	// loop through and update all enemies
	for (i = 0; i < MAX_ENEMIES; i++) {
		for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
			// v -> p
			update_position_by_velocity(&enemylist[i][j].motion);
			
			// p -> core hitbox
			enemylist[i][j].hitbox.p.x = enemylist[i][j].motion.p.x;
			enemylist[i][j].hitbox.p.y = enemylist[i][j].motion.p.y;
		}
	}
}

// update all positional values of all active enemies
void update_bullet_motion()
{
	int i, j;
	
	// update all enemy bullets
	for (i = 0; i < MAX_BULLETS; i++) {
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
			// v -> p
			update_position_by_velocity(&bulletlist[i][j].motion);
			
			// p -> core hitbox
			bulletlist[i][j].hitbox.p.x = bulletlist[i][j].motion.p.x;
			bulletlist[i][j].hitbox.p.y = bulletlist[i][j].motion.p.y;
		}
	}
	
	// update all player bullets
	for (i = 0; i < MAX_PLAYER_BULLETS; i++) {
		// v -> p
		update_position_by_velocity(&player_bullets[i].motion);
		
		// p -> core hitbox
		player_bullets[i].hitbox.p.x = player_bullets[i].motion.p.x;
		player_bullets[i].hitbox.p.y = player_bullets[i].motion.p.y;
	}
}

// all operations
void do_logic()
{
	int i;

	// update player bullet reload
	reload--;
	if (reload < 0)
		reload = MAX_RELOAD;
	
	for (i = 0; i < MAX_PLAYER_BULLETS; i++) {
		if (player_bullets[i].active) {
			player_bullets[i].motion.v.x = 0.;
			player_bullets[i].motion.v.y = -PLAYER_BULLET_SPEED;
		} else {
			player_bullets[i].motion.v.x = 0.;
			player_bullets[i].motion.v.y = 0.;
			
			// set player bullet at player's position
			player_bullets[i].motion.p.x = player[curr_player].motion.p.x;
			player_bullets[i].motion.p.y = player[curr_player].motion.p.y;
			
		}
	}
	
	// update all positional values of entities
	update_player_motion();
	update_bullet_motion();
	update_enemy_motion();
	do_collision(); // and check and do collision between them
}

/*
bool in_open(double num, double lower, double upper)
{
	if (lower < num && num < upper)
		return true;
	else
		return false;
}

// returns top-left of the hitbox as a dest
double topleft(double core, double radius)
{
	return core - radius;
}

double bottomright(double core, double radius)
{
	return core + radius;
}
*/

/*
bool circle_in_circle(struct player p, struct bullet b)
{

}
*/

// FIXME: create a bullet model for QueryTexture to invoke
/*
void load_bullet_model()
{

}
*/
