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

// cycles smart pointer through array until an inactive bullet is found
// returns the smart pointer value that points at the closest inactive bullet
// pointer: the smart pointer as an integer value (nth element in array)
// array: array that pointer points to
int cycle_bullet_smart_pointer(int pointer,
		struct bullet (*array)[MAX_BULLET_MULTIPLE])
{
	int count = 0;

	// main cycle
	while ((*array)[pointer].active) {
		// cycles through array
		pointer++; // increment pointer
		pointer %= MAX_BULLET_MULTIPLE; // if pointer > max, then modulo
		
		// failsafe: if all elements are loaded, then break
		count++;
		if (count > MAX_BULLET_MULTIPLE) {
			printf("BULLETLIST OVERFLOW, UNLOADING SOME BULLETS\n");
			(*array)[pointer].active = false; // unload bullet
			break;
		}
	}
	return pointer;
}

// cycles smart pointer through array until an inactive item is found
// returns the smart pointer value that points at the closest inactive item
// pointer: the smart pointer as an integer value (nth element in array)
// array: array that pointer points to
int cycle_item_smart_pointer(int pointer,
		struct item (*array)[MAX_ITEM_MULTIPLE])
{
	int count = 0;
	while ((*array)[pointer].active) {
		pointer++;
		pointer %= MAX_ITEM_MULTIPLE;

		// failsafe: if all elements are loaded, then break
		count++;
		if (count > MAX_BULLET_MULTIPLE) {
			printf("ITEM LIST OVERFLOW, UNLOADING SOME BULLETS\n");
			(*array)[pointer].active = false; // unload bullet
			break;
		}
	}
	return pointer;
}

