/*
 * danmaku.c contains all basic (sub)patterns for enemy bullets.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include <math.h>
#include <stdlib.h>
#include "logic.h"
#include "common.h"
#include "danmaku.h"
#include "pointer.h"
#include "collision.h"
#include "stage.h"
#include "render.h"

/* GLOBALS */

struct bullet ball_8x8;
struct bullet ball_16x16;
struct bullet bubble;


/* FUNCTIONS */

/* initialization */

// initialize bulletlist with bulletIDs
void initialize_bullets()
{
	int i, j;
	// 8x8 pellet
	ball_8x8.id = BULLET_BALL_8x8;
	ball_8x8.motion.hb.r = 3.;
	bulletlist[ball_8x8.id][0] = ball_8x8;
	
	// 16x16 ball
	ball_16x16.id = BULLET_BALL_16x16;
	ball_16x16.motion.hb.r = 4.27;
	bulletlist[ball_16x16.id][0] = ball_16x16;
	
	// bubble
	bubble.id = BULLET_BUBBLE;
	bubble.motion.hb.r = 16.;
	bulletlist[bubble.id][0] = bubble;
	
	// create multiple of all unique bullets
	for (i = 0; i < MAX_BULLETS; i++) {
		// create multiples per bulletID
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
			bulletlist[i][j] = bulletlist[i][0];
			bulletlist[i][j].active = false;
			bulletlist[i][j].grazed = false;
		}
		
		// set smart pointers to 0
		nth_bullet[i] = 0;
	}
}

// unloads entities if they go off-screen (automatic)
void unload_offscreen_bullets()
{
	int i, j;

	for (i = 0; i < MAX_BULLETS; i++) {
	for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
		if (in_unload_zone(bulletlist[i][j].motion))
			bulletlist[i][j].active = false;
	}
	}
}

// update all positional values of all active bullets
void update_bullet_motion()
{
	int i, j;
	
	// update all enemy bullets
	for (i = 0; i < MAX_BULLETS; i++) {
	for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
		update_motion(&bulletlist[i][j].motion);
	}
	}

	// if bullet falls off-screen, unload
	unload_offscreen_bullets();
}

// includes all operations of bullets
void do_bullet()
{
	update_bullet_motion();
}

/* deletion functions */

// deletes all bullets in circle at given coordinates with radius
// xcoord: x-position of the point
// ycoord: y-position of the point
// radius: radius (in pixels) of the circular field to clear bullets 
void delete_danmaku_in_circle(double xcoord, double ycoord,
		double radius)
{
	int i, j;
	struct circlebox field;
	field.p.x = xcoord;
	field.p.y = ycoord;
	field.r = radius;

	for (i = 0; i < MAX_BULLETS; i++) {
	for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
		if (circle_in_circle(bulletlist[i][j].motion.hb, field))
			// deactivate bullets in circle
			bulletlist[i][j].active = false;
	}
	}
}

// deletes all bullets in the playing field
void delete_danmaku_all()
{
	int i, j;

	for (i = 0; i < MAX_BULLETS; i++) {
	for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
		// deactivate bullets in circle
		bulletlist[i][j].active = false;
	}
	}
}


/* angle functions */

// returns the angle measure between two points in radians
// x_1: x-position of the first point
// y_1: y-position of the first point
// x_2: x-position of the second point
// y_2: y-position of the second point
double measure_angle(double x_1, double y_1, double x_2, double y_2)
{
	// calculates the difference between point 2 and point 1
	return atan2(y_2 - y_1, x_2 - x_1);
}

// returns the measure angle between a point and the position of the player
// xcoord: x-position of the point
// ycoord: y-position of the point
double angle_to_player(double xcoord, double ycoord)
{
	// calculates the difference between given coord and player position
	return measure_angle(xcoord, ycoord,
			player[curr_player].motion.hb.p.x,
			player[curr_player].motion.hb.p.y);
}

/* bullet starters (no position specified) */

// sets velocities of a single bullet based on given angle for constant motion
// *bullet: passing struct bullet by reference
// speed: scalar speed of the bullet in px/fr
// radians: measure angle between positive x-axis and direction of velocity
void start_straight(struct bullet *bullet, double speed, double radians)
{
	// straight bullet: use appropriate operations
	bullet -> motion.curved = false;

	// set bullet speed, with individual v_x and v_y vectors
	set_velocity(&(*bullet).motion.v0, speed * cos(radians),
			speed * sin(radians));
	set_velocity(&(*bullet).motion.v, speed * cos(radians),
			speed * sin(radians));
	// constant motion, no acceleration
	set_acceleration(&(*bullet).motion.a0, 0, 0);
	set_acceleration(&(*bullet).motion.a, 0, 0);
	bullet -> motion.v_max = speed;
	
	// set initial time
	bullet -> motion.time_0 = stage_progress_frames;
	
	// activate bullet
	bullet -> grazed = false;
	bullet -> active = true;
}

// sets velocities of a single bullet based on given angle for constant motion
// *bullet: passing struct bullet by reference
// speed: scalar speed of the bullet in px/fr
// acceleration: scalar acceleration of the bullet in px/fr^2
// max_speed: maximum speed of bullet (regardless of acceleration)
// radians: measure angle between positive x-axis and direction of velocity
void start_accelerated(struct bullet *bullet, double speed, double acceleration,
		double max_speed, double radians)
{
	// straight bullet: use appropriate operations
	bullet -> motion.curved = false;
	
	// set bullet speed, with individual v_x and v_y vectors
	set_velocity(&(*bullet).motion.v0, speed * cos(radians),
			speed * sin(radians));
	// TODO: optimize by creating copy_velocity(b, a)
	set_velocity(&(*bullet).motion.v, speed * cos(radians),
			speed * sin(radians));
	// set constant acceleration, with individual a_x and a_y vectors
	set_acceleration(&(*bullet).motion.a0,
			acceleration * cos(radians),
			acceleration * sin(radians));
	set_acceleration(&(*bullet).motion.a, acceleration * cos(radians),
			acceleration * sin(radians));
	
	// set maximum speed that nullifies acceleration
	bullet -> motion.v_max = max_speed;
	
	// set initial time
	bullet -> motion.time_0 = stage_progress_frames;
	
	// activate bullet
	bullet -> active = true;
	bullet -> grazed = false;
}


/* simple groups */

// creates an n-stack based on specifications
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis
// ycoord: origin position on y-axis
// n > 1: number of bullets in stack
// min_speed: scalar speed of slowest bullet in px/fr
// max_speed: scalar speed of fastest bullet in px/fr
// angle: the radians clockwise from the x-axis that the stack aims at
void launch_stack(int type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed, double angle)
{
	int i = 0;
	
	// difference in speed for each member of the stack
	double delta_speed = (max_speed - min_speed) / (n - 1);

	// cycle smart pointer through bullet array
	cycle_bullet_smart_pointer(type);
	
	// repeat n times for n-bullet stack
	while (i < n) {
		// set spawner position of the bullets
		set_position(&bulletlist[type][nth_bullet[type]].motion.o,
				xcoord, ycoord);
		// set current position of bullet
		set_position(&bulletlist[type][nth_bullet[type]].motion.hb.p,
				xcoord, ycoord);

		// launch individual bullets in stack
		start_straight(&bulletlist[type][nth_bullet[type]],
				min_speed + i * delta_speed, angle);

		// cycle smart pointer to next inactive bullet
		cycle_bullet_smart_pointer(type);
		i++; // loop
	}
}

// creates an aimed n-stack based on specifications
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis
// ycoord: origin position on y-axis
// n: number of bullets in stack
// min_speed: scalar speed of slowest bullet in px/fr
// max_speed: scalar speed of fastest bullet in px/fr
void launch_aimed_stack(int type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed)
{
	// using angle_to_player() for aiming
	launch_stack(type, xcoord, ycoord, n,
			min_speed, max_speed, angle_to_player(xcoord, ycoord));
}

// creates an n-ring of bullets at specified spawner with angle and speed
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis
// ycoord: origin position on y-axis
// n: number of bullets in the ring
// speed: scalar speed of each bullet in px/fr
// angle: the radians clockwise from the x-axis that the 0th bullet aims at
void launch_ring(int type, double xcoord, double ycoord, int n, double speed,
		double angle)
{
	int i;
	double delta_angle = 2. * M_PI / n; // angle difference between bullets
	
	// begin loop to launch n bullets
	for (i = 0; i < n; i++) {
		// cycles pointer to next inactive element of bullet array
		cycle_bullet_smart_pointer(type);

		// set spawner position of the bullets
		set_position(&bulletlist[type][nth_bullet[type]].motion.o,
				xcoord, ycoord);
		// set current position of bullet
		set_position(&bulletlist[type][nth_bullet[type]].motion.hb.p,
				xcoord, ycoord);


		// launch each individual bullet
		start_straight(&bulletlist[type][nth_bullet[type]], speed,
				angle + delta_angle * i);
	}
}

// creates an n-ring of bullets at given spawner with angle, speed, and accel
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis
// ycoord: origin position on y-axis
// n: number of bullets in the ring
// speed: scalar speed of each bullet in px/fr
// acceleration: scalar acceleration of the bullet in px/fr^2
// max_speed: maximum speed of bullet (regardless of acceleration)
// angle: the radians clockwise from the x-axis that the 0th bullet aims at
void launch_ring_accelerated(int type, double xcoord, double ycoord, int n,
		double speed, double acceleration, double max_speed,
		double angle)
{
	int i;
	double delta_angle = 2. * M_PI / n; // angle difference between bullets
	
	// begin loop to launch n bullets
	for (i = 0; i < n; i++) {
		// cycles pointer to next inactive element of bullet array
		cycle_bullet_smart_pointer(type);

		// set spawner position of the bullets
		set_position(&bulletlist[type][nth_bullet[type]].motion.o,
				xcoord, ycoord);
		// set current position of bullet
		set_position(&bulletlist[type][nth_bullet[type]].motion.hb.p,
				xcoord, ycoord);


		// launch each individual bullet
		start_accelerated(&bulletlist[type][nth_bullet[type]], speed,
				acceleration, max_speed,
				angle + delta_angle * i);
	}
}

// creates an n-ring of bullets at specified spawner with speed, seed bullet
// 	aims at the player
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis
// ycoord: origin position on y-axis
// n: number of bullets in the ring
// speed: scalar speed of each bullet in px/fr
void launch_aimed_ring(int type, double xcoord, double ycoord, int n,
		double speed)
{
	// using angle_to_player() for aiming
	launch_ring(type, xcoord, ycoord, n, speed,
			angle_to_player(xcoord, ycoord));
}

// creates an n-spread of bullets at specified spawner with angle and speed
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// n > 1: number of bullets in spread
// speed: scalar speed of each bullet in px/fr
// seed_angle: radians clockwise from the x-axis that spread center targets
// arc: arc angle in radians of spread; 
void launch_spread(int type, double xcoord, double ycoord, int n, double speed,
		double seed_angle, double arc)
{
	int i;
	double start_angle = seed_angle - arc / 2.; // "earliest" bullet clockw.
	double delta_angle;
	
	// n == 1 as parameter not allowed due to division by zero; foolproof
	if (n == 1)
		delta_angle = 0.;
	// set angle difference between bullets
	else
		delta_angle = arc / (n - 1);
	
	// begin loop to launch n bullets
	for (i = 0; i < n; i++) {
		// cycles pointer to next inactive element of bullet array
		cycle_bullet_smart_pointer(type);

		// set spawner position of the bullets
		set_position(&bulletlist[type][nth_bullet[type]].motion.o,
				xcoord, ycoord);
		// set current position of bullet
		set_position(&bulletlist[type][nth_bullet[type]].motion.hb.p,
				xcoord, ycoord);

		// launch each individual bullet
		start_straight(&bulletlist[type][nth_bullet[type]], speed,
				start_angle + delta_angle * i);
	}
}

// creates an n-spread at spec1ified spawner with speed, seed bullet
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// n > 1: number of bullets in spread
// speed: scalar speed of each bullet in px/fr
// arc: arc angle in radians of spread; 
void launch_aimed_spread(int type, double xcoord, double ycoord, int n,
		double speed, double arc)
{
	// using angle_to_player() for aiming
	launch_spread(type, xcoord, ycoord, n, speed,
			angle_to_player(xcoord, ycoord), arc);
}

// creates a phyllotaxic pattern using a constant angular rate
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// speed: scalar speed of each bullet in px/fr
// angle: seed angle in radians clockwise from the x-axis to velocity vector
// angle_rate: angular change with every new bullet
// t: time elapsed since an imaginary t_0, usually invoked as get_cycle(delay)
void launch_spiral(int type, double xcoord, double ycoord, double speed,
		double angle, double angle_rate, double t)
{
	// sets current angle from given angular rate and time
	angle += angle_rate * t;
	// cycles pointer to next inactive element of bullet array
	cycle_bullet_smart_pointer(type);

	// set spawner position of the bullets
	set_position(&bulletlist[type][nth_bullet[type]].motion.o,
			xcoord, ycoord);
	// set current position of bullet
	set_position(&bulletlist[type][nth_bullet[type]].motion.hb.p,
			xcoord, ycoord);
	// launch individual bullet
	start_straight(&bulletlist[type][nth_bullet[type]], speed, angle);
}

// creates an accelerated phyllotaxic pattern using a constant angular accel
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// speed: scalar speed of each bullet in px/fr
// angle: seed angle in radians clockwise from the x-axis to velocity vector
// angle_rate: seed angular rate in rad/fr
// angle_acceleration: angular acceleration constant that derives current angle
// t: time elapsed since an imaginary t_0, usually invoked as get_cycle(delay)
void launch_spiral_accelerated(int type, double xcoord, double ycoord, double speed,
		double angle, double angle_rate, double angle_acceleration,
		double t)
{
	// sets current angle from given angular rate and time
	angle += angle_rate * t + .5 * angle_acceleration * pow(t, 2);
	// cycles pointer to next inactive element of bullet array
	cycle_bullet_smart_pointer(type);

	// set spawner position of the bullets
	set_position(&bulletlist[type][nth_bullet[type]].motion.o,
			xcoord, ycoord);
	// set current position of bullet
	set_position(&bulletlist[type][nth_bullet[type]].motion.hb.p,
			xcoord, ycoord);
	// launch individual bullet
	start_straight(&bulletlist[type][nth_bullet[type]], speed, angle);
}

// creates curtains of rings with constant angular rate
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// speed: scalar speed of each bullet in px/fr
// angle: seed angle in radians clockwise from the x-axis to velocity vector
// angle_rate: angular change with every new  ring
// t: time elapsed since an imaginary t_0, usually invoked as get_cycle(delay)
void launch_ring_spiral(int type, double xcoord, double ycoord,
		int n, double speed, double angle, double angle_rate, double t)
{
	// sets current angle from given angular rate and time
	angle += angle_rate * t;
	
	// using angle_to_player() for aiming
	launch_ring(type, xcoord, ycoord, n, speed, angle);
}

// creates curtains of rings with angular acceleration; generalization of BoWaP
// type: bullet type as specified in danmaku.h, e.g. BULLET_BUBBLE
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// speed: scalar speed of each bullet in px/fr
// angle: seed angle in radians clockwise from the x-axis to velocity vector
// angle_rate: seed angular rate in rad/fr
// angle_acceleration: angular acceleration constant that derives current angle
// t: time elapsed since an imaginary t_0, usually invoked as get_cycle(delay)
void launch_ring_spiral_accelerated(int type, double xcoord, double ycoord,
		int n, double speed, double angle, double angle_rate,
		double angle_acceleration, double t)
{
	// sets current angle from given angular rate and time
	angle += angle_rate * t + .5 * angle_acceleration * pow(t, 2);
	
	// using angle_to_player() for aiming
	launch_ring(type, xcoord, ycoord, n, speed, angle);
}



/* unused functions */
/*
		// "snowball" bullets
		for (i = 0; i < MAX_BULLET_MULTIPLE; i++) {
			bulletlist[BULLET_BALL_8x8][i].motion.hb.p.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
			bulletlist[BULLET_BALL_8x8][i].motion.hb.p.y = FIELD_HEIGHT * (1. / 4)
				+ FIELD_OFFSET_Y;
		}

*/

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
/*

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
*/
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

/*
		// Jellyfish
		// speed[i] = (double)i / 100;
		for (i = 0; i < moving; i++) {
			bulletlist[BULLET_BALL_8x8][i].active = true;
			bulletlist[BULLET_BALL_8x8][i].motion.v.x = speed[i] * cos(angles[i]);
			bulletlist[BULLET_BALL_8x8][i].motion.v.y = speed[i] * sin(angles[i]);
		}
		if (moving < MAX_BULLET_MULTIPLE) {
			moving += 3;
		}
*/
