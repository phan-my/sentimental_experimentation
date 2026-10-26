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
#pragma once
#include "texts.h"
#include "logic.h"

// macros
#define STATE_ON_POWER	0
#define STATE_PLAY	1
#define STATE_MAIN_MENU	2
#define STATE_LOADING	3
#define STATE_PAUSE	4
#define STATE_ENDING	5

// globals
extern int state_menu;
extern struct widget *active_widget;
extern struct widget widget_head;
extern struct widget *widget_tail;
extern struct overlay main_menu;
extern struct overlay border; // UI
extern struct overlay loading;


// functions
void initialize_widgets(void);
void initDemo(void);
void draw_widgets(void);
void do_widgets(void);
void do_loading();

#endif /* MENU_H */
