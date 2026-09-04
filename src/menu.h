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
#define STATE_PLAY	0
#define STATE_MAIN	1
#define STATE_LOADING	2
#define STATE_PAUSE	3
#define STATE_ENDING	4

// globals
extern int state_menu;

#endif /* MENU_H */
