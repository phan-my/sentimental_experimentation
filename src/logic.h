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

// macros
#define REIMU_DEFAULT_SPEED 4.5  
#define REIMU_FOCUS_FACTOR 0.4444444
#define PLAYER_BOTTOM_MARGIN 16 // extra margins for player at bottom of field

#define MAX_BULLETS 1000
#define MAX_FAIRIES 255


#define MAX_RELOAD 2
#define MAX_PLAYER_BULLETS 100

#define FIELD_OFFSET_X 32
#define FIELD_OFFSET_Y 16

/* STRUCTS */ 

/* hitbox shapes */

struct circlebox {
	double r;
	double x; // hitbox core position
	double y; // ditto
};

struct squarebox {
	double l;
	double x;
	double y;
};

// experimental
struct _ellipsebox {
	double eccentricity;
	double major;
	double minor;
	double angle; // in radians
};

/* SDL structs */

struct sdl_types {
	SDL_Surface *surface; // image loads into a surface
	SDL_Texture *texture;
	SDL_Rect rect; // full sprite
};

/* entities */

// has a hitbox; players, enemies, by extension the boss
struct player {
	struct circlebox hitbox;
	struct sdl_types sdl;
	int health;
	double level;
	double speed;
	double diagonal;
	bool active;
};

struct position {
	double x;
	double y;
};

struct entity {
	struct position hitbox;
	struct sdl_types sdl;
};

struct enemy {
	struct circlebox hitbox;
	struct sdl_types sdl;
	int health;
	bool active;
};

struct ball {
	struct circlebox hitbox;
	struct sdl_types sdl;
	int power;
	bool active;
};

// globals
extern struct player reimu;
extern struct ball player_bullets[MAX_PLAYER_BULLETS];

extern SDL_Texture *border_tex; // UI
extern SDL_Texture *tex; // bullets
extern SDL_Texture *player_bullet_texture;
extern SDL_Texture *main_menu;

// functions
bool is_hit(struct circlebox dest, struct circlebox src);
void update_ball_position(struct ball *p);
void update_player_position(struct player *p);
void update_enemy_position(struct enemy *p);

#endif /* LOGIC_H */
