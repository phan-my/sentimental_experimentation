/*
 * render.c includes templates to simplify SDL rendering.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */

#include <SDL2/SDL.h>
#include "render.h"


/* VARIABLES */

SDL_Window *screen;
SDL_Renderer *rend;


/* FUNCTIONS */

void check_sdl_init(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("ERROR: %s\n", SDL_GetError());
		exit(1);
	}
}

// set up window and flag
void initialize_screen(void)
{
	// create window
	screen = SDL_CreateWindow(WINDOW_TITLE,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_CENTERED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL);
	
	// check that surface was retrieved
	if (screen == NULL){
		SDL_Quit();
		exit(1);
	}

	// flag setup
	// https://studios.ptilouk.net/superfluous-returnz/blog/2023-03-14_vsync.html
	Uint32 render_flags =
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	rend = SDL_CreateRenderer(screen, -1, render_flags);
}

// close all windows and quit
void terminate_screen(void)
{
	SDL_DestroyWindow(screen);
	SDL_Quit();
}
