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
#include "pointer.h"

/* GLOBALS */
// double stages_s[MAX_STAGES][MAX_CHECKPOINTS] = {{1, 4.5, 42, 70.5}};
int stages[MAX_STAGES][MAX_CHECKPOINTS] = {{4, 0*60, 1*60, 2*60, 111*50}};
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
		launch_aimed_spread(BULLET_BALL_8x8, x, y, 17, 2.4,
				M_PI / 2.);
	}
}

void pattern_two_fairy_columns()
{
	// update the fairies' position according to specification
	if (get_phase_progress() == 0) {
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

	printf("%d\n", phase_progress % duration);
	if (phase_progress % 20 == 0) {
		launch_aimed_ring(BULLET_BALL_8x8,
				lerp(x0, x1, t), lerp(y0, y1, t), 18, 2.4);
	}
}

void pattern_boundary_of_wave_and_particle()
{
	int phase_progress = get_phase_progress();
	
	// globals
	double alpha = M_PI * (0.3);
	
	// constants
	// values at https://youtu.be/cpz4mnXJYb0?t=318
	double alpha_v = -0.078539816;
	double alpha_a = 0.003926991;
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	int delay = 2;
	
	if (get_phase_progress() % delay == 0) {
		// implementation for "global" variables by basing the 
		// current value (alpha_v) on phase_progress and firing rate
//		alpha_v += alpha_a * (double)phase_progress / delay;
		alpha += alpha_v * (double)phase_progress / delay + alpha_a * 0.5 * pow((double)phase_progress / delay, 2);
		
		launch_ring(BULLET_BALL_8x8, x, y, 8, 3, alpha);
	}
}

/* main loop */

double t = 0;
int global_i = 1;

// invoked in the main loop
void do_stage()
{
	int i;

	// if game state is at a menu/loading screen, initialize stage
	if (state_menu) {
		initialize_stage();
	}

	// main timing of stage elements
	switch (current_stage) {
	case 1:
//		printf("%lf ; %d ; %lf \n", (double)stage_progress_frames / 60., get_phase(), (double)get_phase_progress() / 60.);
		switch (get_phase()) {
		// FIXME: why is there a delay before it starts?
		case 1:
	 		pattern_two_fairy_columns();
	 		break;
		case 2:
			pattern_moving_spawner_test();
			break;
		case 3:
			pattern_boundary_of_wave_and_particle();
			break;
		case -1:
			double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
			
			struct position p_0;
			p_0.x = x - 20;
			p_0.y = y;
			
			struct position p_1;
			p_1.x = x - 20;
			p_1.y = y - 20;

			struct position p_2;
			p_2.x = x + 20;
			p_2.y = y - 20;

			struct position p_3;
			p_3.x = x + 20;
			p_3.y = y;

			if (t > 1)
				global_i = -1;
			if (t < 0)
				global_i = 1;
			t += global_i * 0.05;
			struct position s = cubic_bezier(p_0, p_1, p_2, p_3, t);

			if (get_phase_progress() % 2 == 0)
				launch_spread(BULLET_BALL_16x16, s.x, s.y, 8, 3, M_PI / 2., M_PI / 2.);

			break;

		/* CURVED LASER
			double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
			launch_ring_spiral(BULLET_BALL_16x16, x, y, 5, 2.5,
					0, 0.01);
		*/
		default:
			break;
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
		
		break;
	// universal things in every stage
	default:
		break;
	}
}


