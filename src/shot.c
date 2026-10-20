/*
 * shot.c deals with the player's bullets.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */
#include "shot.h"
#include "logic.h"
#include "pointer.h"

/* VARIABLES */

int reload = MAX_RELOAD;

/* FUNCTIONS */

// use for input.c: cycle smart array and launch player bullet
void activate_reimu_shot()
{
	int i;

	// basic shot: two columns 
	for (i = 0; i < 2; i++) {
		// cycles through array
		cycle_shot_smart_pointer(SHOT_REIMU);
		// activate one bullet
		if (reload == 0)
			shotlist[SHOT_REIMU][nth_shot[SHOT_REIMU]].active = 1;
	}
}

