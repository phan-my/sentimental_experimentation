/*
 * main.c to be compiled as `make`.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#include <stdio.h>
#include <stdbool.h>

#include "logic.h"
#include "sounds.h"
#include "render.h"
#include "common.h"
#include "input.h"
#include "menu.h"
#include "stage.h"

// main function
int main(int argc, char **argv)
{
	/* initialization */
	check_sdl_init();	// begin SDL2 setup
	initialize_screen();	// create window
	initialize_textures();
	initialize_logic();

	/* main loop */
	bool close = 0;
	while (!close) {
//		const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
		switch (state_menu) {
		case STATE_MAIN:
			do_main_menu_input();
			do_main_menu_screen();
			break;
		case STATE_LOADING:
			initialize_stage();
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
			do_stage();
			do_collision();
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

