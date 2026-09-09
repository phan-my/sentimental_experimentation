/*
 * main.c to be compiled as `make`.
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
#include "sounds.h"
#include "render.h"
#include "common.h"
#include "input.h"
#include "menu.h"
#include "stage.h"

#define D_RECT_X 0
#define D_RECT_Y 1
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078

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

	
	/* main loop */

	// events management
	bool close = 0;
	
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
//		const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
		switch (state_menu) {
		case STATE_MAIN:
			do_main_menu_input();
			do_main_menu_screen();
			break;
		case STATE_LOADING:
			initialize_stage(current_stage);
			initialize_sounds();
			state_menu = STATE_PLAY;

			SDL_RenderClear(rend);
			SDL_RenderCopy(rend, loading.sdl.texture, NULL, &loading.sdl.rect);
			SDL_RenderPresent(rend);
			SDL_Delay(500);
			play_track("02");
			break;
		case STATE_PLAY:
			close = do_input();
			do_stage(current_stage);
			do_collision();
			

			/* appendix */
			
			do_screen();

			break;
		default:
			break;
		}
	}

	/* program termination */

	terminate_sounds();
	terminate_screen();
	
	return 0;
}

