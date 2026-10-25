/*
 * logic.h is the headerfile for logic.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

// stackoverflow questions/1653958/ddg#1653965
// "That prevents double declaration of any identifiers" 
#ifndef MENU_H
#define MENU_H

// includes
#include "texts.h"

// macros
#define STATE_ON_POWER	0
#define STATE_PLAY	1
#define STATE_MAIN_MENU	2
#define STATE_LOADING	3
#define STATE_PAUSE	4
#define STATE_ENDING	5

#define MAX_NAME_LENGTH 256

// structs

// https://www.parallelrealities.co.uk/tutorials/widgets/widgets1.php
struct widget {
	char name[MAX_NAME_LENGTH]; // unique name for widget
	
	struct text text;
	
	// coordinates for drawing
	int x;
	int y;
	
	// text that is displayed
	char label[MAX_NAME_LENGTH];
	
	// double-linked list pointers
	// eases back-and-forth movement within list
	struct widget *prev;
	struct widget *next;
	void (*action)(void);
	
};

// globals
extern int state_menu;
extern struct widget *active_widget;
extern struct widget widget_head;
extern struct widget *widget_tail;

// functions
void initialize_widgets(void);
void initDemo(void);
void draw_widgets(void);
void do_widgets(void);
void do_loading();

#endif /* MENU_H */
