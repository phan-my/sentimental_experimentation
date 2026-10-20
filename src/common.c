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

	for (i = num_elements - k + 1; i <= num_elements; i++) 
		sum += points[i];

	return (1. / k) * sum;
}

// returns the base-256 logarithm of a number
// x: anti-logarithm
double log_256(double x)
{
	return log(x)/log(256);
}

// computes linear interpolation (in one dimension) with given values
// a: 1st value for lerp
// b: 2nd value for lerp
// t: value in [0, 1] that represents the states in-between
double lerp(double a, double b, double t)
{
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
	p.x = lerp(x_0, x_1, t);
	p.y = lerp(y_0, y_1, t);
	return p;
}

