/*
 * stage.h is the headerfile for stage.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef STAGE_H
#define STAGE_H

// includes
#pragma once
#include "logic.h"

// macros
#define MAX_STAGES 7
#define MAX_CHECKPOINTS 256

// globals
extern int current_stage;
extern double moving;
extern double speed[MAX_BULLETS];
extern double cap_speed;
extern double angles[MAX_BULLETS];
extern int stopping_line;
extern double fairy_speed;

// functions
void initialize_stage(int stage);
void do_stage(int stage);

#endif /* STAGE_H */
