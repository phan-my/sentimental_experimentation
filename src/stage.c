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
#include "random.h"
#include "menu.h"

/* GLOBALS */
// double stages_s[MAX_STAGES][CHECKPOINTS_STAGE_1] = {{1, 4.5, 42, 70.5}};
int stages[MAX_STAGES][CHECKPOINTS_STAGE_1] = {{60, 270, 520, 4230}};
int current_stage = 1;

// stage variables
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

// invoke during the loading screen to init stage
void initialize_stage()
{
	// initialize clock
	stage_progress_frames = 0;
	clock_gettime(CLOCK_MONOTONIC_RAW, &stage_start);
	single_activation = true;
}

/* main loop */

// invoked in the main loop
void do_stage()
{
	int i;

	nth_powerup = cycle_item_smart_pointer(nth_powerup, &itemlist[ITEM_POWERUP]);
	if (state_menu) {
		initialize_stage();
	}

	switch (current_stage) {
	case 1:
		// fairy movement
		if (stage_progress_frames > stages[current_stage - 1][0]) {
			// update the fairies' position according to specification
			if (stage_progress_frames == 70) {
				int i;
				double fairy_speed = 2.;

				int num_fairies = 12;
				
				// TODO: make this work for nonlinear movements (pointer to function?)
				// two columns of fairies
				for (i = 0; i < num_fairies; i++) {
					if (!enemylist[ENEMY_FAIRY][i].active) {
						// creates left column and right column
						enemylist[ENEMY_FAIRY][i].motion.hb.p.x = FIELD_OFFSET_X + FIELD_WIDTH / 2
							+ pow(-1, i) * 20;
						// positions the fairies apart on the y axis
						enemylist[ENEMY_FAIRY][i].motion.hb.p.y = FIELD_OFFSET_Y - i * 20;

						// spawner

						enemylist[ENEMY_FAIRY][i].motion.o.x = FIELD_OFFSET_X + FIELD_WIDTH / 2
							+ pow(-1, i) * 20;
						// positions the fairies apart on the y axis
						enemylist[ENEMY_FAIRY][i].motion.o.y = FIELD_OFFSET_Y - i * 20;

						// TODO: debug as to why stopped fairies appear again next phase
	//					if (fairies[i].hitbox.p.y <= stopping_line)
						// fairy moves straight down
						enemylist[ENEMY_FAIRY][i].motion.v.x = 0.;
						enemylist[ENEMY_FAIRY][i].motion.v.y = fairy_speed;
						enemylist[ENEMY_FAIRY][i].active = true;
					}
				}
			}
		}


		if (stage_progress_frames >= stages[current_stage - 1][1] &&
				stage_progress_frames
				<= stages[current_stage - 1][2])
{
		if (stage_progress_frames % 20 == 0) {
			// ring
			double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;

			nth_ball_8x8 =
				launch_aimed_ring(&(bulletlist[BULLET_BALL_8x8]),
						nth_ball_8x8, x, y, 18, 2.4);
			nth_ball_8x8 =
				launch_aimed_ring(&(bulletlist[BULLET_BALL_8x8]),
						nth_ball_8x8, x+60, y, 17, 3.);
			nth_ball_8x8 =
				launch_aimed_ring(&(bulletlist[BULLET_BALL_8x8]),
						nth_ball_8x8, x-60, y, 17, 3.);
		}
}


		if (stage_progress_frames >= stages[current_stage - 1][2] &&
				stage_progress_frames
				<= stages[current_stage - 1][3])
{
		if (stage_progress_frames % 20 == 0) {
			// spread
			double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
			
			nth_ball_8x8 =
				launch_aimed_spread(&(bulletlist[BULLET_BALL_8x8]),
						nth_ball_8x8, x, y, 17, 2.4,
						M_PI / 2.);
		}
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
}


