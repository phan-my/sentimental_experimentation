/*
 * input.c includes functions for handling user keyboard input.
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
#include "shot.h"
#include "pointer.h"
#include "danmaku.h"
#include "sounds.h"
#include "player.h"

/* GLOBALS */

int scanned_key;
bool key_down = 0;

/* FUNCTIONS */

// if holdign left ALT + F4
bool holding_quit(const Uint8 *states)
{
	return states[SDL_SCANCODE_LALT] && states[SDL_SCANCODE_F4];
}

// does input parsing for main menu widgets
// TODO: implement widget system
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
		// player selects Reimu
		if (keyboard_states[SDL_SCANCODE_1]) {
			curr_player = PLAYER_REIMU;
			state_menu = STATE_LOADING;
		}
		// player selects Marisa
		if (keyboard_states[SDL_SCANCODE_2]) {
			curr_player = PLAYER_MARISA;
			state_menu = STATE_LOADING;
		}
		if (holding_quit(keyboard_states))
			return 1;
	}
	
//	do_widgets();
}

// UNUSED: updates player hiboxes with input 
void _do_player_input()
{
	// update graze position
	player[curr_player].grazebox.p.x =
		player[curr_player].motion.hb.p.x
		- player[curr_player].grazebox.r / 2.;
	player[curr_player].grazebox.p.y =
		player[curr_player].motion.hb.p.y
		- player[curr_player].grazebox.r / 2.;
	
	// update item collection position
	player[curr_player].itembox.p.x =
		player[curr_player].motion.hb.p.x
		- player[curr_player].itembox.r / 2.;
	player[curr_player].itembox.p.y =
		player[curr_player].motion.hb.p.y
		- player[curr_player].itembox.r / 2.;
}

// returns default speed if not holding shift or focus speed if holding shift
// *states: keyboard states to parse
double set_player_nondiagonal_speed(const Uint8 *states)
{
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

// UNUSED: if exactly two arrow keys are held -> return true
// *states: keyboard state to parse
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

	// return bool value if count equals two (i.e., moving diagonally)
	return count == 2 ? true : false;
}

// moves player according to arrow input
// *keyboard_states: keyboard state to parse
void do_player_motion_input(const Uint8 *keyboard_states)
{
	// mechanism for player to grind at field border
	// bool values refer to if player's itembox hitbox is in playing field
	bool in_left =
		player[curr_player].itembox.p.x
			- player[curr_player].itembox.r
			> FIELD_OFFSET_X;
	bool in_up =
		player[curr_player].itembox.p.y
			- player[curr_player].itembox.r
			> FIELD_OFFSET_Y;
	bool in_down =
		player[curr_player].itembox.p.y
			+ player[curr_player].itembox.r
			< FIELD_OFFSET_Y + FIELD_HEIGHT;
	bool in_right =
		player[curr_player].itembox.p.x
			+ player[curr_player].itembox.r
			< FIELD_OFFSET_X + FIELD_WIDTH;
			
	stop_player_motion();
	double factored_speed = set_player_nondiagonal_speed(keyboard_states);
	double diagonal_speed = factored_speed * INVERSE_SQRT_2;
				
	/* non-diagonal movement */
	
	// left movement
	if (keyboard_states[SDL_SCANCODE_LEFT] && in_left)
		player[curr_player].motion.v.x = -factored_speed;
	// right movement
	if (keyboard_states[SDL_SCANCODE_DOWN] && in_down)
		player[curr_player].motion.v.y = factored_speed;
	// up movement
	if (keyboard_states[SDL_SCANCODE_UP] && in_up)
		player[curr_player].motion.v.y = -factored_speed;
	// down movement
	if (keyboard_states[SDL_SCANCODE_RIGHT] && in_right)
		player[curr_player].motion.v.x = factored_speed;

	/* diagonal movement */
	
	// diagonal left-down
	if (keyboard_states[SDL_SCANCODE_LEFT] &&
			keyboard_states[SDL_SCANCODE_DOWN]) {
		if (in_down)
			player[curr_player].motion.v.y = diagonal_speed;
		if (in_left)
			player[curr_player].motion.v.x = -diagonal_speed;
	}
	
	// diagonal left-up
	if (keyboard_states[SDL_SCANCODE_LEFT] &&
			keyboard_states[SDL_SCANCODE_UP]) {
		if (in_left)
			player[curr_player].motion.v.x = -diagonal_speed;
		if (in_up)
			player[curr_player].motion.v.y = -diagonal_speed;
	}
	
	// diagonal right-down
	if (keyboard_states[SDL_SCANCODE_RIGHT] &&
			keyboard_states[SDL_SCANCODE_DOWN]) {
		if (in_right)
			player[curr_player].motion.v.x = diagonal_speed;
		if (in_down)
			player[curr_player].motion.v.y = diagonal_speed;
	}
	
	// diagonal right-up
	if (keyboard_states[SDL_SCANCODE_RIGHT] &&
			keyboard_states[SDL_SCANCODE_UP]) {
		if (in_right)
			player[curr_player].motion.v.x = diagonal_speed;
		if (in_up)
			player[curr_player].motion.v.y = -diagonal_speed;
	}
}

// starts player bullet on input (accounting for shot reload)
// *keyboard_states: keyboard state to parse
void do_shot_input(const Uint8 *keyboard_states)
{
	// on z press
	// https://www.parallelrealities.co.uk/tutorials/shooter/shooter5.php
	if (keyboard_states[SDL_SCANCODE_Z]) {
		switch (curr_player) {
		// does reimu shot
		case PLAYER_REIMU:
			activate_reimu_shot();
			break;
		// does marisa shot
		case PLAYER_MARISA:
			activate_marisa_shot();
			break;
		default:
			break;
		}
	}
}

// uses bomb on input (accounting for bomb reload)
// *keyboard_states: keyboard state to parse
void do_bomb_input(const Uint8 *keyboard_states)
{
	// on x press, do bomb
	if (keyboard_states[SDL_SCANCODE_X]) {
		// bomb is not on cooldown and player has bombs
		if (bomb_reload < 0 && player[curr_player].bombs > 0) {
			// clears portion of field 
			delete_danmaku_in_circle(
					player[curr_player].motion.hb.p.x,
					player[curr_player].motion.hb.p.y,
					BOMB_RADIUS);
			// sets bomb reload
			bomb_reload = MAX_BOMB_RELOAD;
			player[curr_player].bombs--; // subtract bombs
			
			// set invincibility
			player[curr_player].invincible = true;
			player[curr_player].iframes = MAX_BOMB_RELOAD;
			
			// deathbomb cancels death
			if (player[curr_player].deathbomb_active == 1) {
				player[curr_player].deathbomb_active
					= 0;
				player[curr_player].deathbomb_timer
					= MAX_DEATHBOMB_RELOAD;
			}

			// TODO: move to mixer
			switch (curr_player) {
			case PLAYER_REIMU:
				play_sfx("sfx/bomb_reimu_a.ogx");
				break;
			case PLAYER_MARISA:
				play_sfx("sfx/bomb_marisa_a.ogx");
				break;
			default:
				break;
			}
		}
	}
}

// encompasses all input functionalities
// returns 1 to quit program
int do_input()
{
	// questions/1252976 (stackoverflow)
	// github: mikeanthonywild/sdl-shoot-em-up
	SDL_Event event;
	const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
	
	// polls input event
	// key_down setup avoids key repeat delay
	// questions/21311824/sdl2-key-repeat-delay (stackoverflow)
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
		// releases keypress
		case SDL_KEYUP:
//			stop_player_motion();
// 				key_down = 0;
// 				scanned_key = 0;
			break;
		default:
			break;
		}
	}
	
	/* universal inputs */
	
	// quits game
	if (holding_quit(keyboard_states))
		return 1;
		
	/* state-specific inputs */
	
	switch (state_menu) {
	case STATE_PLAY:
		int i;

		/* movement */
		
		do_player_motion_input(keyboard_states);
			
		/* shooting */

		// basic shot
		do_shot_input(keyboard_states);

		// bomb
		do_bomb_input(keyboard_states);
		return 0;
		break;
	case STATE_MAIN_MENU:
		do_main_menu_input();
		return 0;
		break;
	default:
		return 0;
		break;
	}
}

