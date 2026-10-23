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
#include "input.h"

// main function
int main(int argc, char **argv)
{
	/* initialization */
	check_sdl_init();	// begin SDL2 setup
	initialize_screen();	// create window
	initialize_textures();
	initialize_logic();

	/* main loop */
	bool close = false;
	while (!close) {
		close = do_input();
		do_logic();
		do_screen();
	}

	/* program termination */
	terminate_sounds();
	terminate_screen();
	
	return 0;
}

