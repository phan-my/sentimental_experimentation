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
#include "danmaku.h"


/* GLOBALS */
// double stages_s[MAX_STAGES][CHECKPOINTS_STAGE_1] = {{1, 4.5, 42, 70.5}};
int stages[MAX_STAGES][CHECKPOINTS_STAGE_1] = {{60, 270, 2520, 4230}};
int current_stage = 1;

// stage variables
int moving = 1;
double speed[MAX_BULLET_MULTIPLE];
double cap_speed;
double angles[MAX_BULLET_MULTIPLE];

int stopping_line = FIELD_OFFSET_Y + 200;
double fairy_speed = 2.;

// timing
struct timespec stage_start;
struct timespec stage_now;
uint64_t stage_progress;
int stage_progress_frames = 0;
double stage_progress_seconds;
bool single_activation; // controls start of fairy spawning

// fps count
double frames[10000];
struct timespec dt_start;
struct timespec dt_end;
uint64_t dt; // in microseconds

int hour, minute, second;



/* FUNCTIONS */

// fly in sine waves down, sparsely
void initialize_fairy_single_1()
{
	int i;
	double fairy_speed = 2.;

	int num_fairies = 12;
	
	// two columns of fairies
	for (i = 0; i < num_fairies; i++) {
		// creates left column and right column
		enemylist[ENEMY_FAIRY][i].motion.p.x = FIELD_OFFSET_X + FIELD_WIDTH / 2
			+ pow(-1, i) * 20;
		// positions the fairies apart on the y axis
		enemylist[ENEMY_FAIRY][i].motion.p.y = FIELD_OFFSET_Y - i * 20;
	}
}

// invoke during the loading screen to init stage
void initialize_stage()
{
	int i;
	
	stage_progress = 0;
	
	switch (current_stage) {
	case 1:
		// "snowball" bullets
		for (i = 0; i < MAX_BULLET_MULTIPLE; i++) {
			bulletlist[BULLET_BALL_8x8][i].motion.p.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			bulletlist[BULLET_BALL_8x8][i].motion.p.y = FIELD_HEIGHT * (1. / 4)
				+ FIELD_OFFSET_Y;
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
		for (i = 0; i < MAX_BULLET_MULTIPLE; i++) {
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
		for (i = 0; i < MAX_BULLET_MULTIPLE; i++) {
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

// invoked in the main loop
void do_stage()
{
	int i;

	// update array pointers
	while (powerup[nth_powerup].active) {
		nth_powerup++;
	}

	switch (current_stage) {
	case 1:
		// fairy movement
		if (stage_progress_frames > stages[current_stage - 1][0]) {
			// update the fairies' position according to specification
			if (!single_activation) {
				for (i = 0; i < 12; i++) {
					// TODO: debug as to why stopped fairies appear again next phase
//					if (fairies[i].hitbox.p.y <= stopping_line)
					// fairy moves straight down
					enemylist[ENEMY_FAIRY][i].motion.v.x = 0.;
					enemylist[ENEMY_FAIRY][i].motion.v.y = fairy_speed;
				}
			// activate the fairies
			} else {
				for (i = 0; i < 12; i++) {
					// activate fairies
					if (!enemylist[ENEMY_FAIRY][i].active)
						enemylist[ENEMY_FAIRY][i].active = true;
				}
				single_activation = false;
			}
		}


		if (stage_progress_frames == stages[current_stage - 1][1])
{
		// ring
		struct position spawner;
		spawner.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
		spawner.y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;

		nth_ball_8x8 = launch_ring(&(bulletlist[BULLET_BALL_8x8]), nth_ball_8x8,
				spawner, 8, 2., M_PI / 3.);
}

		/* timing */
		clock_gettime(CLOCK_MONOTONIC_RAW, &stage_now);
		stage_progress = (stage_now.tv_sec - stage_start.tv_sec)
			* 1000000. + (stage_now.tv_nsec - stage_start.tv_nsec) 
			/ 1000.;
		stage_progress_frames++;
//		printf("%d\n", stage_progress_frames);
		stage_progress_seconds = stage_progress / 1000000.;
//		printf("%d time\n", stage_progress);
		
		
	// universal things in every stage
	default:
		break;
	}

	// item falls
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (powerup[i].active) {
			powerup[i].hitbox.p.y += 1.;
			powerup[i].sdl.rect.x = (int)(powerup[i].hitbox.p.x - powerup[i].sdl.rect.w / 2.);
			powerup[i].sdl.rect.y = (int)(powerup[i].hitbox.p.y - powerup[i].sdl.rect.h / 2.);
		}
	}
}


