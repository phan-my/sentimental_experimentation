#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>

// trailing significant field = mantissa
#define TSF_DOUBLE 52

#define BIASED_EXPONENT_DOUBLE 11
#define BE_DOUBLE BIASED_EXPONENT_DOUBLE

#define EMAX_DOUBLE 1023

double log_256(double x)
{
	return log(x)/log(256);
}

int randint(int min, int max)
{
	int difference = max - min;
	int i;
	int p = (int)floor(log_256(difference)) + 1;
	int r;
	unsigned char Buffer[p];   
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

