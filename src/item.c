/*
 * item.c contains functions for items.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */
 
/* INCLUDES */
#include "item.h"
#include "logic.h"
#include "random.h"
#include "pointer.h"

/* GLOBALS */

struct item powerup;
struct item score;

/* FUNCTIONS */

// initialize itemlist with itemIDs
void initialize_items()
{	
	int i, j;
	
	// small powerup item
	powerup.id = ITEM_POWERUP;
	powerup.active = false;
	powerup.motion.hb.r = powerup.sdl.rect.w;
	powerup.motion.v_max = 2.5;
	powerup.value = 0.05;
	itemlist[powerup.id][0] = powerup;
	
	// small score item
	score.id = ITEM_SCORE;
	score.active = false;
	score.motion.hb.r = score.sdl.rect.w;
	score.value = 1;
	score.motion.v_max = 2.5;
	itemlist[score.id][0] = score;
	
	// create multiple of all unique items
	for (i = 0; i < MAX_ITEMS; i++) {
		for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
			itemlist[i][j] = itemlist[i][0];
		}
		nth_item[i] = 0; // sets smart pointer
	}
}

// sets velocities of a single item and spawn it at given position
// *item: item structure by reference
// xcoord: x-position of the spawner
// ycoord: y-position of the spawner
int create_item(struct item *item, int xcoord, int ycoord)
{
	// set initial and current positions
	set_position(&(*item).motion.o, xcoord, ycoord);
	set_position(&(*item).motion.hb.p, xcoord, ycoord);
	
	// set velocity
	double speed = 2.;
	set_velocity(&(*item).motion.v, 0, -speed);
	
	// item accelerates downwards
	set_acceleration(&(*item).motion.a, 0, 0.05);
	
	
	// activate the item
	item -> active = true;
}

// spawns a pile of items at enemy & returns item smart pointer
// entity: the enemy from which the items spawn
// multiple: how much of the item is spawned
int create_item_pile(struct enemy entity)
{
	int i;
	
	// x-axis interval for items to	
	int left = entity.motion.hb.p.x - entity.motion.hb.r;
	int right = entity.motion.hb.p.x + entity.motion.hb.r;
	
	// loop for each item multiple
	for (i = 0; i < entity.item_multiple; i++) {
		// cycle through smart pointer
		cycle_item_smart_pointer(entity.item);
		// spawn item
		create_item(&itemlist[entity.item][nth_item[entity.item]],
				randint(left, right),
				entity.motion.hb.p.y);
	}
}

// update all positional values of all active items
void update_item_motion()
{
	int i, j;
	// item falls
	for (i = 0; i < MAX_ITEMS; i++) {
	for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
		if (itemlist[i][j].active) {
			// updates all values in struct
			update_motion(&itemlist[i][j].motion);
		}
	}
	}
}

// complete template for items
void do_item()
{
	update_item_motion();
}
