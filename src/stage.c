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
// double stages_s[MAX_STAGES][MAX_CHECKPOINTS] = {{1, 4.5, 42, 70.5}};
int stages[MAX_STAGES][MAX_CHECKPOINTS] = {{4, 60, 270, 320, 430}};
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

/* patterns */

// 17-spread (aimed) of ball_8x8
void pattern_spread_test()
{
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
	if (stage_progress_frames % 20 == 0) {	
		nth_bullet[BULLET_BALL_8x8] =
			launch_aimed_spread(&(bulletlist[BULLET_BALL_8x8]),
					nth_bullet[BULLET_BALL_8x8], x, y, 17, 2.4,
					M_PI / 2.);
	}
}

void pattern_two_fairy_columns()
{
	// update the fairies' position according to specification
	if (stage_progress_frames == 70) {
		int i;
		double fairy_speed = 2.;

		int num_fairies = 12;
		
		// TODO: make this work for nonlinear movements (pointer to function?)
		// two columns of fairies
		for (i = 0; i < num_fairies; i++) {
			if (!enemylist[ENEMY_FAIRY_BLUE][i].active) {
				// creates left column and right column
				enemylist[ENEMY_FAIRY_BLUE][i].motion.hb.p.x = FIELD_OFFSET_X + FIELD_WIDTH / 2
					+ pow(-1, i) * 20;
				// positions the fairies apart on the y axis
				enemylist[ENEMY_FAIRY_BLUE][i].motion.hb.p.y = FIELD_OFFSET_Y - i * 20;

				// spawner

				enemylist[ENEMY_FAIRY_BLUE][i].motion.o.x = FIELD_OFFSET_X + FIELD_WIDTH / 2
					+ pow(-1, i) * 20;
				// positions the fairies apart on the y axis
				enemylist[ENEMY_FAIRY_BLUE][i].motion.o.y = FIELD_OFFSET_Y - i * 20;

				// TODO: debug as to why stopped fairies appear again next phase
//				if (fairies[i].hitbox.p.y <= stopping_line)
				// fairy moves straight down
				enemylist[ENEMY_FAIRY_BLUE][i].motion.v.x = 0.;
				enemylist[ENEMY_FAIRY_BLUE][i].motion.v.y = fairy_speed;
				enemylist[ENEMY_FAIRY_BLUE][i].active = true;
			}
		}
	}
}

// testing moving spawner pattern with rings
void pattern_moving_spawner_test()
{
	int phase_progress = get_phase_progress();

	double x0 = 50;
	double y0 = 50;
	double x1 = 512;
	double y1 = 50;

	int duration = 60 * 5;
	double t = (double) (phase_progress % duration) / (double) duration; 

	if (phase_progress % 20 == 0) {
		nth_bullet[BULLET_BALL_8x8] =
			launch_aimed_ring(&(bulletlist[BULLET_BALL_8x8]),
					nth_bullet[BULLET_BALL_8x8],
					lerp(x0, x1, t), lerp(y0, y1, t), 18, 2.4);
	}
}

/* main loop */

// invoked in the main loop
void do_stage()
{
	int i;

//	nth_item[ITEM_POWERUP] = cycle_item_smart_pointer(nth_item[ITEM_POWERUP], &itemlist[ITEM_POWERUP]);
	if (state_menu) {
		initialize_stage();
	}

	switch (current_stage) {
	case 1:
		// fairy movement
		if (stage_progress_frames > stages[current_stage - 1][0]) {
		  	pattern_two_fairy_columns();
		}
		
		if (stage_progress_frames > stages[current_stage - 1][1]) {
			pattern_moving_spawner_test();
		}


/*
		if (stage_progress_frames >= stages[current_stage - 1][1] &&
				stage_progress_frames
				<= stages[current_stage - 1][2])
{
		if (stage_progress_frames % 20 == 0) {
			// ring
			double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;

			nth_bullet[BULLET_BUBBLE] =
				launch_aimed_ring(&(bulletlist[BULLET_BUBBLE]),
						nth_bullet[BULLET_BUBBLE], x, y, 8, 2.4);
			nth_bullet[BULLET_BUBBLE] =
				launch_aimed_ring(&(bulletlist[BULLET_BUBBLE]),
						nth_bullet[BULLET_BUBBLE], x+60, y, 7, 3.);
			nth_bullet[BULLET_BUBBLE] =
				launch_aimed_ring(&(bulletlist[BULLET_BUBBLE]),
						nth_bullet[BULLET_BUBBLE], x-60, y, 7, 3.);
		}
}


		if (stage_progress_frames >= stages[current_stage - 1][2] &&
				stage_progress_frames
				<= stages[current_stage - 1][3])
{
		pattern_spread_test();
}

		if (stage_progress_frames >= stages[current_stage - 1][3])
{
		if (stage_progress_frames % 120 == 0) {
			// stack
			double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
			
			nth_bullet[BULLET_BALL_8x8] =
				launch_stack(&(bulletlist[BULLET_BALL_8x8]),
						nth_bullet[BULLET_BALL_8x8], x, y, 6, 2.4, 5.,
						M_PI / 2.);
			nth_bullet[BULLET_BALL_8x8] =
				launch_aimed_stack(&(bulletlist[BULLET_BALL_8x8]),
						nth_bullet[BULLET_BALL_8x8], x, y, 6, 2.4, 5.);
		}
}
*/

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


