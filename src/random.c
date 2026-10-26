/*
 * random.c has functions for RNG mechanics.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <math.h>

#include "random.h"
#include "common.h"

/* FUNCTIONS */

// returns random integer in given range
// min: minimum integer (inclusive)
// min: maximum integer (inclusive)
int randint(int min, int max)
{
	int difference = max - min;
	int i;
	int p = (int)floor(log_256(difference)) + 1;
	int r;
	unsigned char Buffer[p];   
	// slow algorithm but avoids modular bias
	do {
		r = 0;
		int fd = open("/dev/urandom", O_RDONLY);
		read(fd, Buffer, p);
		close(fd);
		for(i = 0; i < p; ++i)
			r += Buffer[i] * pow(256, i);
	} while(r > difference);
	return r + min;
}

// returns a pseudorandom double in range (0.0, 1.0)
double uniform()
{
	// generation by manipulating to IEEE 754 floating points standards
	int i;
	union udouble f;
	f.d = 0.0d;
	bool mantissa[TSF_DOUBLE];

	// note: this will result in range [1.0, 2.0)
	bool BIASED_EXPONENT[BE_DOUBLE] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	// add sign (trivial)
	f.u <<= 1;
	f.u += 0;

	// add biased exponent
	for (i = 0; i < BE_DOUBLE; i++) {
		f.u <<= 1;
		f.u += BIASED_EXPONENT[i];
	}

	// generate random mantissa
	for (i = 0; i < TSF_DOUBLE; i++) {
		f.u <<= 1;
		f.u += randint(0, 1);
	}

	// cut the 1 before binary point
	return f.d - 1.0;
}
