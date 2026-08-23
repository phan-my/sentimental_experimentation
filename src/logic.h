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
#include <SDL2/SDL.h>
#include <math.h>


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
};

struct ball {
	struct circlebox hitbox;
	struct sdl_types sdl;
	int power;
};

// variables
extern struct player reimu;

// functions
bool is_hit(struct circlebox dest, struct circlebox src);
void update_ball_position(struct ball *p);
void update_player_position(struct player *p);
void update_enemy_position(struct enemy *p);

#endif /* LOGIC_H */
