/*
 * menu.c includes the widget system.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */
#include "menu.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string.h>
#include "logic.h"
#include "render.h"
#include "sounds.h"
#include "input.h"
#include "texts.h"


/* GLOBALS */
int state_menu = STATE_ON_POWER;
struct widget *active_widget; // pointer to currently selected widget
struct widget widget_head;
struct widget *widget_tail;

struct overlay main_menu;
struct overlay border; // UI
struct overlay loading;


/* FUNCTIONS */

// initializes linked list with widget_head and *widget_tail
void initialize_widgets(void)
{
	// https://www.parallelrealities.co.uk/tutorials/widgets/widgets1.php#introduction
	memset(&widget_head, 0, sizeof(struct widget));
	widget_tail = &widget_head;
}

//
struct widget *create_widget(char *name)
{
	// https://www.parallelrealities.co.uk/tutorials/widgets/widgets1.php#introduction
	struct widget *w;

	w = malloc(sizeof(struct widget));
	memset(w, 0, sizeof(struct widget));
	widget_tail -> next = w;
	w -> prev = widget_tail;
	widget_tail = w;

	strcpy(w -> name, name);

	return w;
}

// collection of all widget functionalities
void do_widgets(void)
{
	// https://www.parallelrealities.co.uk/tutorials/widgets/widgets1.php#introduction
	const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
	if (keyboard_states[SDL_SCANCODE_UP]) {
	printf("HERE\n");
		active_widget = active_widget -> prev;

		if (active_widget == &widget_head) {
			active_widget = widget_tail;
		}
	}

	if (keyboard_states[SDL_SCANCODE_DOWN]) {
		active_widget = active_widget -> next;

	printf("HERE\n");
		if (active_widget == NULL) {
			active_widget = widget_head.next;
		}
	}
}


void draw_widgets(void)
{
	// https://www.parallelrealities.co.uk/tutorials/widgets/widgets1.php#introduction
	struct widget *w;
	w -> text = text_main_menu_pointer;

	for (w = widget_head.next; w != NULL ; w = w -> next) {
		if (w == active_widget) {
			draw_text(w -> text, w -> x - 40, w -> y);
		}
		draw_text(w -> text, w -> text.x, w->y);
	}
	
}

// invokes all functions during the loading screen
void do_loading()
{
      	reset_player_position();
	
	// wait to prevent flashing lights
	SDL_Delay(500);

	// mixer
	do_sounds();
	

	state_menu = STATE_PLAY;
}

// opens pause menu during gameplay

void initDemo(void)
{
    struct widget *w;
    int x;

    x = 500;

    w = create_widget("start");
    w->x = x;
    w->y = 200;
    strcpy(w->label, "Start");

    active_widget = w;

    w = create_widget("load");
    w->x = x;
    w->y = 250;
    strcpy(w->label, "Load");

    w = create_widget("options");
    w->x = x;
    w->y = 300;
    strcpy(w->label, "Options");

    w = create_widget("credits");
    w->x = x;
    w->y = 350;
    strcpy(w->label, "Credits");

    w = create_widget("exit");
    w->x = x;
    w->y = 400;
    strcpy(w->label, "Exit");
}

//
void do_main_menu()
{
	
}

