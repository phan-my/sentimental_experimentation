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

// invoke as dest = f(d_dest, speed, turns)
void launch_straight(struct bullet *bullet, double speed, double radians)
{
	// set bullet speed, with individual v_x and v_y vectors
	bullet -> motion.v.x = speed * cos(radians);
	bullet -> motion.v.y = speed * sin(radians);
	
	// activate bullet
	bullet -> active = true;
}

/*
// creates a ring at specified spawner
// *bullet: the array of bullet to use
// nth_bullet: smart pointer for the bullet array
// max: the maximum number of elements in the arrays
// spawner: origin position of the ring group
// n: bullets to launch
// speed: scalar speed of each bullet in pixels per second at 60 fps 
// angle: the radians counterclockwise from the +x-axis that the ring aims at
int launch_ring(struct bullet **bullets, int nth_bullet, int max,
		struct position spawner, int n, double speed, double angle)
{
	int i;
	double delta_angle = angle / n; // difference in angle between bullets

	for (i = 0; i < n; i++) {
		// cycle through next inactive element of bullet array
		nth_bullet = cycle_bullet_smart_pointer(nth_bullet, &bullets, max);

		// launch each bullet
		launch_straight(&bullets[i], speed, delta_angle * i);
		
		// set positions 
		bullets[i].motion.o.x = spawner.x;
		bullets[i].motion.o.y = spawner.y;
		bullets[i].motion.p.x = spawner.x;
		bullets[i].motion.p.y = spawner.y;
	}
	nth_bullet = cycle_bullet_smart_pointer(nth_bullet, &bullets, max);
	return nth_bullet;
}
*/

