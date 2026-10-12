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
int launch_ring(struct bullet (*bullets)[MAX_BULLET_MULTIPLE], int nth_bullet,
		double xcoord, double ycoord, int n, double speed,
		double angle);
int launch_aimed_ring(struct bullet (*bullets)[MAX_BULLET_MULTIPLE],
		int nth_bullet, double xcoord, double ycoord, int n,
		double speed);
int launch_spread(struct bullet (*bullets)[MAX_BULLET_MULTIPLE], int nth_bullet,
		double xcoord, double ycoord, int n, double speed,
		double seed_angle, double arc);

#endif /* DANMAKU_H */
