/*
 * danmaku.h is the headerfile for danmaku.h.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef DANMAKU_H
#define DANMAKU_H
#include "logic.h"

void launch_straight(struct bullet *bullet, double speed, double radians);
void launch_stack(int bullet_type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed, double angle);
void launch_aimed_stack(int type, double xcoord, double ycoord, int n,
		double min_speed, double max_speed);
void launch_ring(int type, double xcoord, double ycoord, int n, double speed,
		double angle);
void launch_aimed_ring(int type, double xcoord, double ycoord, int n,
		double speed);
void launch_spread(int type, double xcoord, double ycoord, int n, double speed,
		double seed_angle, double arc);
void launch_aimed_spread(int type, double xcoord, double ycoord, int n,
		double speed, double arc);
void launch_ring_spiral(int type, double xcoord, double ycoord, int n, double speed,
		double angle, double angular_acceleration);

#endif /* DANMAKU_H */
