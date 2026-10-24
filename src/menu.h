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

// macros
#define STATE_ON_POWER	0
#define STATE_PLAY	1
#define STATE_MAIN_MENU	2
#define STATE_LOADING	3
#define STATE_PAUSE	4
#define STATE_ENDING	5

// globals
extern int state_menu;

// functions
void do_loading();

#endif /* MENU_H */
