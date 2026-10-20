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
#define CHECKPOINTS_STAGE_1 4

// globals
extern int current_stage;
extern int stages[MAX_STAGES][MAX_CHECKPOINTS];
extern int stage_progress_frames;
// extern double speed[MAX_BULLET_MULTIPLE];
// extern double cap_speed;
// extern double angles[MAX_BULLET_MULTIPLE];
// extern int stopping_line;
// extern double fairy_speed;

// functions
void initialize_stage();
void do_stage();

#endif /* STAGE_H */
