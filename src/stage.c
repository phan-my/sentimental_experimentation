/*
 * stage.c is developed to be the editable file for the game engine.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include <math.h>
#include <time.h>

#include "stage.h"
#include "common.h"
#include "logic.h"
#include "render.h"
#include "random.h"


/* GLOBALS */
double stages[MAX_STAGES][CHECKPOINTS_STAGE_1] = {{1, 14.5, 42, 70.5}};
int current_stage = 1;

// stage variables
double moving = 1;
double speed[MAX_BULLETS];
double cap_speed;
double angles[MAX_BULLETS];

int stopping_line = FIELD_OFFSET_Y + 200;
double fairy_speed = 2.;

// timing
struct timespec stage_start;
struct timespec stage_now;
uint64_t stage_progress;
double stage_progress_seconds;
bool single_activation; // controls start of fairy spawning

// fps count
double frames[10000];
struct timespec dt_start;
struct timespec dt_end;
uint64_t dt; // in microseconds

int hour, minute, second;



/* FUNCTIONS */

/* initialization */
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
void initialize_fairy_single_1()
{
	int i;
	double fairy_speed = 1.;

	int num_fairies = 12;
	
	// two columns of fairies
	for (i = 0; i < num_fairies; i++) {
		// creates left column and right column
		fairies[i].hitbox.x = FIELD_OFFSET_X + FIELD_WIDTH / 2
			+ pow(-1, i) * 20;
		// positions the fairies apart on the y axis
		fairies[i].hitbox.y = FIELD_OFFSET_Y - i * 20;
	}
}

// invoke during the loading screen to init stage
void initialize_stage()
{
	int i;
	switch (current_stage) {
	case 1:
		// "snowball" bullets
		for (i = 0; i < MAX_BULLETS; i++) {
	//		ball_8x8[i].w /= scale;
	//		ball_8x8[i].h /= scale;
			ball_8x8[i].hitbox.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			ball_8x8[i].hitbox.y = FIELD_HEIGHT * (1. / 4)
				+ FIELD_OFFSET_Y;
			ball_8x8[i].hitbox.r = 3.8;
		}

		initialize_fairy_single_1();


		// NORMAL (SPIRAL SPEEDS
		// set bullet speeds
		/*
		// variables for bullet movement
		double r;
		double phi = 0.;
		double a = 1;
		double speed[MAX_BULLETS];
		for (i = 0; i < MAX_BULLETS; i++)
			speed[i] = (double) i / 50;
		*/


		/* jellyfish */

		// set bullet speeds
		for (i = 0; i < MAX_BULLETS; i++) {
			if (i % 40 < 20)
				cap_speed = 2.;
			else
				cap_speed = 3.;
			if (i % 2)
				speed[i] = (double) 1. + i / 50. / 6.;
			else
				speed[i] = (double) 1. + i / 67. / 6.;
			if (speed[i] > cap_speed)
				speed[i] = cap_speed;
		}

		// create double version of dest for subpixel precision
		for (i = 0; i < MAX_BULLETS; i++) {
			if (i % 2)
				angles[i] = i * GOLDEN_RATIO;
			else
				angles[i] = i * 1.3 * GOLDEN_RATIO;
		}

		// initialize clock
		clock_gettime(CLOCK_MONOTONIC_RAW, &stage_start);
		single_activation = true;

		break;
	case 2:
		break;
	default:
		break;
	}
}


/* main loop */

// complete template for player bullets
void do_player_bullets()
{
	int i;
	// update player bullets
	for (i = 0; i < MAX_PLAYER_BULLETS; i++) {
		// move activated bullet by specified speed
		if (player_bullets[i].active) {
			player_bullets[i].hitbox.y -=
				PLAYER_BULLET_SPEED;
			update_ball_position(&player_bullets[i]);
		}
		// reset bullets if they fall out of bounds
		if (player_bullets[i].hitbox.y < 0 ||
				!player_bullets[i].active) {
			player_bullets[i].active = 0;
			player_bullets[i].hitbox.x = reimu.hitbox.x;
			player_bullets[i].hitbox.y = reimu.hitbox.y;
		}
	}
}

// invoked in the main loop
void do_stage()
{
	int i;
	switch (current_stage) {
	case 1:
		/* FAIRY MOVEMENT */
		
		if (stage_progress_seconds > stages[current_stage - 1][0]) {
			// update the fairies' position according to specification
			if (!single_activation) {
				for (i = 0; i < 12; i++) {
					// fairy moves straight down
					if (fairies[i].hitbox.y < stopping_line)
						fairies[i].hitbox.y += fairy_speed;
					update_enemy_position(&fairies[i]);
				}
			// activate the fairies
			} else {
				for (i = 0; i < 12; i++) {
					// activate fairies
					if (!fairies[i].active)
						fairies[i].active = true;
				}
				single_activation = false;

			}
		}
		
		/* BULLET MOVEMENTS */
		/*
		// sine movement
		double period = 300;
		double amplitude = 100;
		for (i = 0; i < MAX_BULLETS; i++) {
			d_ball_8x8[i][D_RECT_Y] += amplitude * 2 * PI / period *
				cos(2 * PI * ball_8x8[i].x / period);
			ball_8x8[i].y = (int)d_ball_8x8[i][D_RECT_Y];
			ball_8x8[i].x = (int)(d_ball_8x8[i][D_RECT_X] += speed[i]);
		}
		*/

		/* spiral TRAIL movement
		r = a * phi;
		for (i = 0; i < MAX_BULLETS; i++) {
			ball_8x8[i].y = (d_ball_8x8[i][D_RECT_Y] = speed[i] * r * cos(phi) + SCREEN_HEIGHT / 2);
			ball_8x8[i].x = (d_ball_8x8[i][D_RECT_X] = speed[i] * r * sin(phi) + SCREEN_WIDTH / 2);
		}
		phi += 0.05;
		*/

		/*
		// sunflower spirals
		for (i = 0; i < moving; i++) {
			double p = 1.;
			d_ball_8x8[i][D_RECT_X] += 3. * cos(angles[i] / p);
			d_ball_8x8[i][D_RECT_Y] += 3. * sin(angles[i] / p);
			ball_8x8[i].x = d_ball_8x8[i][D_RECT_X];
			ball_8x8[i].y = d_ball_8x8[i][D_RECT_Y];
		}

		if (moving < MAX_BULLETS - 60) {
			moving += 5 * GOLDEN_RATIO;
//			printf("%lf\n", moving);
		} else {
			printf("Recycle your bullets!\n");
			break;
		}
		*/


		if (stage_progress_seconds > stages[current_stage - 1][1])
{
		// Jellyfish
		// speed[i] = (double)i / 100;
		for (i = 0; i < moving; i++) {
			ball_8x8[i].hitbox.x += speed[i] * cos(angles[i]);
			ball_8x8[i].hitbox.y += speed[i] * sin(angles[i]);
			update_ball_position(&ball_8x8[i]);
		}
		if (moving < MAX_BULLETS)
			moving += 2;
}

		/* timing */
		clock_gettime(CLOCK_MONOTONIC_RAW, &stage_now);
		stage_progress = (stage_now.tv_sec - stage_start.tv_sec)
			* 1000000. + (stage_now.tv_nsec - stage_start.tv_nsec) 
			/ 1000.;
		stage_progress_seconds = stage_progress / 1000000.;
//		printf("%d time\n", stage_progress);
		
		
	// universal things in every stage
	default:
		do_player_bullets();
		break;
	}
}


