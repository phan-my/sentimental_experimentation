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

/* START EDITING HERE */

// PHASES START AT 1
// double stages_s[MAX_STAGES][MAX_CHECKPOINTS] = {{1, 4.5, 42, 70.5}};
int stages[MAX_STAGES][MAX_CHECKPOINTS] = {
	{4, 1*60, 10*60, 20*60, 30*60}, // STAGE 1 (0th array)
	{4, 0*60, 1*60, 2*60, 3*60}  // STAGE 2
};
int current_stage = 0; // STARTS AT 0

// boss timing : number of spells of boss
bool is_boss = 0;
int boss_phases[MAX_STAGES] = {
	2 // number of spells or nonspells of the stage boss
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



/* main loop */

// plays through 1st stage (0th stage in array)
void do_1st_stage()
{
	// stage proper
	switch (get_phase()) {
	case 1:

		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
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


