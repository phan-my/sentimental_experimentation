/*
 * common.c includes misc functions not classified in any other include file.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "input.h"
#include "logic.h"
#include "common.h"
#include "render.h"
#include "menu.h"

/* VARIABLES */

// space between player bullet in ticks

int reload = MAX_RELOAD;
int nth_player_bullet = 0;
double factored_speed;
double diagonal;
int scanned_key;
bool key_down = 0;


/* FUNCTIONS */

int do_main_menu_input()
{
	SDL_Event event;
	const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event)) {
		switch (event.type) {

		// close button
		case SDL_QUIT:
			return 1;
			break;

		// any key pressed
		case SDL_KEYDOWN:
			key_down = 1;
			scanned_key = event.key.keysym.scancode;
			break;
		case SDL_KEYUP:
// 				key_down = 0;
// 				scanned_key = 0;
			break;
		default:
			break;
		}
	}
	
	// start game
	if (key_down) {
		if (keyboard_states[SDL_SCANCODE_Z])
			state_menu = STATE_LOADING;
	}
}

// update player 
void do_player_input()
{
	// update graze position
	player[curr_player].grazebox.p.x = player[curr_player].core.p.x - player[curr_player].grazebox.r / 2.;
	player[curr_player].grazebox.p.y = player[curr_player].core.p.y - player[curr_player].grazebox.r / 2.;
	
	// update item collection position
	player[curr_player].itembox.p.x = player[curr_player].core.p.x - player[curr_player].itembox.r / 2.;
	player[curr_player].itembox.p.y = player[curr_player].core.p.y - player[curr_player].itembox.r / 2.;
}

// returns 1 to exit the main loop
int do_input()
{
	// questions/1252976
	// github: mikeanthonywild/sdl-shoot-em-up
	SDL_Event event;
	const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);

	// mechanism for player to grind at field border
	bool in_left	= player[curr_player].sdl.rect.x > FIELD_OFFSET_X;
	bool in_up = player[curr_player].sdl.rect.y > FIELD_OFFSET_Y;
	bool in_down = player[curr_player].sdl.rect.y < FIELD_OFFSET_Y + FIELD_HEIGHT
		- player[curr_player].sdl.rect.h + PLAYER_BOTTOM_MARGIN;
	bool in_right = player[curr_player].sdl.rect.x < FIELD_OFFSET_X + FIELD_WIDTH
		- player[curr_player].sdl.rect.w;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {

		// close button
		case SDL_QUIT:
			return 1;
			break;

		// any key pressed
		case SDL_KEYDOWN:
			key_down = 1;
			scanned_key = event.key.keysym.scancode;
			break;
		case SDL_KEYUP:
// 				key_down = 0;
// 				scanned_key = 0;
			break;
		default:
			break;
		}
	}

	// avoids key repeat delay
	// questions/21311824/sdl2-key-repeat-delay
	if (key_down) {
		// keyboard API
		
		// escape key
		if (keyboard_states[SDL_SCANCODE_ESCAPE])
			return 1;


		/* movement */

		if (keyboard_states[SDL_SCANCODE_LSHIFT])
			factored_speed =
				REIMU_FOCUS_FACTOR * REIMU_DEFAULT_SPEED;
		else
			factored_speed = REIMU_DEFAULT_SPEED;
		diagonal = factored_speed * (INVERSE_SQRT_2 - 1);

		// non-diagonal movement
		if (keyboard_states[SDL_SCANCODE_LEFT] && in_left)
			player[curr_player].core.p.x -= factored_speed;
		if (keyboard_states[SDL_SCANCODE_DOWN] && in_down)
			player[curr_player].core.p.y += factored_speed;
		if (keyboard_states[SDL_SCANCODE_UP] && in_up)
			player[curr_player].core.p.y -= factored_speed;
		if (keyboard_states[SDL_SCANCODE_RIGHT] && in_right)
			player[curr_player].core.p.x += factored_speed;

		// diagonal movement
		if (keyboard_states[SDL_SCANCODE_LEFT] &&
				keyboard_states[SDL_SCANCODE_DOWN]) {
			if (in_down)
				player[curr_player].core.p.y += diagonal;
			if (in_left)
				player[curr_player].core.p.x -= diagonal;
		}
		if (keyboard_states[SDL_SCANCODE_LEFT] &&
				keyboard_states[SDL_SCANCODE_UP]) {
			if (in_left)
				player[curr_player].core.p.x -= diagonal;
			if (in_up)
				player[curr_player].core.p.y -= diagonal;
		}
		if (keyboard_states[SDL_SCANCODE_RIGHT] &&
				keyboard_states[SDL_SCANCODE_DOWN]) {
			if (in_right)
				player[curr_player].core.p.x += diagonal;
			if (in_down)
				player[curr_player].core.p.y += diagonal;
		}
		if (keyboard_states[SDL_SCANCODE_RIGHT] &&
				keyboard_states[SDL_SCANCODE_UP]) {
			if (in_right)
				player[curr_player].core.p.x += diagonal;
			if (in_up)
				player[curr_player].core.p.y -= diagonal;
		}

//			player[curr_player].sdl.rect.x = player[curr_player].core.p.x;
//			player[curr_player].sdl.rect.y = player[curr_player].core.p.y;
		do_player_input();

		
		/* shooting */

		// https://www.parallelrealities.co.uk/tutorials/shooter/shooter5.php
		if (keyboard_states[SDL_SCANCODE_Z]) {
			// activate one bullet
			if (reload == 0)
				player_bullets[nth_player_bullet].active = 1;
			// cycles through array
			while (player_bullets[nth_player_bullet].active) {
				nth_player_bullet++;
				nth_player_bullet %= MAX_PLAYER_BULLETS;
			}
		}
		reload--;
		if (reload < 0)
			reload = MAX_RELOAD;
	}
	return 0;
}

