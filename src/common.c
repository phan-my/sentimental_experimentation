/*
 * common.c includes misc functions not classified in any other include file.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include <math.h>
#include <stdbool.h>
#include "logic.h"

/* FUNCTIONS */

// returns the simple moving average with in an array
// *points: array of recorded double floats
// num_elements: number of elements in *points
double rolling_average(double *points, int num_elements)
{
	int k = 60;
	double sum;
	int i;

	// calculates sum in range
	for (i = num_elements - k + 1; i <= num_elements; i++) 
		sum += points[i];

	// returns average of sum
	return (1. / k) * sum;
}

// returns the base-256 logarithm of a number
// x: anti-logarithm base 256
double log_256(double x)
{
	return log(x)/log(256);
}

// https://docs.godotengine.org/en/latest/tutorials/math/interpolation.html

// computes linear interpolation (in one dimension) with given values
// a: 1st value for lerp
// b: 2nd value for lerp
// t: value in [0, 1] that represents the states in-between
double lerp(double a, double b, double t)
{
	// simplified formula for t in [0, 1]
	return a + t * (b - a);
}

// computes lerp coordinates on plane out of two given sets of coordinates
// x_0: x-value of first coordinate
// y_0: y-value of first coordinate
// x_1: x-value of second coordinate
// y_1: y-value of second coordinate
// t: value in [0, 1] that represents the states in-between
struct position lerp2(double x_0, double y_0, double x_1, double y_1, double t)
{
	struct position p;
	
	// lerps individual positional vectors
	p.x = lerp(x_0, x_1, t);
	p.y = lerp(y_0, y_1, t);
	
	return p;
}

// alternative version of lerp2 that uses struct position parameters
// computes lerp coordinates on plane out of two given sets of points
// p_0: first position
// p_1: second position
// t: value in [0, 1] that represents the states in-between
struct position lerp_p(struct position p_0, struct position p_1, double t)
{
	struct position p;
	
	// lerps individual positional vectors
	p.x = lerp(p_0.x, p_1.x, t);
	p.y = lerp(p_0.y, p_1.y, t);
	
	return p;
}

// returns the position of the Bezier curve at variable t
// p_0: first position
// p_1: second position
struct position cubic_bezier(struct position p_0, struct position p_1,
		struct position p_2, struct position p_3, double t)
{
	// https://docs.godotengine.org/en/latest/tutorials/math/beziers_and_curves.html
	struct position q0 = lerp_p(p_0, p_1, t);
	struct position q1 = lerp_p(p_1, p_2, t);
	struct position q2 = lerp_p(p_2, p_3, t);

	struct position r0 = lerp_p(q0, q1, t);
	struct position r1 = lerp_p(q1, q2, t);

	struct position s = lerp_p(r0, r1, t);
	return s;
}
