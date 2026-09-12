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

// returns random integer in range
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
