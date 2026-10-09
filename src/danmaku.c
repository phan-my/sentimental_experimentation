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

// passing struct bullet by reference
void launch_straight(struct bullet *bullet, double speed, double radians)
{
	// set bullet speed, with individual v_x and v_y vectors
	bullet -> motion.v.x = speed * cos(radians);
	bullet -> motion.v.y = speed * sin(radians);
	
	// activate bullet
	bullet -> active = true;
}

// creates an n-ring of bullets at specified spawner and speed
// returns value of smart pointer nth_bullet
// (*bullets)[MAX_BULLET_MULTIPLE]: the array of bullet to use
// nth_bullet: smart pointer for the bullet array
// xcoord: origin position on x-axis of the ring group
// ycoord: origin position on y-axis of the ring group
// n: number of bullets in the ring
// speed: scalar speed of each bullet in px/fr
// angle: the radians counterclockwise from the +x-axis that the ring aims at
// TODO: use **bullets instead of (*bullets)[MAX_BULLET_MULTIPLE]
int launch_ring(struct bullet (*bullets)[MAX_BULLET_MULTIPLE], int nth_bullet,
		double xcoord, double ycoord, int n, double speed, double angle)
{
	int i;
	double delta_angle = 2. * M_PI / n; // angle difference between bullets
	
	// begin loop to launch n bullets
	for (i = 0; i < n; i++) {
		// cycles pointer to next inactive element of bullet array
		nth_bullet = cycle_bullet_smart_pointer(nth_bullet,
				&(*bullets), MAX_BULLET_MULTIPLE);

		// set positions 
		(*bullets)[nth_bullet].motion.o.x = xcoord; // spawner
		(*bullets)[nth_bullet].motion.o.y = ycoord;

		(*bullets)[nth_bullet].motion.p.x = xcoord; // curr position
		(*bullets)[nth_bullet].motion.p.y = ycoord;


		// launch each individual bullet
		launch_straight(&(*bullets)[nth_bullet], speed,
				angle + delta_angle * i);
	}
	// returns value of the smart pointer
	return cycle_bullet_smart_pointer(nth_bullet, bullets,
			MAX_BULLET_MULTIPLE);
}

/* initialization */
/*
// "snowball" bullets
for (i = 0; i < MAX_BULLETS; i++) {
//		ball_8x8[i].w /= scale;
//		ball_8x8[i].h /= scale;
        ball_8x8[i].hitbox.p.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
        ball_8x8[i].hitbox.p.y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
        ball_8x8[i].hitbox.r = 3.8;
        update_ball_position(&ball_8x8[i]);
}
*/

/* BULLET MOVEMENTS */
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
