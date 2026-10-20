/*
 * common.h is the headerfile for common.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef COMMON_H
#define COMMON_H

#pragma once
#include "logic.h"

// macros
#define INVERSE_SQRT_2 0.7071067811865475244008444
#define GOLDEN_RATIO 0.6180339887498948482045868343656381177203091798057628621

// functions
double rolling_average(double *points, int num_elements);
double log_256(double x);
double lerp(double a, double b, double t);
struct position lerp2(double x_0, double y_0, double x_1, double y_1, double t);

#endif /* COMMON_H */
