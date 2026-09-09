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


/* VARIABLES */

struct player reimu;
struct ball player_bullets[MAX_PLAYER_BULLETS];

struct ball ball_8x8[MAX_BULLETS];
struct enemy fairies[MAX_FAIRIES];

struct overlay main_menu;
struct overlay border; // U
struct overlay loading;
SDL_Texture *tex; // bullets
SDL_Texture *player_bullet_texture;


/* FUNCTIONS */

// circle-circle collision
// TODO: Pythagorean expression optimizable?
bool is_hit(struct circlebox dest, struct circlebox src)
{
	// https://silentmatt.com/rectangle-intersection/
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
	double dx = src.x + src.r - (dest.x + dest.r);
	double dy = src.y + src.r - (dest.y + dest.r);
	double distance = sqrt(dx * dx + dy * dy);
	return distance < src.r + dest.r;
}

// set sdl int position to match the float position for a bullet
void update_ball_position(struct ball *p)
{
	p -> sdl.rect.x = (int)(p -> hitbox.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> hitbox.y - p -> sdl.rect.h / 2.);
}

// set sdl int position to match the float position for the player
void update_player_position(struct player *p)
{
	p -> sdl.rect.x = (int)(p -> hitbox.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> hitbox.y - p -> sdl.rect.h / 2.);
}

// set sdl int position to match the float position for an enemy
void update_enemy_position(struct enemy *p)
{
	p -> sdl.rect.x = (int)(p -> hitbox.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> hitbox.y - p -> sdl.rect.h / 2.);
}

// main collision detection logic
void do_collision()
{
	int i, j;

	// TODO: quadtree hitbox detection
	// questions/21650246/sdl-2-collision-detetection
	// github.com/arpit2297/Collision-Detection-using-Quad-Trees

	// player -- enemy bullet
	for (i = 0; i < MAX_BULLETS; i++) {
		if (is_hit(ball_8x8[i].hitbox, reimu.hitbox)) {
			printf("%d: HIT\n", i);
			if (i % 2)
				printf("T\n");
		}
	}

	// player -- fairy -- player bullet
	for (i = 0; i < MAX_FAIRIES; i++) {
		// fairy hits player
		if (is_hit(fairies[i].hitbox, reimu.hitbox) && fairies[i].active)
			printf("FAIRY HITT  \n");

		// player bullet hits fairy
		for (j = 0; j < MAX_PLAYER_BULLETS; j++) {
			// fairy takes damage
			if (player_bullets[j].active && fairies[i].active && is_hit(fairies[i].hitbox,
						player_bullets[j].hitbox)) {
				printf("PLAYER BULLET HTIS FAIRY\n");

				// damage dealt based on player bullet power
				fairies[i].health -= player_bullets[j].power;

				// unload bullet
				player_bullets[j].hitbox.y = 0;

				// fairy dies
				if (fairies[i].health == 0) {
					fairies[i].active = 0;
					player_bullets[j].active = 0;
				}
			}
		}
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

/*
bool is_hit(struct player p, struct bullet b)
{

}
*/

// FIXME: create a bullet model for QueryTexture to invoke
/*
void load_bullet_model()
{

}
*/
