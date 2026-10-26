/*
 * item.h is the headerfile for item.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */
 
#ifndef ITEM_H
#define ITEM_H

// includes
#pragma once
#include "logic.h"
#include "enemy.h"

// macros


// globals
extern struct item powerup;
extern struct item score;

// prevents warning: ‘struct enemy’ declared inside parameter list will not be
// 	visible outside of this definition or declaration
struct enemy;

// functions
void initialize_items();
int create_item(struct item *item, int xcoord, int ycoord);
int create_item_pile(struct enemy entity);
void do_item();

#endif /* ITEM_H */
