/*
 * logic.h is the headerfile for logic.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

// stackoverflow questions/1653958/ddg#1653965
// "That prevents double declaration of any identifiers" 
#ifndef LOGIC_H
#define LOGIC_H

// includes
#pragma once
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "player.h"

/* MACROS */
// hitboxes
#define SQUARE 0  // determines hitbox of entity
#define CIRCLE 1

#define MAX_PLAYERS 2

#define PLAYER_BOTTOM_MARGIN 16 // extra margins for player at bottom of field

#define MAX_ENEMIES 1

#define MAX_PLAYER_BULLETS MAX_BULLET_MULTIPLE
#define MAX_RELOAD 3
#define PLAYER_BULLET_SPEED 16.

#define MAX_ENEMY_MULTIPLE 1024

// for bulletlist
#define MAX_BULLETS 1
#define MAX_BULLET_MULTIPLE 2048
#define BULLET_BALL_8x8 0

#define MAX_POWERUPS MAX_ITEM_MULTIPLE

// name for enemyID
#define ENEMY_FAIRY 0

#define MAX_ITEM_MULTIPLE 1024

/* STRUCTS */ 

/* kinematics */

struct position {
	double x;
	double y;
};

struct velocity {
	double x;
	double y;
};

struct acceleration {
	double x;
	double y;
};

/* hitbox shapes */


// circle-shaped hitbox
struct circlebox {
	double r; // radius of hitbox
	struct position p; // hitbox core position
};

// unused: all hitboxes are circles, for now
// values for rectangular hitbox
struct rectbox {
	double w;
	double h;
	double x;
	double y;
};

// unused: ellipse hitbox
struct _ellipsebox {
	double eccentricity;
	double major;
	double minor;
	double angle; // in radians
};

struct kinematics {
	struct position o; // origin point p_0(x_0, y_0)
	struct circlebox hb;
	struct velocity v; // current velocity (vector)
	struct acceleration a; // current acceleration
	int time_0; // exact time of spawn in frames
};

/* colors */

// colors in RGB
struct color {
	int r;
	int g;
	int b;
	int alpha;
};

/* SDL structs */

// an entity's types necessary to perform render
struct sdl_types {
	SDL_Surface *surface; // image loads into a surface
	SDL_Texture *texture;
	SDL_Rect rect; // full sprite
};

/* entities */

// playable character struct: 
struct player {
	char *name;
	// motion
	struct kinematics motion;
	struct circlebox grazebox; // =size of sprite, for grazing
	struct circlebox itembox;  // aura where items gravitate to in gen 2&3
	double speed;
	double focus;

	// render and SDL-movement
	struct sdl_types sdl;

	// stats
	int health;
	double power;
	int bombs;
	int shot;
	bool active;
	bool invincible; // i-frames
	int iframes;
};

// generic stage enemy
struct enemy {
	int id;

	// motion
	struct kinematics motion;

	// render & SDL-movement
	struct sdl_types sdl;

	// stats
	int health;
	bool active;
	
	// misc
	bool clearable; // 1: automatically unloaded if reaches off-screen
};

struct bullet {
	int id;
	struct kinematics motion;
	struct sdl_types sdl;

	int source; // 0: player bullet; 1: enemy bullet

	double power;
	bool active;

	// function pointer
	// questions/840501
	int (*trail)(double, double);
};

// collectible items such as powerups, bombs, score
struct item {
	struct kinematics motion;
	struct sdl_types sdl;

	bool active;
	double value;
};

struct overlay {
	struct sdl_types sdl;
};

// globals
extern struct player player[MAX_PLAYERS];
extern int curr_player;
extern struct bullet player_bullets[MAX_PLAYER_BULLETS];
extern int reload;
extern int nth_player_bullet;

extern struct bullet bulletlist[MAX_BULLETS][MAX_BULLET_MULTIPLE];
extern struct bullet ball_8x8;
extern int nth_ball_8x8;

extern struct enemy enemylist[MAX_ENEMIES][MAX_ENEMY_MULTIPLE];
extern struct enemy fairies;

extern struct overlay main_menu;
extern struct overlay border; // UI
extern struct overlay loading;
extern SDL_Texture *tex; // bullets
extern SDL_Texture *player_bullet_texture;

extern struct item powerup[MAX_POWERUPS];
extern int nth_powerup;

// functions
void initialize_logic();
void reset_player_position();
void update_player_motion();
void update_enemy_motion();
void do_logic();

#endif /* LOGIC_H */
