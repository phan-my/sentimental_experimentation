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
#include "stage.h"


/* VARIABLES */

// player
struct player player[MAX_PLAYERS];
int curr_player = 1;

// shot
struct bullet shotlist[MAX_SHOTS][MAX_SHOT_MULTIPLE];
int nth_shot[MAX_SHOTS];
struct bullet shot_reimu;
int nth_shot_reimu = 0;
struct bullet shot_reimu_a;
int nth_shot_reimu_a = 0;
struct bullet shot_marisa;
int nth_shot_marisa = 0;

// bullets
struct bullet bulletlist[MAX_BULLETS][MAX_BULLET_MULTIPLE];
int nth_bullet[MAX_BULLETS];
struct bullet ball_8x8;
struct bullet ball_16x16;
struct bullet bubble;

// enemies
struct enemy enemylist[MAX_ENEMIES][MAX_ENEMY_MULTIPLE];
int nth_enemy[MAX_ENEMIES];
struct enemy fairy;
struct enemy fairy_blue;

// items
struct item itemlist[MAX_ITEMS][MAX_ITEM_MULTIPLE];
int nth_item[MAX_ITEMS];
struct item powerup;
struct item score;

// overlay
struct overlay main_menu;
struct overlay border; // UI
struct overlay loading;


/* FUNCTIONS */

// modifies values of struct position
// *p: positional structure by reference
// xcoord: x-position of position
// ycoord: y-position of position
void set_position(struct position *p, double xcoord, double ycoord)
{
	p -> x = xcoord;
	p -> y = ycoord;
}

// modifies values of struct velocity by reference
// *p: velocity structure by reference
// xvec: x-vector of velocity
// yvec: y-vector of velocity
void set_velocity(struct velocity *v, double xvec, double yvec)
{
	v -> x = xvec;
	v -> y = yvec;
}

// modifies values of struct velocity by reference
// *p: velocity structure by reference
// xvec: x-vector of velocity
// yvec: y-vector of velocity
void set_acceleration(struct acceleration *a, double xvec, double yvec)
{
	a -> x = xvec;
	a -> y = yvec;
}

// returns measure angle in radians of a velocity vector
// v: given velocity vector 
double get_angle_from_velocity(struct velocity v)
{
	return atan2(v.y, v.x);
}

// sets player at the main start position
void reset_player_position()
{
	player[curr_player].motion.hb.p.x = PLAYER_ORIGIN_X;
	player[curr_player].motion.hb.p.y = PLAYER_ORIGIN_Y;
//	update_player_render(&player[curr_player]); // subpixel hitbox -> integer SDL_Rect
}

// returns timer since stage phase started
int get_phase_progress()
{
	int size = stages[current_stage][0]; // first item in array is size
	int i = 1;

	// loops through array until ith item is geq stage timestamp
	while (stage_progress_frames >= stages[current_stage - 1][i] ) {
		i++;
	}
	
	// return error
	if (i == size)
		return -1;
	return stage_progress_frames - stages[current_stage - 1][i - 1];
}

// returns timer since stage phase started; starts at 0
int get_phase()
{
	int size = stages[current_stage][0]; // first item in array is size
	int i = 1;

	// loops through array until ith item is geq stage timestamp
	while (stage_progress_frames > stages[current_stage - 1][i] ) {
		// return error
		if (i == size)
			return -1;
		i++;
	}

	return i - 1;
}


// initialize bulletlist with bulletIDs
void initialize_bullets()
{
	int i, j;
	// 8x8 pellet
	ball_8x8.id = BULLET_BALL_8x8;
	ball_8x8.active = false;
	ball_8x8.motion.hb.r = 3.;
	bulletlist[ball_8x8.id][0] = ball_8x8;
	
	// 16x16 ball
	ball_16x16.id = BULLET_BALL_16x16;
	ball_16x16.active = false;
	ball_16x16.motion.hb.r = 4.27;
	bulletlist[ball_16x16.id][0] = ball_16x16;
	
	// bubble
	bubble.id = BULLET_BUBBLE;
	bubble.active = false;
	bubble.motion.hb.r = 16.;
	bulletlist[bubble.id][0] = bubble;
	
	// create multiple of all unique bullets
	for (i = 0; i < MAX_BULLETS; i++) {
		// create multiples per bulletID
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++)
			bulletlist[i][j] = bulletlist[i][0];
		
		// set smart pointers to 0
		nth_bullet[i] = 0;
	}
}

// initialzie enemylist with enemyIDs
void initialize_enemies()
{
	int i, j;
	
	// red fairy 
	fairy.id = ENEMY_FAIRY;
	fairy.active = false;
	fairy.motion.hb.r = 10.;
	fairy.health = 4;
	fairy.item = ITEM_POWERUP;
	enemylist[fairy.id][0] = fairy;
	
	// blue fairy
	fairy_blue.id = ENEMY_FAIRY_BLUE;
	fairy_blue.active = false;
	fairy_blue.motion.hb.r = 10.;
	fairy_blue.health = 4;
	fairy_blue.item = ITEM_SCORE;
	enemylist[fairy_blue.id][0] = fairy_blue;
	
	// create multiple of all unique enemies
	for (i = 0; i < MAX_ENEMIES; i++) {
		for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
			enemylist[i][j] = enemylist[i][0];
		}
	}
	
}

// full initialization of objects and their property
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

	shot_marisa.id = SHOT_MARISA;
	shot_marisa.active = false;
	shot_marisa.motion.hb.r = 8.;
	shot_marisa.power = 1;
	shotlist[shot_marisa.id][0] = shot_marisa;

	for (i = 0; i < MAX_SHOTS; i++) {
		for (j = 0; j < MAX_SHOT_MULTIPLE; j++)
			shotlist[i][j] = shotlist[i][0];
		nth_shot[i] = 0; // sets smart pointer
	}

	
	initialize_enemies();
	initialize_bullets();
	
	/* initialize items */
	
	// small powerup item
	powerup.id = ITEM_POWERUP;
	powerup.active = false;
	powerup.motion.hb.r = powerup.sdl.rect.w;
	powerup.motion.v_max = 2.5;
	powerup.value = 0.05;
	itemlist[powerup.id][0] = powerup;
	
	// small score item
	score.id = ITEM_SCORE;
	score.active = false;
	score.motion.hb.r = score.sdl.rect.w;
	score.value = 1;
	score.motion.v_max = 2.5;
	itemlist[score.id][0] = score;
	
	for (i = 0; i < MAX_ITEMS; i++) {
		for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
			itemlist[i][j] = itemlist[i][0];
		}
		nth_item[i] = 0; // sets smart pointer
	}
}

// translate velocity into position in struct kinematics
void update_position_by_velocity(struct kinematics *motion)
{
	// increment pointer to p.x by v.x
	motion -> hb.p.x += motion -> v.x;
	motion -> hb.p.y += motion -> v.y;
}

// translate velocity into position in struct kinematics
void update_velocity_by_acceleration(struct kinematics *motion)
{
	// increment pointer to p.x by v.x
	motion -> v.x += motion -> a.x;
	motion -> v.y += motion -> a.y;
}

// translate angular acceleration into angular rate and velocity angle
void update_angle(struct kinematics *motion)
{
	// acceleration to rate
	motion -> angle_rate += motion -> angle_acceleration;
}

// translates angular values to new velocity vectors
// *motion: kinematic properties, by reference
void update_velocity_by_angle(struct kinematics *motion)
{
	double magnitude = hypot(motion -> v.x, motion -> v.y);
	motion -> angle_rate += motion -> angle_acceleration;
	double new_angle = get_angle_from_velocity(motion -> v) + motion -> angle_rate;

	motion -> v.x = magnitude * cos(new_angle);
	motion -> v.y = magnitude * sin(new_angle);
}

// updates all motion values with given acceleration
// motion: kinematic properties, passed by reference
void update_motion(struct kinematics *motion)
{
	// updates velocity by acceleration until cap velocity
	if (hypot(motion -> v.x, motion -> v.y) <= motion -> v_max)
		update_velocity_by_acceleration(motion);
	// if at cap velocity, set acceleration to 0
	else
		set_acceleration(&(motion -> a), 0, 0);

	// velocity -> position
	update_position_by_velocity(motion);
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
			// angles
			update_angle(&bulletlist[i][j].motion);
			update_velocity_by_angle(&bulletlist[i][j].motion);
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
	
	// update player bullet reload
	reload--;
	if (reload < 0)
		reload = MAX_RELOAD;
	
	// update player bullets
	for (i = 0; i < MAX_SHOTS; i++) {
		for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
			switch (i) {
			// basic player shot
			case SHOT_REIMU:
			case SHOT_MARISA:
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

// operations with items
void do_items()
{
	int i, j;
	// item falls
	for (i = 0; i < MAX_ITEMS; i++) {
	for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
		if (itemlist[i][j].active) {
			// updates all values in struct
			update_motion(&itemlist[i][j].motion);
		}
	}
	}
}

// all operations
void do_logic()
{
	int i;

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
