/*
 * common.c includes misc functions not classified in any other include file.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */

#include <SDL2/SDL.h>
#include "input.h"

/* VARIABLES */
// questions/12750796
// const Uint8 *keyboard_states;



/* FUNCTIONS */

// returns 1 to exit the main loop
int do_input()
{
	// questions/1252976
	// github: mikeanthonywild/sdl-shoot-em-up
	bool key_down = 0;
	SDL_Event event;
	const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);

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
}

