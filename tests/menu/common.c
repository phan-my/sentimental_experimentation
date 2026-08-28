/*
 * common.c includes misc functions not classified in any other include file.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


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

