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
#include "player.h"
#include "shot.h"


/* VARIABLES */

// textures
SDL_Window *screen;
SDL_Renderer *rend;

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

// initialize chosen player
void initialize_player_render()
{
	int i, j;
	// TODO: player selection in main menu
	// reimu
	player[PLAYER_REIMU].sdl.texture = create_texture("assets/reimu.png");
	player[PLAYER_MARISA].sdl.texture = create_texture("assets/marisa.png");

	for (i = 0; i < MAX_PLAYERS; i++) {
		SDL_QueryTexture(player[i].sdl.texture, NULL, NULL,
				&player[i].sdl.rect.w,
				&player[i].sdl.rect.h);
	}
}

void initialize_shot_render()
{
	int i, j;
	// player bullets
	shot_reimu.sdl.texture = create_texture("assets/player_bullet.png");
	for (i = 0; i < MAX_SHOTS; i++) {
		for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
			SDL_QueryTexture(shot_reimu.sdl.texture, NULL, NULL,
					&shot_reimu.sdl.rect.w,
					&shot_reimu.sdl.rect.h);
		}
	}
}

// loads sprites to texture of bullets
void initialize_bullet_render()
{
	int i;

	// "snowball" bullets
	ball_8x8.sdl.texture = create_texture("assets/bullet_snow.png");
	for (i = 0; i < MAX_BULLETS; i++) {
		SDL_QueryTexture(ball_8x8.sdl.texture, NULL, NULL,
				&ball_8x8.sdl.rect.w,
				&ball_8x8.sdl.rect.h);
	}
}

// load sprites of stage enemies, bullets, and bosses
void initialize_enemy_render()
{
	int i;

	// fairies
	fairies.sdl.texture
		= create_texture("assets/fairy.png");
	for (i = 0; i < MAX_ENEMY_MULTIPLE; i++) {
		SDL_QueryTexture(fairies.sdl.texture, NULL, NULL,
				&fairies.sdl.rect.w, &fairies.sdl.rect.h);
	}
}

// load items
void initialize_items()
{
	int i;

	powerup.sdl.texture = create_texture("assets/power_small.png");
	for (i = 0; i < MAX_ITEM_MULTIPLE; i++) {
		SDL_QueryTexture(powerup.sdl.texture, NULL, NULL, &powerup.sdl.rect.w,
				&powerup.sdl.rect.h);
	}
}

// load sprites
void initialize_textures(void)
{
	initialize_overlays();
	initialize_player_render();
	initialize_bullet_render();
	initialize_enemy_render();
	initialize_items();
	initialize_shot_render();
}

// set sdl int position to match the float position for a bullet
void update_bullet_render(struct bullet *p)
{
	p -> sdl.rect.x = (int)(p -> motion.hb.p.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> motion.hb.p.y - p -> sdl.rect.h / 2.);
}

// set sdl int position to match the float position for the player
void update_player_render(struct player *p)
{
	p -> sdl.rect.x = (int)(p -> motion.hb.p.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> motion.hb.p.y - p -> sdl.rect.h / 2.);
}

// set sdl int position to match the float position for an enemy
void update_enemy_render(struct enemy *p)
{
	p -> sdl.rect.x = (int)(p -> motion.hb.p.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> motion.hb.p.y - p -> sdl.rect.h / 2.);
}

// copies motion values into SDL_Rect and renders new sprite position
void do_player_render()
{
	update_player_render(&player[curr_player]);
	SDL_RenderCopy(rend, player[curr_player].sdl.texture, NULL,
			&player[curr_player].sdl.rect);
}

// update objects on screen
void do_screen()
{
	int i, j;

	// clear screen
	SDL_RenderClear(rend);

	do_player_render();

	// bullets
	for (i = 0; i < MAX_BULLETS; i++) {
		for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
			if (bulletlist[i][j].active) {
				update_bullet_render(&bulletlist[i][j]);
				SDL_RenderCopy(rend, bulletlist[i][j].sdl.texture, NULL, &bulletlist[i][j].sdl.rect);
			}
		}
	}

	// enemies
	for (i = 0; i < MAX_ENEMIES; i++) {
		for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
			if (enemylist[i][j].active) {
				update_enemy_render(&enemylist[i][j]);
				SDL_RenderCopy(rend,
						enemylist[i][j].sdl.texture,
						NULL,
						&enemylist[i][j].sdl.rect);
			}
		}
	}

	// player bullets
	for (i = 0; i < MAX_SHOTS; i++) {
		for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
			if (shotlist[i][j].active) {
				update_bullet_render(&shotlist[i][j]);
				SDL_RenderCopy(rend, shotlist[i][j].sdl.texture,
						NULL,
						&shotlist[i][j].sdl.rect);
			}
		}
	}

	// items
	for (i = 0; i < MAX_ITEMS; i++) {
		for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
			if (itemlist[i][j].active) {
				itemlist[i][j].sdl.rect.x = (int)(itemlist[i][j].motion.hb.p.x
						- itemlist[i][j].sdl.rect.w / 2.);
				itemlist[i][j].sdl.rect.y = (int)(itemlist[i][j].motion.hb.p.y
						- itemlist[i][j].sdl.rect.h / 2.);
				SDL_RenderCopy(rend, itemlist[i][j].sdl.texture,
						NULL, &itemlist[i][j].sdl.rect);
			}
		}
	}
	// overlays
	SDL_RenderCopy(rend, border.sdl.texture, NULL, &border.sdl.rect);

	// end
	SDL_RenderPresent(rend);
}

// update sprites in main menu
void do_main_menu_screen()
{
	SDL_RenderClear(rend);
	SDL_RenderCopy(rend, main_menu.sdl.texture, NULL, &main_menu.sdl.rect);
	SDL_RenderPresent(rend);
}

