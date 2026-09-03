/*
 * main.c to be compiled as `gcc main.c -lm -lSDL2 -lSDL2_image -lSDL2_mixer`.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include "logic.h"
#include "random.h"
#include "sounds.h"
#include "render.h"
#include "common.h"
#include "input.h"
#include "menu.h"

#define D_RECT_X 0
#define D_RECT_Y 1
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078
#define GOLDEN_RATIO 0.6180339887498948482045868343656381177203091798057628621

// invoke as dest = f(d_dest, speed, turns)
double *launch_straight(double *d_dest, double speed, double turns)
{
	d_dest[D_RECT_X] += speed * cos(turns);
	d_dest[D_RECT_Y] += speed * sin(turns);
	return d_dest;
}

// main function
int main(int argc, char **argv)
{
	int i, j;

	check_sdl_init();	// begin SDL2 setup
	initialize_screen();	// create window

	/* image loader */

	initialize_textures();
	
	/* positioning hitboxes */

	// background
	SDL_Rect border_dest;
	SDL_QueryTexture(border_tex, NULL, NULL, &border_dest.w,
				&border_dest.h);
	border_dest.x = 0;
	
	// menu
	SDL_Rect main_menu_dest;
	SDL_QueryTexture(main_menu, NULL, NULL, &main_menu_dest.w, &main_menu_dest.h);
	main_menu_dest.x = 0;

	// player
	// create object
	SDL_QueryTexture(reimu.sdl.texture, NULL, NULL, &reimu.sdl.rect.w,
				&reimu.sdl.rect.h);
	reimu.hitbox.x = FIELD_WIDTH / 2. + FIELD_OFFSET_X;
	reimu.hitbox.y = FIELD_HEIGHT * 0.75 + FIELD_OFFSET_Y;
	update_player_position(&reimu);
	reimu.hitbox.r = 2.;

	// "snowball" bullets
	for (i = 0; i < MAX_BULLETS; i++) {
		SDL_QueryTexture(tex, NULL, NULL, &dest[i].sdl.rect.w,
				&dest[i].sdl.rect.h);
//		dest[i].w /= scale;
//		dest[i].h /= scale;
		dest[i].hitbox.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
		dest[i].hitbox.y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
		dest[i].hitbox.r = 3.8;
		update_ball_position(&dest[i]);
	}


	// fairy
	for (i = 0; i < MAX_FAIRIES; i++) {
		SDL_QueryTexture(fairies[0].sdl.texture, NULL, NULL,
				&fairies[i].sdl.rect.w, &fairies[i].sdl.rect.h);
		fairies[i].hitbox.x = FIELD_OFFSET_X + randint(0, FIELD_WIDTH);
		fairies[i].hitbox.y = FIELD_OFFSET_Y;
		update_enemy_position(&fairies[i]);
		fairies[i].hitbox.r = 8.;
		fairies[i].health = 2;
	}

	// player_bullets
	for (i = 0; i < MAX_PLAYER_BULLETS; i++) {
		SDL_QueryTexture(player_bullet_texture, NULL, NULL,
				&player_bullets[i].sdl.rect.w, &player_bullets[i].sdl.rect.h);
		player_bullets[i].hitbox.x = reimu.hitbox.x;
		player_bullets[i].hitbox.y = reimu.hitbox.y;
		player_bullets[i].hitbox.r = 8.;
		player_bullets[i].power = 1;
	}

	/* music loader */
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	initialize_sounds();
	play_track("02");


	/* numerical setup */
	
	// JELLYFISH
	// set bullet speeds
	double speed[MAX_BULLETS];
	double cap_speed;
	for (i = 1; i < MAX_BULLETS; i++) {
		if (i % 40 < 20)
			cap_speed = 2.;
		else
			cap_speed = 3.;
		if (i % 2)
			speed[i] = (double) 1. + i / 50. / 6.;
		else
			speed[i] = (double) 1. + i / 67. / 6.;
		if (speed[i] > cap_speed)
			speed[i] = cap_speed;
	}

	// NORMAL (SPIRAL SPEEDS
	// set bullet speeds
	/*
	double speed[MAX_BULLETS];
	for (i = 0; i < MAX_BULLETS; i++) 
		speed[i] = (double) i / 50;
	*/
	
	// create double version of dest for subpixel precision
	double angles[MAX_BULLETS];
	for (i = 0; i < MAX_BULLETS; i++) {
		update_ball_position(&dest[i]);
		if (i % 2)
			angles[i] = i * GOLDEN_RATIO;
		else
			angles[i] = i * 1.3 * GOLDEN_RATIO;
	}
	
	// variables for bullet movement
	double r;
	double phi = 0.;
	double a = 1;
	double moving = 1;
	
	// player speed
	// https://en.touhouwiki.net/wiki/User:Arcorann/Character_Speeds#Massive_chart
	double marisa_speed = REIMU_DEFAULT_SPEED * 1.5;
	// for lshift focus
	double factored_speed = REIMU_DEFAULT_SPEED;
	double diagonal;

	// player bullet
	double player_bullet_speed = 8.;

	/* main loop */

	// events management
	bool close = 0;
	SDL_Event event;
	bool key_down = 0;
	int scanned_key = 0;

	// timing
	double frames[10000];
	struct timespec start_game;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_game);
	int hour, minute, second;

	struct timespec dt_start;
	struct timespec dt_end;
	uint64_t dt; // in microseconds
	clock_gettime(CLOCK_MONOTONIC_RAW, &dt_start);
	clock_gettime(CLOCK_MONOTONIC_RAW, &dt_end);
	Uint64 ticks = 0;

	// main loop
	while (!close) {
		const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
		switch (state_menu) {
		case STATE_MAIN:
			while (SDL_PollEvent(&event)) {
				switch (event.type) {

				// close button
				case SDL_QUIT:
					return 1;
					break;

				// any key pressed
				case SDL_KEYDOWN:
					key_down = 1;
					scanned_key = event.key.keysym.scancode;
					break;
				case SDL_KEYUP:
		// 				key_down = 0;
		// 				scanned_key = 0;
					break;
				default:
					break;
				}
			}

			// avoids key repeat delay
			// questions/21311824/sdl2-key-repeat-delay
			if (key_down) {

				if (keyboard_states[SDL_SCANCODE_Z])
					state_menu = STATE_PLAY;
			}

			SDL_RenderClear(rend);
			SDL_RenderCopy(rend, main_menu, NULL, &main_menu_dest);
		
			// double buffer
			SDL_RenderPresent(rend);
			break;
		case STATE_PLAY:
			close = do_input();

			// update player bullets
			for (i = 0; i < MAX_PLAYER_BULLETS; i++) {
				// update bullets if they fall out of bounds
				if (player_bullets[i].hitbox.y < 0 ||
						!player_bullets[i].active) {
					player_bullets[i].active = 0;
					player_bullets[i].hitbox.x = reimu.hitbox.x;
					player_bullets[i].hitbox.y = reimu.hitbox.y;
				}
				if (player_bullets[i].active) {
					player_bullets[i].hitbox.y -= player_bullet_speed;
					update_ball_position(&player_bullets[i]);
				}
			}

			/* BULLET MOVEMENTS */

			/*
			// sine movement
			double period = 300;
			double amplitude = 100;
			for (i = 0; i < MAX_BULLETS; i++) {
				d_dest[i][D_RECT_Y] += amplitude * 2 * PI / period * 
					cos(2 * PI * dest[i].x / period);
				dest[i].y = (int)d_dest[i][D_RECT_Y];
				dest[i].x = (int)(d_dest[i][D_RECT_X] += speed[i]);
			}
			*/
			
			/* spiral TRAIL movement
			r = a * phi;
			for (i = 0; i < MAX_BULLETS; i++) {
				dest[i].y = (d_dest[i][D_RECT_Y] = speed[i] * r * cos(phi) + SCREEN_HEIGHT / 2);
				dest[i].x = (d_dest[i][D_RECT_X] = speed[i] * r * sin(phi) + SCREEN_WIDTH / 2);
			}
			phi += 0.05;
			*/

			/*
			// sunflower spirals
			for (i = 0; i < moving; i++) {
				double p = 1.;
				d_dest[i][D_RECT_X] += 3. * cos(angles[i] / p);
				d_dest[i][D_RECT_Y] += 3. * sin(angles[i] / p);
				dest[i].x = d_dest[i][D_RECT_X];
				dest[i].y = d_dest[i][D_RECT_Y];
			}
			
			if (moving < MAX_BULLETS - 60) {
				moving += 5 * GOLDEN_RATIO;
	//			printf("%lf\n", moving);
			} else {
				printf("Recycle your bullets!\n");
				break;
			}
			*/


			// Jellyfish
			// speed[i] = (double)i / 100;
			for (i = 0; i < moving; i++) {
				dest[i].hitbox.x += speed[i] * cos(angles[i]);
				dest[i].hitbox.y += speed[i] * sin(angles[i]);
				update_ball_position(&dest[i]);
			}
			
			if (moving < MAX_BULLETS)
				moving += 2;


			/* ENEMY MOVEMENT */

			int stopping_line = FIELD_OFFSET_Y + 200;
			double fairy_speed = 1.;
			
			for (i = 0; i < MAX_FAIRIES; i++) {
				double diff = stopping_line - fairies[i].hitbox.y;
				if (fairies[i].hitbox.y < stopping_line)
					fairies[i].hitbox.y += fairy_speed ;

				update_enemy_position(&fairies[i]);
			}
			

			/* COLLISION DETECTION*/
			// TODO: quadtree hitbox detection
			// questions/21650246/sdl-2-collision-detetection
			// github.com/arpit2297/Collision-Detection-using-Quad-Trees
			
			// player -- enemy bullet
			for (i = 0; i < MAX_BULLETS; i++) {
				if (is_hit(dest[i].hitbox, reimu.hitbox)) {
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
			

			/* appendix */

			// clear screen
			SDL_RenderClear(rend);
			SDL_RenderCopy(rend, reimu.sdl.texture, NULL, &reimu.sdl.rect);
			for (i = 0; i < MAX_BULLETS; i++)
				SDL_RenderCopy(rend, tex, NULL, &dest[i].sdl.rect);
			for (i = 0; i < MAX_FAIRIES; i++) {
				if (fairies[i].active) {
					SDL_RenderCopy(rend, fairies[0].sdl.texture,
							NULL, &fairies[i].sdl.rect);
				}
			}
			for (i = 0; i < MAX_PLAYER_BULLETS; i++)
				if (player_bullets[i].active)
					SDL_RenderCopy(rend, player_bullet_texture,
							NULL,
							&player_bullets[i].sdl.rect);

			SDL_RenderCopy(rend, border_tex, NULL, &border_dest);

			SDL_RenderPresent(rend);
			break;
		default:
			break;
		}
		// double buffer
	}

	/* program termination */

	terminate_sounds();
	terminate_screen();
	
	return 0;
}

