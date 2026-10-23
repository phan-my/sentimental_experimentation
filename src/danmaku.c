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


// returns the measure angle between two points in radians
// x_1: x-position of the first point
// y_1: y-position of the first point
// x_2: x-position of the second point
// y_2: y-position of the second point
double measure_angle(double x_1, double y_1, double x_2, double y_2)
{
	// calculate the difference between point 2 and point 1
	return atan2(y_2 - y_1, x_2 - x_1);
}

// returns the measure angle between a point and the position of the player
// xcoord: x-position of the point
// ycoord: y-position of the point
double angle_to_player(double xcoord, double ycoord)
{
	return measure_angle(xcoord, ycoord,
			player[curr_player].motion.hb.p.x,
			player[curr_player].motion.hb.p.y);
}

// sets velocities of a single bullet based on given angle for constant motion
// *bullet: passing struct bullet by reference
// speed: scalar speed of the bullet in px/fr
// radians: measure angle between positive x-axis and direction of velocity
void launch_straight(struct bullet *bullet, double speed, double radians)
{
	// set bullet speed, with individual v_x and v_y vectors
	set_velocity(&(*bullet).motion.v, speed * cos(radians),
			speed * sin(radians));
	// constant motion, no acceleration
	set_acceleration(&(*bullet).motion.a, 0, 0);
	
	// activate bullet
	bullet -> active = true;
}

// sets velocities of stack of n bullets based on given angle and positon
void launch_stack(int type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed, double angle)
{
	int i = 0;
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
		launch_straight(&bulletlist[type][nth_bullet[type]],
				min_speed + i * delta_speed, angle);

		// cycle smart pointer to next inactive bullet
		cycle_bullet_smart_pointer(type);
		i++; // loop
	}
}

// 
void launch_aimed_stack(int type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed)
{
	launch_stack(type, xcoord, ycoord, n,
			min_speed, max_speed, angle_to_player(xcoord, ycoord));

}


// creates an n-ring of bullets at specified spawner with angle and speed
// returns value of smart pointer nth_bullet
// (*bullets)[MAX_BULLET_MULTIPLE]: the array of bullets parsed by reference
// nth_bullet: smart pointer for the bullet array
// xcoord: origin position on x-axis of the ring group
// ycoord: origin position on y-axis of the ring group
// n: number of bullets in the ring
// speed: scalar speed of each bullet in px/fr
// angle: the radians clockwise from the x-axis that the 0th bullet aims at
// TODO: use **bullets instead of (*bullets)[MAX_BULLET_MULTIPLE]
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
		launch_straight(&bulletlist[type][nth_bullet[type]], speed,
				angle + delta_angle * i);
	}
}

// creates rounds of rings with angular acceleration, similar to BoWaP
void launch_ring_spiral(int type, double xcoord, double ycoord, int n, double speed,
		double angle, double angular_acceleration)
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

		// set angular values
		bulletlist[type][nth_bullet[type]].motion.angle_rate = 0;
		bulletlist[type][nth_bullet[type]].motion.angle_acceleration = angular_acceleration;

		// launch each individual bullet
		launch_straight(&bulletlist[type][nth_bullet[type]], speed,
				angle + delta_angle * i);
	}
}

// creates an n-ring of bullets at specified spawner with speed, seed bullet
// aims at the player
// returns value of smart pointer nth_bullet
// (*bullets)[MAX_BULLET_MULTIPLE]: the array of bullet to use
// nth_bullet: smart pointer for the bullet array
// xcoord: origin position on x-axis of the ring group
// ycoord: origin position on y-axis of the ring group
// n: number of bullets in the ring
// speed: scalar speed of each bullet in px/fr
void launch_aimed_ring(int type, double xcoord, double ycoord, int n,
		double speed)
{
	launch_ring(type, xcoord, ycoord, n, speed,
			angle_to_player(xcoord, ycoord));
}

// creates an n-spread of bullets at specified spawner with angle and speed
// returns value of smart pointer nth_bullet
// (*bullets)[MAX_BULLET_MULTIPLE]: the array of bullets parsed by reference
// nth_bullet: smart pointer for the bullet array
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// n > 1: number of bullets in spread
// speed: scalar speed of each bullet in px/fr
// seed_angle: radians clockwise from the x-axis that spread center targets
// arc: arc angle in radians of spread; 
// TODO: use **bullets instead of (*bullets)[MAX_BULLET_MULTIPLE]
void launch_spread(int type, double xcoord, double ycoord, int n, double speed,
		double seed_angle, double arc)
{
	int i;
	double start_angle = seed_angle - arc / 2.; // "earliest" bullet clockw.
	double delta_angle;
	
	// n == 1 as parameter not allowed due to division by zero; foolproof
	if (n == 1) {
		delta_angle = 0.;
	}

	delta_angle = arc / (n - 1); // angle difference between bullets
	
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
		launch_straight(&bulletlist[type][nth_bullet[type]], speed,
				start_angle + delta_angle * i);
	}
}

// creates an n-spread at spec1ified spawner with speed, seed bullet
// returns value of smart pointer nth_bullet
// (*bullets)[MAX_BULLET_MULTIPLE]: the array of bullets parsed by reference
// nth_bullet: smart pointer for the bullet array
// xcoord: origin position on x-axis of the spread group
// ycoord: origin position on y-axis of the spread group
// n > 1: number of bullets in spread
// speed: scalar speed of each bullet in px/fr
// arc: arc angle in radians of spread; 
// TODO: use **bullets instead of (*bullets)[MAX_BULLET_MULTIPLE]
void launch_aimed_spread(int type, double xcoord, double ycoord, int n,
		double speed, double arc)
{
	launch_spread(type, xcoord, ycoord, n, speed,
			angle_to_player(xcoord, ycoord), arc);
}

// creates a 


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
