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
void _do_player_input()
{
	// update graze position
	player[curr_player].grazebox.p.x =
		player[curr_player].core.p.x
		- player[curr_player].grazebox.r / 2.;
	player[curr_player].grazebox.p.y =
		player[curr_player].core.p.y
		- player[curr_player].grazebox.r / 2.;
	
	// update item collection position
	player[curr_player].itembox.p.x =
		player[curr_player].core.p.x
		- player[curr_player].itembox.r / 2.;
	player[curr_player].itembox.p.y =
		player[curr_player].core.p.y
		- player[curr_player].itembox.r / 2.;
}

// if holdign left ALT + F4
bool holding_quit(const Uint8 *states)
{
	return states[SDL_SCANCODE_LALT] && states[SDL_SCANCODE_F4];
}

// returns default speed if not holding shift or focus speed if holding shift
double set_player_nondiagonal_speed(const Uint8 *states)
{
	double factored_speed;
	// set focus speed
	if (states[SDL_SCANCODE_LSHIFT])
		return player[curr_player].focus;
	// set default speed
	else
		return player[curr_player].speed;
}

// sets all player motion vectors to zero
void stop_player_motion()
{
	// set velocity to zero
	player[curr_player].motion.v.x = 0;
	player[curr_player].motion.v.y = 0;

	// set acceleration to zero
	player[curr_player].motion.a.x = 0;
	player[curr_player].motion.a.y = 0;
}

// if exactly two arrow keys are held -> return true for diagonal movement
bool holding_diagonal(Uint8 *states)
{
	// create array for all 4 arrow keys
	Uint8 arrow_keys[4] = {
		states[SDL_SCANCODE_LEFT],
		states[SDL_SCANCODE_DOWN],
		states[SDL_SCANCODE_UP],
		states[SDL_SCANCODE_RIGHT]
	};

	// loop through array, count the number of keys held
	int i;
	int count = 0;
	for (i = 0; i < 4; i++) {
		if (arrow_keys[i])
			count++;
	}

	// return bool value if count equals two
	return count == 2 ? true : false;
}

// returns 1 to exit the main loop
int do_input()
{
	// questions/1252976
	// github: mikeanthonywild/sdl-shoot-em-up
	SDL_Event event;
	const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);

	// mechanism for player to grind at field border
	bool in_left =
		player[curr_player].itembox.p.x - player[curr_player].itembox.r
		> FIELD_OFFSET_X;
	bool in_up =
		player[curr_player].itembox.p.y - player[curr_player].itembox.r
		> FIELD_OFFSET_Y;
	bool in_down =
		player[curr_player].itembox.p.y + player[curr_player].itembox.r
		< FIELD_OFFSET_Y + FIELD_HEIGHT;
	bool in_right =
		player[curr_player].itembox.p.x + player[curr_player].itembox.r
		< FIELD_OFFSET_X + FIELD_WIDTH;

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
			stop_player_motion();
// 				key_down = 0;
// 				scanned_key = 0;
			break;
		default:
			break;
		}
	}

	// avoids key repeat delay
	// questions/21311824/sdl2-key-repeat-delay
	// quit game
	if (holding_quit(keyboard_states))
		return 1;

	/* movement */

	stop_player_motion();
	factored_speed = set_player_nondiagonal_speed(keyboard_states);
	diagonal = factored_speed * INVERSE_SQRT_2;

	// non-diagonal movement
	if (keyboard_states[SDL_SCANCODE_LEFT] && in_left)
		player[curr_player].motion.v.x = -factored_speed;
	if (keyboard_states[SDL_SCANCODE_DOWN] && in_down)
		player[curr_player].motion.v.y = factored_speed;
	if (keyboard_states[SDL_SCANCODE_UP] && in_up)
		player[curr_player].motion.v.y = -factored_speed;
	if (keyboard_states[SDL_SCANCODE_RIGHT] && in_right)
		player[curr_player].motion.v.x = factored_speed;

	// diagonal movement
	if (keyboard_states[SDL_SCANCODE_LEFT] &&
			keyboard_states[SDL_SCANCODE_DOWN]) {
		if (in_down)
			player[curr_player].motion.v.y = diagonal;
		if (in_left)
			player[curr_player].motion.v.x = -diagonal;
	}
	if (keyboard_states[SDL_SCANCODE_LEFT] &&
			keyboard_states[SDL_SCANCODE_UP]) {
		if (in_left)
			player[curr_player].motion.v.x = -diagonal;
		if (in_up)
			player[curr_player].motion.v.y = -diagonal;
	}
	if (keyboard_states[SDL_SCANCODE_RIGHT] &&
			keyboard_states[SDL_SCANCODE_DOWN]) {
		if (in_right)
			player[curr_player].motion.v.x = diagonal;
		if (in_down)
			player[curr_player].motion.v.y = diagonal;
	}
	if (keyboard_states[SDL_SCANCODE_RIGHT] &&
			keyboard_states[SDL_SCANCODE_UP]) {
		if (in_right)
			player[curr_player].motion.v.x = diagonal;
		if (in_up)
			player[curr_player].motion.v.y = -diagonal;
	}

//	player[curr_player].sdl.rect.x = player[curr_player].motion.p.x;
//	player[curr_player].sdl.rect.y = player[curr_player].motion.p.y;
	update_player_motion();

		
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
	return 0;
}

