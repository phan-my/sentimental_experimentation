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

/* GLOBALS */

struct bullet shot_reimu;
struct bullet shot_reimu_a;
struct bullet shot_marisa;

/* VARIABLES */

int shot_reload = MAX_SHOT_RELOAD;
int bomb_reload = 0;

/* FUNCTIONS */

// initialization of logic behind shots
void initialize_shot()
{
	int i, j;
	// Reimu's basic shot
	shot_reimu.id = SHOT_REIMU;
	shot_reimu.active = false;
	shot_reimu.motion.hb.r = 8.;
	shot_reimu.power = 1;
	shotlist[shot_reimu.id][0] = shot_reimu;

	// Marisa's basic shot
	shot_marisa.id = SHOT_MARISA;
	shot_marisa.active = false;
	shot_marisa.motion.hb.r = 8.;
	shot_marisa.power = 1;
	shotlist[shot_marisa.id][0] = shot_marisa;

	// fill shotlist for each multiple
	for (i = 0; i < MAX_SHOTS; i++) {
		for (j = 0; j < MAX_SHOT_MULTIPLE; j++)
			shotlist[i][j] = shotlist[i][0];
		nth_shot[i] = 0; // sets smart pointer
	}
}

// use for input.c: cycle smart array and launch player bullet
void activate_reimu_shot()
{
	int i;

	// basic shot: two columns 
	for (i = 0; i < 2; i++) {
		// cycles through array
		cycle_shot_smart_pointer(SHOT_REIMU);
		// activate one bullet
		if (shot_reload == 0)
			shotlist[SHOT_REIMU][nth_shot[SHOT_REIMU]].active = 1;
	}
}

// use for input.c: cycle smart array and launch player bullet
void activate_marisa_shot()
{
	int i;
	// two columns of marisa shot
	for (i = 0; i < 2; i++) {
		// cycles through array
		cycle_shot_smart_pointer(SHOT_MARISA);
		// activate one bullet
		if (shot_reload == 0)
			shotlist[SHOT_MARISA][nth_shot[SHOT_MARISA]].active = 1;
	}
}


// update all positional values of all active shot types
void update_shot_motion()
{
	int i, j;
	// update player bullets
	for (i = 0; i < MAX_SHOTS; i++) {
	for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
		switch (i) {
		// basic player shot
		case SHOT_REIMU:
		case SHOT_MARISA:
			// continue setting specified speed to active bullet 
			if (shotlist[i][j].active) {
				shotlist[i][j].motion.v.y = -PLAYER_BULLET_SPEED;
				// v -> p
				update_position_by_velocity(&shotlist[i][j].motion);		
				// reset bullets if they fall out of bounds
				if (shotlist[i][j].motion.hb.p.y < 0)
					shotlist[i][j].active = 0;
			// case bullet is inactive
			} else {
				shotlist[i][j].motion.hb.p.y
					= player[curr_player].motion.hb.p.y;
			
				// set player bullet at player's position
				// creates two columns of player bullets 
				if (j % 2)
					shotlist[i][j].motion.hb.p.x
						= player[curr_player].motion.hb.p.x + 8;
				else
					shotlist[i][j].motion.hb.p.x
						= player[curr_player].motion.hb.p.x - 8;			
			}
			break;
		default:
			break;
		}
	}
	}
}

// complete template for player bullets
void do_shot()
{
	int i, j;
	
	// update player bullet reload
	shot_reload--;
	if (shot_reload < 0)
		shot_reload = MAX_SHOT_RELOAD;
	// update bomb counter
	bomb_reload--;
	if (bomb_reload == 0) {
		delete_danmaku_all();
	}
	
	update_shot_motion();
}

