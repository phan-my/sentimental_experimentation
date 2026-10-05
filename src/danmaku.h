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
//int launch_ring(struct bullet *bullets, int nth_bullet, int max,
//		struct position spawner, int n, double speed, double angle);

#endif /* DANMAKU_H */
