/*
 * render.c includes templates to simplify SDL rendering.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

/* INCLUDES */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "render.h"
#include "logic.h"


/* VARIABLES */

// textures
SDL_Window *screen;
SDL_Renderer *rend;
struct ball dest[MAX_BULLETS];
struct enemy fairies[MAX_FAIRIES];

// dests

/* FUNCTIONS */

void check_sdl_init(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("ERROR: %s\n", SDL_GetError());
		exit(1);
	}
}

// set up window and flag
void initialize_screen(void)
{
	// create window
	screen = SDL_CreateWindow(WINDOW_TITLE,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_CENTERED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL);
	
	// check that surface was retrieved
	if (screen == NULL){
		SDL_Quit();
		exit(1);
	}

	// flag setup
	// https://studios.ptilouk.net/superfluous-returnz/blog/2023-03-14_vsync.html
	Uint32 render_flags =
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	rend = SDL_CreateRenderer(screen, -1, render_flags);
}

// close all windows and quit
void terminate_screen(void)
{
	SDL_DestroyWindow(screen);
	SDL_Quit();
}

// simplify tex creation by omitting surface
SDL_Texture *create_texture(char *sprite)
{
	SDL_Surface *surface = IMG_Load(sprite);
	// load image to memory
	SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void position_rect(SDL_Rect rect, int x, int y)
{
	rect.x = (int)x;
	rect.y = (int)y;
}

// create and position in-game overlay
void initialize_overlays()
{
	// main menu
	main_menu.sdl.texture = create_texture("assets/main_menu.png");
	SDL_QueryTexture(main_menu.sdl.texture, NULL, NULL,
			&main_menu.sdl.rect.w, &main_menu.sdl.rect.h);
	position_rect(main_menu.sdl.rect, 0, 0);

	// in-game overlay
	border.sdl.texture = create_texture("assets/window.png"); // UI
	SDL_QueryTexture(border.sdl.texture, NULL, NULL, &border.sdl.rect.w,
				&border.sdl.rect.h);
	position_rect(border.sdl.rect, 0, 0);

	// loading screen
	loading.sdl.texture = create_texture("assets/loading.png");
	SDL_QueryTexture(loading.sdl.texture, NULL, NULL, &loading.sdl.rect.w,
			&loading.sdl.rect.h);
	position_rect(loading.sdl.rect, 0, 0);
}


void initialize_player()
{
	// player
	// create object
	reimu.sdl.texture = create_texture("assets/reimu.png");
	SDL_QueryTexture(reimu.sdl.texture, NULL, NULL, &reimu.sdl.rect.w,
				&reimu.sdl.rect.h);
	// positioning
	reimu.hitbox.x = FIELD_WIDTH / 2. + FIELD_OFFSET_X;
	reimu.hitbox.y = FIELD_HEIGHT * 0.75 + FIELD_OFFSET_Y;
	update_player_position(&reimu); // subpixel hitbox -> macro rect
	reimu.hitbox.r = 2.;
}

void initialize_textures(void)
{
	int i;
	// fairy movement control
	for (i = 0; i < MAX_FAIRIES; i++)
		fairies[i].active = true;
	// player bullet go! queue
	for (i = 0; i < MAX_PLAYER_BULLETS; i++)
		player_bullets[i].active = false;

	// load sprites
	initialize_overlays();
	initialize_player();
	fairies[0].sdl.texture = create_texture("assets/fairy.png");
	tex = create_texture("assets/bullet_snow.png"); // bullets
	player_bullet_texture = create_texture("assets/player_bullet.png");
}
