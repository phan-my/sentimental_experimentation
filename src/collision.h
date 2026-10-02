/*
 * collision.h is the headerfile for collision.h.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef COLLISION_H
#define COLLISION_H

// includes 
#pragma once
#include "logic.h"

// functions
bool circle_in_circle(struct circlebox dest, struct circlebox src);
void do_collision();

#endif /* COLLISION_H */
