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
#include "stage.h"
#include "menu.h"
#include "sounds.h"


/* VARIABLES */

// player
struct player player[MAX_PLAYERS];

// shot
struct bullet shotlist[MAX_SHOTS][MAX_SHOT_MULTIPLE];
int nth_shot[MAX_SHOTS]; // smart pointer

// bullets
struct bullet bulletlist[MAX_BULLETS][MAX_BULLET_MULTIPLE];
int nth_bullet[MAX_BULLETS]; // smart pointer

// enemies
struct enemy enemylist[MAX_ENEMIES][MAX_ENEMY_MULTIPLE];
int nth_enemy[MAX_ENEMIES]; // smart pointer

// items
struct item itemlist[MAX_ITEMS][MAX_ITEM_MULTIPLE];
int nth_item[MAX_ITEMS];


/* FUNCTIONS */

/* static */

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

// FIXME: does not work as intended
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

// FIXME: does not work as intended
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

// sets default value of struct kinematics (except player)
void reset_position()
{
	;
}

// full initialization of objects and their property
void initialize_logic()
{
	initialize_widgets();
	initialize_players();
	initialize_shot();
	initialize_enemies();
	initialize_bullets();
	initialize_items();
}

/* dynamic */

// translate velocity into position in struct kinematics
// *motion: struct kinematics to parse by reference
void update_position_by_velocity(struct kinematics *motion)
{
	// increment pointer to p.x by v.x
	motion -> hb.p.x += motion -> v.x;
	motion -> hb.p.y += motion -> v.y;
}

// translate acceleration into velocity in struct kinematics
// *motion: struct kinematics to parse by reference
void update_velocity_by_acceleration(struct kinematics *motion)
{
	// increment pointer to p.x by v.x
	motion -> v.x += motion -> a.x;
	motion -> v.y += motion -> a.y;
}

// translates angular values to new velocity and acceleration vector directions
// *motion: struct kinematics to parse by reference
void update_velocity_by_angle(struct kinematics *motion)
{
	// magnitude of velocity vector
	double speed = hypot(motion -> v.x, motion -> v.y);
	double acceleration = hypot(motion -> a.x, motion -> a.y);
	
	// add angular acceleration to new angular rate
	motion -> angle_rate += motion -> angle_acceleration;
	
	// add angular rate to new angle
	double new_angle
		= get_angle_from_velocity(motion -> v) + motion -> angle_rate;

	// sets vectors according to angles
	motion -> v.x = speed * cos(new_angle);
	motion -> v.y = speed * sin(new_angle);
	motion -> a.x = acceleration * cos(new_angle);
	motion -> a.y = acceleration * sin(new_angle);
}

// updates all motion values with given acceleration
// motion: kinematic properties, passed by reference
void update_motion(struct kinematics *motion)
{
	
	// get new angles
	// optimization: update only if angular rate and acceleration exist
	if (motion -> angle_rate != 0. && motion -> angle_acceleration != 0.)
		update_velocity_by_angle(motion);

	// if updated velocity <= v_max, update velocity
	// optimization: using sum of squares instead of hypothenuse
	if (pow(motion -> v.x + motion -> a.x, 2)
			+ pow(motion -> v.y + motion -> a.y, 2) 
			<= pow(motion -> v_max, 2))
		update_velocity_by_acceleration(motion);
	
	// velocity -> position
	update_position_by_velocity(motion);
}

// checks if entity is off-screen
// entity: struct kinematics
bool in_unload_zone(struct kinematics entity)
{
	if ((0 <= entity.hb.p.x && entity.hb.p.x <= FIELD_RIGHT) ||
			(0 <= entity.hb.p.y &&
			 entity.hb.p.y <= + FIELD_BOTTOM + FIELD_OFFSET_Y))
		return false;
	else
		return true;
}

// all operations
void do_logic()
{
	switch (state_menu) {
	case STATE_PLAY:
		do_stage();
		do_collision();
		do_player();
		do_bullet();
		do_enemy();
		do_shot();
		do_item();
		break;
	case STATE_LOADING:
		do_loading();
		break;
	case STATE_ON_POWER:
		state_menu = STATE_MAIN_MENU;
		break;
	case STATE_MAIN_MENU:
		break;
	default:
		break;
	}

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


// FIXME: create a bullet model for QueryTexture to invoke
/*
void load_bullet_model()
{

}
*/
