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


/* VARIABLES */

struct player player[MAX_PLAYERS];
int curr_player = 0;

struct bullet player_bullets[MAX_PLAYER_BULLETS];

struct bullet ball_8x8[MAX_BULLETS];
struct enemy fairies[MAX_FAIRIES];

// items
struct item powerup[MAX_POWERUPS];
int nth_powerup;

// overlay
struct overlay main_menu;
struct overlay border; // UI
struct overlay loading;

SDL_Texture *tex; // bullets
SDL_Texture *player_bullet_texture;


/* FUNCTIONS */

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

// sets player at the main start position
void reset_player_position()
{
	player[curr_player].motion.p.x = FIELD_WIDTH / 2. + FIELD_OFFSET_X;
	player[curr_player].motion.p.y = FIELD_HEIGHT * 0.9 + FIELD_OFFSET_Y;
//	update_player_render(&player[curr_player]); // subpixel hitbox -> integer SDL_Rect

}

// full initialization of structs
void initialize_logic()
{
	int i;	
	
	// numerical array pointer for power items
	nth_powerup = 0;
	
	// set initial values common to all players
	for (i = 0; i < MAX_PLAYERS; i++) {
		player[i].invincible = false;
		player[i].iframes = MAX_IFRAMES;
		player[i].power = 0.;
		player[i].core.r = 2.;
		// TODO: redraw to deal with grazing
		player[i].grazebox.r = 4.;
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
	}
}

// main collision detection logic
void do_collision()
{
	int i, j;

	// TODO: quadtree hitbox detection: github.com/arpit2297/Collision-Detection-using-Quad-Trees
	
	// questions/21650246/sdl-2-collision-detetection
	// player hitting things
	if (!player[curr_player].invincible) {
		// player -- enemy bullet
		for (i = 0; i < MAX_BULLETS; i++) {
			if (circle_in_circle(ball_8x8[i].hitbox,
						player[curr_player].core)
					&& ball_8x8[i].active) {
//				printf("%d: HIT\n", i);
				player[curr_player].invincible = true;
				reset_player_position();
			}
		}
	
		// player -- fairy
		for (i = 0; i < MAX_FAIRIES; i++) {
			// fairy hits player
			if (circle_in_circle(fairies[i].hitbox,
						player[curr_player].core) &&
					fairies[i].active) {
//				printf("player-fairy HIT  \n");
				player[curr_player].invincible = true;
				reset_player_position();
			}
		}
	} else if (player[curr_player].iframes > 0) {	// decrease iframe
		player[curr_player].iframes--;
	} else { // if iframe is emptied: reset iframe
		player[curr_player].invincible = false;
		player[curr_player].iframes = MAX_IFRAMES;
	}

	// player -- powerup
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (circle_in_circle(powerup[i].hitbox,
					player[curr_player].itembox)
				&& powerup[i].active) {
			powerup[i].active = false;
			player[curr_player].power += powerup[i].value;
		}
	}
		

	// player bullet hitting things
	for (i = 0; i < MAX_FAIRIES; i++) {

		// player bullet hits fairy
		for (j = 0; j < MAX_PLAYER_BULLETS; j++) {
			// fairy takes damage
			if (player_bullets[j].active && fairies[i].active &&
					circle_in_circle(fairies[i].hitbox,
						player_bullets[j].hitbox)) {
//				printf("PLAYER BULLET HTIS FAIRY\n");

				// damage dealt based on player bullet power
				fairies[i].health -= player_bullets[j].power;

				// unload bullet
				player_bullets[j].hitbox.p.y = 0;

				// fairy dies
				if (fairies[i].health == 0) {
					fairies[i].active = 0;
					player_bullets[j].active = 0;
					// item drop
					powerup[nth_powerup].active = true;
					powerup[nth_powerup].hitbox.p.x = fairies[i].hitbox.p.x;
					powerup[nth_powerup].hitbox.p.y = fairies[i].hitbox.p.y;
					powerup[i].sdl.rect.x = (int)(powerup[i].hitbox.p.x - powerup[i].sdl.rect.w / 2.);
					powerup[i].sdl.rect.y = (int)(powerup[i].hitbox.p.y - powerup[i].sdl.rect.h / 2.);
				}

			}
		}
	}
}

void update_player_position()
{
	player[curr_player].motion.p.x += player[curr_player].motion.v.x;
	player[curr_player].motion.p.y += player[curr_player].motion.v.y;
}

void update_player_motion()
{
	update_player_position();
	// core hitbox
	player[curr_player].core.p.x = player[curr_player].motion.p.x;
	player[curr_player].core.p.y = player[curr_player].motion.p.y;

	// graze hitbox
	player[curr_player].grazebox.p.x = player[curr_player].motion.p.x;
	player[curr_player].grazebox.p.y = player[curr_player].motion.p.y;
	
	// item collection htibox
	player[curr_player].itembox.p.x = player[curr_player].motion.p.x;
	player[curr_player].itembox.p.y = player[curr_player].motion.p.y;

}

// all operations
void do_logic()
{
	update_player_motion();
	do_collision();
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
