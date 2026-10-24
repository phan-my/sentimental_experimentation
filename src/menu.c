/*
 * menu.c includes the main menu and pausation templates.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include "menu.h"
#include <SDL2/SDL_ttf.h>
#include "logic.h"
#include "render.h"
#include "sounds.h"


/* GLOBALS */
int state_menu = STATE_ON_POWER;

// invokes all functions during the loading screen
void do_loading()
{
	// change to loading screen
	SDL_RenderClear(rend);
	SDL_RenderCopy(rend, loading.sdl.texture, NULL,
			&loading.sdl.rect);
	SDL_RenderPresent(rend);
	
	// wait to prevent flashing lights
	SDL_Delay(500);

	// mixer
	do_sounds();

	state_menu = STATE_PLAY;
}

