/*
 * player.c includes functions to manipulate player operations.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */

#include <stdbool.h>
#include "player.h"
#include "logic.h"

/* VARIABLES */

/* FUNCTIONS */


void initialize_players()
{
	int i;
	// set initial values common to all players
	for (i = 0; i < MAX_PLAYERS; i++) {
		player[i].invincible = false;
		player[i].iframes = MAX_IFRAMES;
		player[i].power = 0.;
		player[i].score = 0;
		player[i].motion.hb.r = 2.;
		// TODO: redraw to deal with grazing
		player[i].grazebox.r = 8.;
		player[i].itembox.r = player[i].grazebox.r * 2.;
		player[i].health = 2;
		player[i].bombs = 2;
	}

	// set initial values for each individual player
	player[PLAYER_REIMU].speed = REIMU_DEFAULT_SPEED;
	player[PLAYER_REIMU].focus = REIMU_FOCUS_SPEED;
	
	player[PLAYER_MARISA].speed = MARISA_DEFAULT_SPEED;
	player[PLAYER_MARISA].focus = MARISA_FOCUS_SPEED;
}

