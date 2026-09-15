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


/* VARIABLES */

struct player reimu;
struct ball player_bullets[MAX_PLAYER_BULLETS];

struct ball ball_8x8[MAX_BULLETS];
struct enemy fairies[MAX_FAIRIES];

// items
struct item powerup[MAX_POWERUPS];
int nth_powerup;

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
	double dx = src.x + src.r - (dest.x + dest.r);
	double dy = src.y + src.r - (dest.y + dest.r);
	double distance = sqrt(dx * dx + dy * dy);
	return distance < src.r + dest.r;
}

// checks if position is in rectbox
bool in_rect(double x, double y, struct rectbox obj)
{
	if (obj.x <= x && x <= obj.x + obj.w
			&& obj.y <= y && y <= obj.y + obj.h)
		return true;
	else
		return false;
}

// rectangle-rectangle collision
bool rect_in_rect(struct rectbox obj_1, struct rectbox obj_2)
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

// sets player at the main start position
void set_player_position()
{
	reimu.hitbox.x = FIELD_WIDTH / 2. + FIELD_OFFSET_X;
	reimu.hitbox.y = FIELD_HEIGHT * 0.9 + FIELD_OFFSET_Y;
	update_player_position(&reimu); // subpixel hitbox -> integer SDL_Rect

}

// full initialization of structs
void initialize_logic()
{
	reimu.invincible = false;
	reimu.iframes = MAX_IFRAMES;
	reimu.attack = 0;
	nth_powerup = 0;
}

// main collision detection logic
void do_collision()
{
	int i, j;

	// TODO: quadtree hitbox detection: github.com/arpit2297/Collision-Detection-using-Quad-Trees
	
	// questions/21650246/sdl-2-collision-detetection
	// player hitting things
	if (!reimu.invincible) {
		// player -- enemy bullet
		for (i = 0; i < MAX_BULLETS; i++) {
			if (circle_in_circle(ball_8x8[i].hitbox, reimu.hitbox)
					&& ball_8x8[i].active) {
//				printf("%d: HIT\n", i);
				reimu.invincible = true;
				set_player_position();
			}
		}
	
		// player -- fairy
		for (i = 0; i < MAX_FAIRIES; i++) {
			// fairy hits player
			if (circle_in_circle(fairies[i].hitbox, reimu.hitbox) &&
					fairies[i].active) {
//				printf("player-fairy HIT  \n");
				reimu.invincible = true;
				set_player_position();
			}
		}

		// player -- powerup
		for (i = 0; i < MAX_POWERUPS; i++) {
			if (rect_in_rect(powerup[i].hitbox, reimu.bigbox)
					&& powerup[i].active) {
				powerup[i].active = false;
				reimu.attack += powerup[i].value;
			}
		}
	} else if (reimu.iframes > 0) {	// decrease iframe
		reimu.iframes--;
	} else { // if iframe is emptied: reset iframe
		reimu.invincible = false;
		reimu.iframes = MAX_IFRAMES;
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
				player_bullets[j].hitbox.y = 0;

				// fairy dies
				if (fairies[i].health == 0) {
					fairies[i].active = 0;
					player_bullets[j].active = 0;
					// item drop
					powerup[nth_powerup].active = true;
		
					powerup[nth_powerup].hitbox.x = fairies[i].hitbox.x;
					powerup[nth_powerup].hitbox.y = fairies[i].hitbox.y;
		
					powerup[i].sdl.rect.x = (int)(powerup[i].hitbox.x - powerup[i].sdl.rect.w / 2.);
					powerup[i].sdl.rect.y = (int)(powerup[i].hitbox.y - powerup[i].sdl.rect.h / 2.);

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
