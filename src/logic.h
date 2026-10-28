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
#include <SDL2/SDL_ttf.h>
#include "player.h"
#include "shot.h"
#include "danmaku.h"
#include "enemy.h"
#include "item.h"
#include "menu.h"

/* MACROS */
// UNUSED: hitboxes
#define SQUARE 0  // determines hitbox of entity
#define CIRCLE 1

// player
#define MAX_PLAYERS 2
#define PLAYER_BOTTOM_MARGIN 16 // extra margins for player at bottom of field

// player's shot
#define MAX_SHOTS 2
#define MAX_SHOT_MULTIPLE 2048
// #define SHOT_REIMU 0
// #define SHOT_REIMU_A 1
#define PLAYER_BULLET_SPEED 16.

// for bulletlist
#define MAX_BULLETS 3
#define MAX_BULLET_MULTIPLE 2048

// name for enemyID
#define MAX_ENEMIES 2
#define MAX_ENEMY_MULTIPLE 1024

// item
#define MAX_ITEMS 2
#define MAX_ITEM_MULTIPLE 1024
#define ITEM_POWERUP 0
#define ITEM_SCORE 1

// widget
#define MAX_NAME_LENGTH 256

// curves
#define MAX_CURVES 100 // max curves per composite path

/* STRUCTS */ 

/* kinematics */

struct position {
	double x;
	double y;
};

struct velocity {
	double x;
	double y;
	double max;
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

// all relevant values for a moving entity
struct kinematics {
	struct position o; // origin point p_0(x_0, y_0)
	struct circlebox hb; // hitbox
	struct velocity v0; // initial velocity
	struct velocity v; // current velocity (vector)
	double v_max; // maximum velocity
	struct acceleration a0; // initial acceleration
	struct acceleration a; // current acceleration
	int time_0; // exact time of spawn in frames
	bool curved; // false: do funcs for straight motion; true: curvy motion
	double angle_rate; // first derivative with respect to time
	double angle_acceleration; // 2nd derivative with respect to time
	struct kinematics *next;
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
	int score;
	int graze;
	int bombs;

	int deathbomb_timer; // countdown timer for deathbombing
	bool deathbomb_active; // true: start counting down deathbomb_timer
	
	int shot; 
	bool active; // only 1 player can be active
	bool invincible; // i-frames
	int iframes; // countdown to 0
};

// generic stage enemy
struct enemy {
	int id;

	// motion
	struct kinematics motion;
	bool is_curve; // false: update motion as usual; true: do lerp

	// render & SDL-movement
	struct sdl_types sdl;

	// stats
	int health;
	bool active;
	
	// misc
	int item; // item that drops upon defeat
	int item_multiple; // number of items dropped upon defeat
	bool clearable; // 1: automatically unloaded if reaches off-screen
	
	bool grazed;
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

	bool grazed;

	int max_homing; // in fr, maximum time that bullet is homing
	bool is_homing; // true: use module for homing bullets; false: otherwise
};

// collectible items such as powerups, bombs, score
struct item {
	int id;
	struct kinematics motion;
	struct sdl_types sdl;

	bool active;
	double value;
};

// for overlay images
struct overlay {
	struct sdl_types sdl;
};

// for ttf functionality
struct text {
	// render
	struct sdl_types sdl;
	
	double x;
	double y;
	
	// formal requirements
	TTF_Font *font;
	SDL_Color color;
	const char *string;
};

// https://www.parallelrealities.co.uk/tutorials/widgets/widgets1.php
struct widget {
	char name[MAX_NAME_LENGTH]; // unique name for widget
	
	struct text text;
	
	// coordinates for drawing
	int x;
	int y;
	
	// text that is displayed
	char label[MAX_NAME_LENGTH];
	
	// double-linked list pointers
	// eases back-and-forth movement within list
	struct widget *prev;
	struct widget *next;
	void (*action)(void);
	
};

// a singular bezier curve
struct bezier {
	struct position p0;
	struct position c0;
	struct position p1;
	struct position c1;
};


// globals

// player
extern struct player player[MAX_PLAYERS];

// shot
extern struct bullet shotlist[MAX_SHOTS][MAX_SHOT_MULTIPLE];
extern int nth_shot[MAX_SHOTS];

// bullet
extern struct bullet bulletlist[MAX_BULLETS][MAX_BULLET_MULTIPLE];
extern int nth_bullet[MAX_BULLETS];

// enemy
extern struct enemy enemylist[MAX_ENEMIES][MAX_ENEMY_MULTIPLE];
extern int nth_enemy[MAX_ENEMIES];

// item
extern struct item itemlist[MAX_ITEMS][MAX_ITEM_MULTIPLE];
extern int nth_item[MAX_ITEMS];

// overlay
extern SDL_Texture *tex; // bullets
extern SDL_Texture *player_bullet_texture;

// functions
void set_position(struct position *p, double xcoord, double ycoord);
void set_velocity(struct velocity *v, double xvec, double yvec);
void set_acceleration(struct acceleration *a, double xvec, double yvec);
void update_position_by_velocity(struct kinematics *motion);
void update_motion(struct kinematics *motion);
void reset_player_position();
int get_phase_progress();
int get_phase();
void initialize_logic();
void update_player_motion();
void update_enemy_motion();
bool in_unload_zone(struct kinematics entity);
void do_logic();

#endif /* LOGIC_H */
