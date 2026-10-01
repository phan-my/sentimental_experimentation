/*
 * danmaku.c contains all basic (sub)patterns for enemy bullets.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include <math.h>
#include "logic.h"

// invoke as dest = f(d_dest, speed, turns)
void launch_straight(struct kinematics *kinemat, double speed, double radians)
{
	kinemat -> v.x = speed * cos(radians);
	kinemat -> v.y = speed * sin(radians);
}
