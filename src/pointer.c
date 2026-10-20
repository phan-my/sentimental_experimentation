/*
 * pointer.c includes includes relevant functions for smart pointers.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include "pointer.h"
#include "logic.h"

/* FUNCTIONS */

// cycles smart pointer (global) through array until an inactive bullet is found
// returns the smart pointer value that points at the closest inactive bullet
// type: integer index for type of bullet, e.g. BULLET_BUBBLE
void cycle_bullet_smart_pointer(int type)
{
	int count = 0;

	// main cycle
	while (bulletlist[type][nth_bullet[type]].active) {
		// cycles through array
		nth_bullet[type]++; // increment pointer
		nth_bullet[type] %= MAX_BULLET_MULTIPLE; // if pointer > max, then modulo
		
		// failsafe: if all elements are loaded, then break
		count++;
		if (count > MAX_BULLET_MULTIPLE) {
			printf("BULLETLIST OVERFLOW, UNLOADING SOME BULLETS\n");
			bulletlist[type][nth_bullet[type]].active = false; // unload bullet
			break;
		}
	}
}

// cycles smart pointer (global) through array until an inactive shot is found
// returns the smart pointer value that points at the closest inactive shot
// type: integer index for type of shot, e.g. SHOT_REIMU_A
void cycle_shot_smart_pointer(int type)
{
	int count = 0;

	// main cycle
	while (shotlist[type][nth_shot[type]].active) {
		// cycles through array
		nth_shot[type]++; // increment pointer
		nth_shot[type] %= MAX_SHOT_MULTIPLE; // if pointer > max, then modulo
		
		// failsafe: if all elements are loaded, then break
		count++;
		if (count > MAX_SHOT_MULTIPLE) {
			printf("SHOTLIST OVERFLOW, UNLOADING SOME BULLETS\n");
			shotlist[type][nth_shot[type]].active = false; // unload shot
			break;
		}
	}
}

// cycles smart pointer through array until an inactive item is found
// returns the smart pointer value that points at the closest inactive item
// pointer: the smart pointer as an integer value (nth element in array)
// array: array that pointer points to
void cycle_item_smart_pointer(int type)
{
	int count = 0;

	// main cycle
	while (itemlist[type][nth_item[type]].active) {
		// cycles through array
		nth_item[type]++; // increment pointer
		nth_item[type] %= MAX_ITEM_MULTIPLE; // if pointer > max, then modulo
		
		// failsafe: if all elements are loaded, then break
		count++;
		if (count > MAX_ITEM_MULTIPLE) {
			printf("ITEMLIST OVERFLOW, UNLOADING SOME BULLETS\n");
			itemlist[type][nth_item[type]].active = false; // unload shot
			break;
		}
	}
}
