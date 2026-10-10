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
#include "common.h"
#include "player.h"
#include "shot.h"


/* VARIABLES */

// player
struct player player[MAX_PLAYERS];
int curr_player = 0;

// shot
struct bullet shotlist[MAX_SHOTS][MAX_SHOT_MULTIPLE];
struct bullet shot_reimu;
int nth_shot_reimu = 0;
struct bullet shot_reimu_a;
int nth_shot_reimu_a = 0;
int reload = MAX_RELOAD;

// bullets
struct bullet bulletlist[MAX_BULLETS][MAX_BULLET_MULTIPLE];
struct bullet ball_8x8;
int nth_ball_8x8 = 0;

// enemies
struct enemy enemylist[MAX_ENEMIES][MAX_ENEMY_MULTIPLE];
struct enemy fairies;

// items
struct item itemlist[MAX_ITEMS][MAX_ITEM_MULTIPLE];
struct item powerup;
int nth_powerup = 0;

// overlay
struct overlay main_menu;
struct overlay border; // UI
struct overlay loading;


/* FUNCTIONS */

// sets player at the main start position
void reset_player_position()
{
	player[curr_player].motion.hb.p.x = PLAYER_ORIGIN_X;
	player[curr_player].motion.hb.p.y = PLAYER_ORIGIN_Y;
//	update_player_render(&player[curr_player]); // subpixel hitbox -> integer SDL_Rect

}

// full initialization of structs
void initialize_logic()
{
	int i, j;	
	
	initialize_players();
	
	reset_player_position();
	update_player_motion();
	
	// player bullet initialization
	shot_reimu.id = SHOT_REIMU;
	shot_reimu.active = false;
	shot_reimu.motion.hb.r = 8.;
	shot_reimu.power = 1;
	shotlist[shot_reimu.id][0] = shot_reimu;
	for (i = 0; i < MAX_SHOTS; i++) {
		for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
			shotlist[i][j] = shotlist[i][0];
//			shotlist[i][j].motion.hb.p.x = player[curr_player].motion.hb.p.x;
//			shotlist[i][j].motion.hb.p.y = player[curr_player].motion.hb.p.y;
		}
	}

	
	// initialzie enemylist with enemyIDs
	fairies.id = ENEMY_FAIRY;
	fairies.active = false;
	fairies.motion.hb.r = 10.;
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
	ball_8x8.motion.hb.r = 3.8;
	bulletlist[ball_8x8.id][0] = ball_8x8;
	
	// create multiple of all unique bullets
	for (i = 0; i < MAX_BULLETS; i++) {
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
			bulletlist[i][j] = bulletlist[i][0];
		}
	}
	
	// initialize items
	powerup.id = ITEM_POWERUP;
	powerup.active = false;
	powerup.motion.hb.r = powerup.sdl.rect.w;
	powerup.value = 1.;
	itemlist[powerup.id][0] = powerup;
	
	for (i = 0; i < MAX_ITEMS; i++)
		for (j = 0; j < MAX_ITEM_MULTIPLE; j++) 
			itemlist[i][j] = itemlist[powerup.id][0];
}

// translate velocity into position in struct kinematics
void update_position_by_velocity(struct kinematics *motion)
{
	// increment pointer to p.x by v.x
	motion -> hb.p.x += motion -> v.x;
	motion -> hb.p.y += motion -> v.y;
}

// update all positional values of the player
void update_player_motion()
{
	// v -> p
	update_position_by_velocity(&player[curr_player].motion);
	
	// p -> graze hitbox
	player[curr_player].grazebox.p.x = player[curr_player].motion.hb.p.x;
	player[curr_player].grazebox.p.y = player[curr_player].motion.hb.p.y;
	
	// p -> item collection htibox
	player[curr_player].itembox.p.x = player[curr_player].motion.hb.p.x;
	player[curr_player].itembox.p.y = player[curr_player].motion.hb.p.y;
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

/*
			// p -> core hitbox
			enemylist[i][j].hitbox.p.x = enemylist[i][j].motion.hb.p.x;
			enemylist[i][j].hitbox.p.y = enemylist[i][j].motion.hb.p.y;
*/
		}
	}
}

bool in_unload_zone(struct kinematics entity)
{
	if ((0 <= entity.hb.p.x && entity.hb.p.x <= FIELD_RIGHT) ||
			(0 <= entity.hb.p.y &&
			 entity.hb.p.y <= + FIELD_BOTTOM + FIELD_OFFSET_Y))
		return false;
	else
		return true;
}

void unload_offscreen_bullets()
{
	int i, j;

	for (i = 0; i < MAX_BULLETS; i++) {
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
			if (in_unload_zone(bulletlist[i][j].motion))
				bulletlist[i][j].active = false;
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

/*
			// p -> core hitbox
			bulletlist[i][j].hitbox.p.x
				= bulletlist[i][j].motion.hb.p.x;
			bulletlist[i][j].hitbox.p.y
				= bulletlist[i][j].motion.hb.p.y;
*/
		}
	}

	unload_offscreen_bullets();
}

// complete template for player bullets
void do_player_bullets()
{
	int i, j;
	
	// update player bullets
	for (i = 0; i < MAX_SHOTS; i++) {
		for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
			switch (i) {
			case SHOT_REIMU:
				// continue setting specified speed to active bullet 
				if (shotlist[i][j].active) {
					shotlist[i][j].motion.v.y = -PLAYER_BULLET_SPEED;
					// v -> p
					update_position_by_velocity(&shotlist[i][j].motion);		
					// reset bullets if they fall out of bounds
					if (shotlist[i][j].motion.hb.p.y < 0)
						shotlist[i][j].active = 0;
				// case bullet is inactive
				} else {
					shotlist[i][j].motion.v.x = 0.;
					shotlist[i][j].motion.v.y = 0.;
				
					// set player bullet at player's position
					// creates two columns of player bullets 
					if (j % 2)
						shotlist[i][j].motion.hb.p.x
							= player[curr_player].motion.hb.p.x + 8;
					else
						shotlist[i][j].motion.hb.p.x
							= player[curr_player].motion.hb.p.x - 8;			
					shotlist[i][j].motion.hb.p.y
						= player[curr_player].motion.hb.p.y;
				}
				break;
			default:
				break;
			}
		}
	}
}

void do_items()
{
	int i, j;
	// item falls
	for (i = 0; i < MAX_ITEMS; i++) {
		for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
			if (itemlist[i][j].active) {
				// TODO: use motion.v
				itemlist[i][j].motion.hb.p.y += 1.;
				// TODO: move to render.c
				itemlist[i][j].sdl.rect.x = (int)(itemlist[i][j].motion.hb.p.x - itemlist[i][j].sdl.rect.w / 2.);
				itemlist[i][j].sdl.rect.y = (int)(itemlist[i][j].motion.hb.p.y - itemlist[i][j].sdl.rect.h / 2.);
			}
		}
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
	
	nth_powerup = cycle_item_smart_pointer(nth_powerup, &itemlist[ITEM_POWERUP]);
	
	// update all positional values of entities
	update_player_motion();
	update_bullet_motion();
	update_enemy_motion();
	do_player_bullets();
	do_items();
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
