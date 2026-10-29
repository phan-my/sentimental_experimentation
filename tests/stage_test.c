/*
 * stage_test.c includes example patterns and stage positioning elements.
 * Replace /src/stage.c with the contents of this file and /src/stage.h with the
 * contents of stage_test.h to run the custom stage.
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

/* START EDITING HERE */

// PHASES START AT 1
// double stages_s[MAX_STAGES][MAX_CHECKPOINTS] = {{1, 4.5, 42, 70.5}};
int stages[MAX_STAGES][MAX_CHECKPOINTS] = {
	{6, 1*60, 10*60, 20*60, 30*60, 40*60, 50*60, 60*60}, // STAGE 1 (0th array)
	{4, 0*60, 1*60, 2*60, 3*60}  // STAGE 2
};
int current_stage = 0; // STARTS AT 0

// boss timing : number of spells of boss
bool is_boss = 0;
int boss_phases[MAX_STAGES] = {
	2
};
int curr_boss_phase;

// midboss
// designate the phase
int midboss_phases[MAX_STAGES] = {
	3
};

/* HALT EDITING HERE */

// timing
struct timespec stage_start;
struct timespec stage_now;
uint64_t stage_progress;
int stage_progress_frames;
double stage_progress_seconds;

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
	clock_gettime(CLOCK_MONOTONIC_RAW, &dt_start);
	is_boss = false;
}

// returns the nth cycle of a bullet curtain since the start of the phase
// delay: delay between each curtain fire in frames
double get_cycle(int delay)
{
	return ((double)get_phase_progress() / (double)delay);
}

/* PATTERNS */

/* START EDITING HERE */

// 17-spread (aimed) of ball_8x8
void pattern_spread_test()
{
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
	if (get_phase_progress() % 20 == 0) {	
		launch_aimed_spread(BULLET_BALL_8x8, x, y, 17, 2.4,
				M_PI / 2.);
	}
}

// testing two fairy columns
void pattern_two_fairy_columns()
{
	int delay = 40;
	int i, j;
	
	double x = FIELD_OFFSET_X + MIDDLE_X;
	double y = FIELD_OFFSET_Y;
	
	// firing rate: spawn fairies continuously only with breaks of the interval `delay`
	if (get_phase_progress() % delay == 0) {
		// condition to fire exactly 12 enemies
		if (get_cycle(delay) <= 24) {
			enemy_launch_straight(ENEMY_FAIRY_BLUE, x + 200, y, 2., M_PI / 2. + M_PI / 3.);
			enemy_launch_straight(ENEMY_FAIRY_RED, x - 200, y, 2., M_PI / 2. - M_PI / 3.);
		}
	}
	
	// red fairies fire 8-rings
	delay = 120;
	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		if (enemylist[i][j].active &&
				enemylist[i][j].id == ENEMY_FAIRY_RED) {
			if ((stage_progress_frames - enemylist[i][j].motion.time_0) % delay == 0) {
				launch_aimed_ring(BULLET_BALL_8x8,
						enemylist[i][j].motion.hb.p.x,
						enemylist[i][j].motion.hb.p.y, 16, 2.4);
			}
		}
	}
	}
	
	// blue fairies fire stacks
	delay = 120;
	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		if (enemylist[i][j].active &&
				enemylist[i][j].id == ENEMY_FAIRY_BLUE) {
			if ((stage_progress_frames - enemylist[i][j].motion.time_0) % delay == 0) {
				launch_aimed_stack(BULLET_BALL_8x8,
						enemylist[i][j].motion.hb.p.x,
						enemylist[i][j].motion.hb.p.y, 4, 2.4, 3);
			}
		}
	}
	}
}

// three aimed rings of bubbles
void pattern_ring_test()
{
	double x = FIELD_MIDDLE_X;
	double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
	if (get_phase_progress() % 20 == 0) {
		launch_aimed_ring(BULLET_BUBBLE, x, y, 8, 2.4);
		launch_aimed_ring(BULLET_BUBBLE, x+60, y, 7, 3.);
		launch_aimed_ring(BULLET_BUBBLE, x-60, y, 7, 3.);
	}
}

// one fixed and one aimed stack
void pattern_stack_test()
{
	int delay = 120;
	double x = FIELD_MIDDLE_X;
	double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
	
	if (get_phase_progress() % delay == 0) {
		launch_stack(BULLET_BALL_8x8, x, y, 6, 2.4, 5., M_PI / 2.);
		launch_aimed_stack(BULLET_BALL_8x8, x, y, 6, 2.4, 5.);
	}
}

// testing moving spawner pattern with rings
void pattern_moving_spawner_test()
{
	int delay = 20;

	double x0 = 50;
	double y0 = 50;
	double x1 = 512;
	double y1 = 50;

	int duration = 60 * 5;
	double t = (double)(get_phase_progress() % duration) / (double) duration; 

//	printf("%d\n", phase_progress % duration);
	if (get_phase_progress() % delay == 0) {
		launch_aimed_ring(BULLET_BALL_8x8,
				lerp(x0, x1, t), lerp(y0, y1, t), 18, 2.4);
	}
}

// testing moving spawner that moves back and forth
// TODO: remove the global variable requirement
int global_i = 1;

void pattern_moving_spawner_test_2()
{
	int delay = 2;
		
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	double t = 0;
	
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

	if (get_phase_progress() % delay == 0) {
		if (t > 1)
			global_i = -1;
		if (t < 0)
			global_i = 1;
		t += global_i * 0.05 * get_cycle(delay);
		struct position s = cubic_bezier(p_0, p_1, p_2, p_3, t);
		launch_spread(BULLET_BALL_16x16, s.x, s.y, 8, 3, M_PI / 2., M_PI / 2.);
	}
}

// curved laser in flower pattern (no danmaku attack)
void pattern_curved_laster_test()
{
	int delay;
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
	launch_ring_spiral(BULLET_BALL_16x16, x, y, 5, 2.5,
			0, 0.01, get_cycle(delay));
}

// implementation of spell card in TH11 Stage 5 under ReimuA
void pattern_boundary_of_wave_and_particle()
{
	int delay = 2;
	
	// constants
	// values at https://youtu.be/cpz4mnXJYb0?t=318
      	double angle = 0.3 * M_PI;
	double angle_v = -0.078539816;
	double angle_a = 0.003926991;
	
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	
	
	if (get_phase_progress() % delay == 0) {
		launch_ring_spiral_accelerated(BULLET_BALL_8x8, x, y,
			5, 3, angle, angle_v, angle_a, get_cycle(delay));
	}
}

// Hong Meiling's spell card
void pattern_gorgeous_sweet_flower()
{
	int delay = 5;
	double x = FIELD_MIDDLE_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	
	double alpha = 0;
	double alpha_v = 0.14; // GOLDEN_RATIO * 2 * M_PI;
	
	if (get_phase_progress() % delay == 0) {
		alpha += alpha_v * get_cycle(delay);
		launch_ring(BULLET_BALL_8x8, x, y, 6, 2, alpha);
		launch_ring(BULLET_BALL_8x8, x, y, 6, 2, -(alpha + M_PI / 3.));
	}
}

// pattern using phyllotaxis theory
void pattern_sunflower()
{
	int delay = 1;
	double x = FIELD_MIDDLE_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	
	double alpha = 0;
	double alpha_v = GOLDEN_RATIO * 2 * M_PI;
	
	if (get_phase_progress() % delay == 0) {
		alpha += alpha_v * get_cycle(delay);
		launch_ring(BULLET_BALL_8x8, x, y, 1, 1.4, alpha);
//		launch_ring(BULLET_BALL_8x8, x, y, 6, 2, -(alpha + M_PI / 3.));
	}
}

// testing spiral function
void pattern_spiral_test()
{
	int delay = 2;
	double x = FIELD_MIDDLE_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	
	if (get_phase_progress() % delay == 0) {
		launch_spiral_accelerated(BULLET_BALL_8x8, x, y, 3,
			0, GOLDEN_RATIO * M_PI * 2, 0.1, get_cycle(delay));
	}
}

// titlepage of the matura, consisting of the sunflower pattern and BoWaP
void pattern_titlepage()
{
	int delay = 2;
	
	// constants
	// values at https://youtu.be/cpz4mnXJYb0?t=318
	double alpha_v = -0.078539816;
	double alpha_a = 0.003926991;
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	
      	// globals
	double alpha = M_PI * (0.3);
	
	if (get_phase_progress() % delay == 0) {
		// implementation for "global" variables by basing the 
		// current value (alpha_v) on phase_progress and firing rate
//		alpha_v += alpha_a * (double)phase_progress / delay;
		alpha += alpha_v * get_cycle(delay)
			+ alpha_a * .5 * pow(get_cycle(delay), 2);
		launch_ring(BULLET_BALL_16x16, x, y, 8, 3, alpha);
	}
}

// tests acceleration functionality
void pattern_acceleration_test()
{
	int delay = 2;
	
	// constants
	// values at https://youtu.be/cpz4mnXJYb0?t=318
	double alpha_v = -0.078539816;
	double alpha_a = 0.003926991;
	double x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
	double y = FIELD_HEIGHT * (0.4) + FIELD_OFFSET_Y;
	
      	// globals
	double alpha = M_PI * (0.3);
	
	if (get_phase_progress() % delay == 0) {
		// implementation for "global" variables by basing the 
		// current value (alpha_v) on phase_progress and firing rate
//		alpha_v += alpha_a * (double)phase_progress / delay;
		alpha += alpha_v * get_cycle(delay)
			+ alpha_a * .5 * pow(get_cycle(delay), 2);
		launch_ring_accelerated(BULLET_BALL_8x8, x, y, 5, 1, .01, 6, alpha);
	}
}

/* main loop */

// plays through 1st stage (0th stage in array)
void do_1st_stage()
{
	// stage proper
	switch (get_phase()) {
	case 1:
		pattern_two_fairy_columns();
		break;
	case 2:
		pattern_ring_test();
		break;
	case 3:
		pattern_sunflower();
		break;
	case 4:
		pattern_boundary_of_wave_and_particle();
		break;
	case 5:
		pattern_titlepage();
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	case 11:
		break;
	case -1:
		
	default:
		break;
	}
}

// invoked in the main loop
void do_stage()
{
	// if game state is at a menu/loading screen, initialize stage
	if (state_menu == STATE_LOADING) {
		initialize_stage();
	}
	

	// main timing of stage elements
	switch (current_stage) {
	case 0:
		do_1st_stage();
		break;
	// universal things in every stage
	default:
		break;
	}
	
	
	
	
	/* DO NOT EDIT BEYOND THIS LINE */
	
	/* timing */
	clock_gettime(CLOCK_MONOTONIC_RAW, &stage_now);
	clock_gettime(CLOCK_MONOTONIC_RAW, &dt_end);
	
	stage_progress = (stage_now.tv_sec - stage_start.tv_sec)
		* 1000000. + (stage_now.tv_nsec - stage_start.tv_nsec) 
		/ 1000.;
	dt = (dt_end.tv_sec - dt_start.tv_sec) * 1000000. 
		+ (dt_end.tv_nsec - dt_start.tv_nsec) / 1000.;
		
	frames[stage_progress_frames] = dt / 1000000.;
//	printf("%d\n", stage_progress_frames);
	stage_progress_seconds = stage_progress / 1000000.;
//	printf("%d time\n", stage_progress);

//	if (stage_progress_frames > 60 && stage_progress_frames % 60 == 0)
//		printf("fps = %lf\n", rolling_average(frames, stage_progress_frames));

	stage_progress_frames++;
}


