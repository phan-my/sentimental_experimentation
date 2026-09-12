/*
 * random.h is the headerfile for random.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef RANDOM_H
#define RANDOM_H

#pragma once

// trailing significant field = mantissa
#define TSF_DOUBLE 52

#define BIASED_EXPONENT_DOUBLE 11
#define BE_DOUBLE BIASED_EXPONENT_DOUBLE

#define EMAX_DOUBLE 1023

// functions
int randint(int min, int max);

#endif /* RANDOM_H */
