/*
 * stage.c is developed to be the editable file for the game engine.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include "stage.h"
#include "logic.h"
#include "render.h"
#include "random.h"


/* GLOBALS */
double stages[MAX_STAGES][MAX_CHECKPOINTS] = {1, 14.5, 42, 70.5};
int current_stage = 1;


/* FUNCTIONS */
/*
// "snowball" bullets
for (i = 0; i < MAX_BULLETS; i++) {
//		ball_8x8[i].w /= scale;
//		ball_8x8[i].h /= scale;
        ball_8x8[i].hitbox.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
        ball_8x8[i].hitbox.y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
        ball_8x8[i].hitbox.r = 3.8;
        update_ball_position(&ball_8x8[i]);
}
*/

// fly in sine waves down, sparsely
void fairy_action_1()
{
	int i;
	double fairy_speed = 1.;
	
	// fairy
	for (i = 0; i < MAX_FAIRIES; i++) {
		fairies[i].hitbox.x = FIELD_OFFSET_X + randint(0, FIELD_WIDTH);
		fairies[i].hitbox.y = FIELD_OFFSET_Y;
		update_enemy_position(&fairies[i]);
	}
}

// invoke during the loading screen to init stage
void initialize_stage(int stage)
{
	int i;
	switch (stage) {
	case 1:
		// "snowball" bullets
		for (i = 0; i < MAX_BULLETS; i++) {
	//		ball_8x8[i].w /= scale;
	//		ball_8x8[i].h /= scale;
			ball_8x8[i].hitbox.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			ball_8x8[i].hitbox.y = FIELD_HEIGHT * (1. / 4)
				+ FIELD_OFFSET_Y;
			ball_8x8[i].hitbox.r = 3.8;
			update_ball_position(&ball_8x8[i]);
		}

		fairy_action_1();



		break;
	case 2:
		break;
	default:
		break;
	}
}
