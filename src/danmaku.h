/*
 * danmaku.h is the headerfile for danmaku.h.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef DANMAKU_H
#define DANMAKU_H

// includes
#include "logic.h"

// macros
#define BULLET_BUBBLE 0
#define BULLET_BALL_16x16 2
#define BULLET_BALL_8x8 1

// globals
extern struct bullet ball_8x8;
extern struct bullet ball_16x16;
extern struct bullet bubble;

// functions
void initialize_bullets();
void do_bullet();
void delete_danmaku_in_circle(double xcoord, double ycoord,
		double radius);
void delete_danmaku_all();
void start_straight(struct bullet *bullet, double speed, double radians);
void start_accelerated(struct bullet *bullet, double speed, double acceleration,
		double max_speed, double radians);
void launch_stack(int bullet_type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed, double angle);
void launch_aimed_stack(int type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed);
void launch_ring(int type, double xcoord, double ycoord, int n, double speed,
		double angle);
void launch_ring_accelerated(int type, double xcoord, double ycoord, int n,
		double speed, double acceleration, double max_speed,
		double angle);
void launch_aimed_ring(int type, double xcoord, double ycoord, int n,
		double speed);
void launch_spread(int type, double xcoord, double ycoord, int n, double speed,
		double seed_angle, double arc);
void launch_aimed_spread(int type, double xcoord, double ycoord, int n,
		double speed, double arc);
void launch_spiral(int type, double xcoord, double ycoord, double speed,
		double angle, double angle_rate, double t);
void launch_spiral_accelerated(int type, double xcoord, double ycoord, double speed,
		double angle, double angle_rate, double angle_acceleration,
		double t);
void launch_ring_spiral(int type, double xcoord, double ycoord,
		int n, double speed, double angle, double angle_rate, double t);
void launch_ring_spiral_accelerated(int type, double xcoord, double ycoord,
		int n, double speed, double angle, double angle_rate,
		double angle_acceleration, double t);

#endif /* DANMAKU_H */
