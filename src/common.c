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

// simple moving average (SMA)
double rolling_average(double *points, int num_elements)
{
	int k = 60;
	double sum;
	int i;

	for (i = num_elements - k + 1; i <= num_elements; i++) 
		sum += points[i];

	return (1. / k) * sum;
}

// returns 
double log_256(double x)
{
	return log(x)/log(256);
}

// cycles smart pointer through array until an inactive element is found
// pointer: the smart pointer as an integer value (nth element in array)
// array: array that pointer points to
// max: the maximum number of elements in the arrays
int cycle_bullet_smart_pointer(int pointer, struct bullet (*array)[MAX_BULLET_MULTIPLE], int max)
{
	int i;
	while ((*array)[pointer].active) {
		pointer++;
		pointer %= max;
		
		// failsafe: if all elements are loaded, then break
		i++;
		if (i > max)
			break;	
	}

	return pointer;
}

// cycles smart pointer through array until an inactive element is found
// pointer: the smart pointer as an integer value (nth element in array)
// array: array that pointer points to
// max: the maximum number of elements in the arrays
int cycle_item_smart_pointer(int pointer, struct item *array, int max)
{
	int i;
	while (array[pointer].active) {
		pointer++;
		pointer %= max;
	}
	return pointer;
}

