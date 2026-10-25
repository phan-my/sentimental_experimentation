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
#include "menu.h"
#include "texts.h"


/* VARIABLES */

// textures
SDL_Window *screen;
SDL_Renderer *rend;
struct text tmptext;


// dests

/* FUNCTIONS */

/* static */

// creates texture with given sprite by allocating and freeing temporary surface
// sprite: path to file of sprite
SDL_Texture *create_texture(char *sprite)
{
	SDL_Surface *surface = IMG_Load(sprite);
	// load image to memory
	SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);
	return texture;
}

// sets coordinates of SDL_rect type to provided values 
// rect: given SDL_Rect type
// xcoord: x-coordinate at which to set rect
// ycoord: y-coordinate at which to set rect
void position_rect(SDL_Rect rect, int xcoord, int ycoord)
{
	rect.x = (int)xcoord;
	rect.y = (int)ycoord;
}

// loads sprites to texture of and position in-game overlay
void initialize_overlay_render()
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

// loads sprites to texture of players
void initialize_player_render()
{
	int i, j;
	// TODO: player selection in main menu
	player[PLAYER_REIMU].sdl.texture = create_texture("assets/reimu.png");
	player[PLAYER_MARISA].sdl.texture = create_texture("assets/marisa.png");

	for (i = 0; i < MAX_PLAYERS; i++) {
		SDL_QueryTexture(player[i].sdl.texture, NULL, NULL,
				&player[i].sdl.rect.w,
				&player[i].sdl.rect.h);
	}
}

// loads sprites to texture of player shots
void initialize_shot_render()
{
	int i, j;
	// reimu's basic shot
	shot_reimu.sdl.texture = create_texture("assets/shot_reimu.png");
	SDL_QueryTexture(shot_reimu.sdl.texture, NULL, NULL,
			&shot_reimu.sdl.rect.w,
			&shot_reimu.sdl.rect.h);
	
	// marisa's basic shot
	shot_marisa.sdl.texture = create_texture("assets/shot_marisa.png");
	SDL_QueryTexture(shot_marisa.sdl.texture, NULL, NULL,
			&shot_marisa.sdl.rect.w,
			&shot_marisa.sdl.rect.h);
}

// loads sprites to texture of bullets
void initialize_bullet_render()
{
	int i;

	// "snowball" bullets
	ball_8x8.sdl.texture = create_texture("assets/bullet_snow.png");
	SDL_QueryTexture(ball_8x8.sdl.texture, NULL, NULL,
			&ball_8x8.sdl.rect.w,
			&ball_8x8.sdl.rect.h);
	
	// 16x16 snowball bullets
	ball_16x16.sdl.texture = create_texture("assets/ball.png");
	SDL_QueryTexture(ball_16x16.sdl.texture, NULL, NULL,
			&ball_16x16.sdl.rect.w,
			&ball_16x16.sdl.rect.h);
			
	// bubble bullets
	bubble.sdl.texture = create_texture("assets/bubble.png");
	SDL_QueryTexture(bubble.sdl.texture, NULL, NULL,
			&bubble.sdl.rect.w,
			&bubble.sdl.rect.h);
}

// loads sprites to texture of enemies
void initialize_enemy_render()
{
	int i;

	// red fairies
	fairy.sdl.texture
		= create_texture("assets/fairy.png");
	SDL_QueryTexture(fairy.sdl.texture, NULL, NULL,
			&fairy.sdl.rect.w, &fairy.sdl.rect.h);
	
	// blue fairies
	fairy_blue.sdl.texture
		= create_texture("assets/fairy_blue.png");
	SDL_QueryTexture(fairy_blue.sdl.texture, NULL, NULL,
			&fairy_blue.sdl.rect.w, &fairy_blue.sdl.rect.h);
}

// loads sprites to texture of items
void initialize_item_render()
{
	int i;

	// small powerup item
	powerup.sdl.texture = create_texture("assets/power_small.png");
	SDL_QueryTexture(powerup.sdl.texture, NULL, NULL, &powerup.sdl.rect.w,
			&powerup.sdl.rect.h);
	
	// small score item
	score.sdl.texture = create_texture("assets/score_small.png");
	SDL_QueryTexture(score.sdl.texture, NULL, NULL, &score.sdl.rect.w,
			&score.sdl.rect.h);
}


// loads sprites to texture of all entities
void initialize_textures(void)
{
	initialize_overlay_render();
	initialize_player_render();
	initialize_bullet_render();
	initialize_enemy_render();
	initialize_item_render();
	initialize_shot_render();
	initialize_widgets();
}

// exits program if SDL initialization fails, prints error
void check_sdl_init(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("ERROR: %s\n", SDL_GetError());
		exit(1);
	}
}

// checks that screen was retrieved
void check_screen()
{
	if (screen == NULL) {
		SDL_Quit();
		exit(1);
	}
}

// sets up window and flag
void initialize_screen(void)
{
	// scale of the window
	double windowscale = 2.;
	
	// check SDL initialization; if error, quit program
	check_sdl_init();
	
	// create window
	screen = SDL_CreateWindow(WINDOW_TITLE,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_CENTERED,
			SCREEN_WIDTH * windowscale, SCREEN_HEIGHT * windowscale,
			SDL_WINDOW_OPENGL);
	
	// check that surface was retrieved
	check_screen();

	// flag setup
	// https://studios.ptilouk.net/superfluous-returnz/blog/2023-03-14_vsync.html
	Uint32 render_flags =
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	rend = SDL_CreateRenderer(screen, -1, render_flags);
	
	// upscaling screen
	// https://superjamie.github.io/2022/11/06/sdl-fullscreen-scaling
	SDL_RenderSetLogicalSize(rend, SCREEN_WIDTH * windowscale,
			SCREEN_HEIGHT * windowscale);
	SDL_RenderSetIntegerScale(rend, SDL_TRUE);
//	SDL_SetWindowFullscreen(screen, 1);
	SDL_RenderSetScale(rend, windowscale, windowscale);

	// load graphics
	initialize_textures();
	
	initialize_ttf();
}

// closes all windows and quits program
void terminate_screen(void)
{
	SDL_DestroyWindow(screen);
	SDL_Quit();
}

/* dynamic */

// sets integer graphical position to match the logical position for a bullet
// *p: the structure of the `bullet`, passed by reference
void update_bullet_render(struct bullet *p)
{
	p -> sdl.rect.x = (int)(p -> motion.hb.p.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> motion.hb.p.y - p -> sdl.rect.h / 2.);
}

// sets integer graphical position to match the logical position for the player
// *p: the structure of the `player`, passed by reference
void update_player_render(struct player *p)
{
	p -> sdl.rect.x = (int)(p -> motion.hb.p.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> motion.hb.p.y - p -> sdl.rect.h / 2.);
}

// sets integer graphical position to match the logical position for an enemy
// *p: the structure of the `enemy`, passed by reference
void update_enemy_render(struct enemy *p)
{
	p -> sdl.rect.x = (int)(p -> motion.hb.p.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> motion.hb.p.y - p -> sdl.rect.h / 2.);
}

// sets integer graphical position to match the logical position for an item
// *p: the structure of the `item`, passed by reference
void update_item_render(struct item *p)
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

// copies motion values into SDL_Rect and renders new sprite position of active
// bullets
void do_bullet_render()
{
	int i, j;

	// loops through each multiple of every bullet in `bulletlist`
	for (i = 0; i < MAX_BULLETS; i++) {
	for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
		// update if active
		if (bulletlist[i][j].active) {
			update_bullet_render(&bulletlist[i][j]);			
			SDL_RenderCopy(rend, bulletlist[i][j].sdl.texture, NULL,
					&bulletlist[i][j].sdl.rect);
		}
	}
	}
}

// copies motion values into SDL_Rect and renders new sprite position of active
// enemies
void do_enemy_render()
{
	int i, j;

	// loops through each multiple of every bullet in `enemylist`
	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		// update if active
		if (enemylist[i][j].active) {
			update_enemy_render(&enemylist[i][j]);
			SDL_RenderCopy(rend, enemylist[i][j].sdl.texture, NULL,
					&enemylist[i][j].sdl.rect);
		}
	}
	}
}

// copies motion values into SDL_Rect and renders new sprite position of active
// player shots
void do_shot_render()
{
	int i, j;

	// loops through each multiple of every bullet in `shotlist`
	for (i = 0; i < MAX_SHOTS; i++) {
	for (j = 0; j < MAX_SHOT_MULTIPLE; j++) {
		// update if active
		if (shotlist[i][j].active) {
			update_bullet_render(&shotlist[i][j]);
			SDL_RenderCopy(rend, shotlist[i][j].sdl.texture,
					NULL,
					&shotlist[i][j].sdl.rect);
		}
	}
	}
}

// copies motion values into SDL_Rect and renders new sprite position of active
// items
void do_item_render()
{
	int i, j;
	
	for (i = 0; i < MAX_ITEMS; i++) {
	for (j = 0; j < MAX_ITEM_MULTIPLE; j++) {
		// update if active
		if (itemlist[i][j].active) {
			update_item_render(&itemlist[i][j]);
			SDL_RenderCopy(rend, itemlist[i][j].sdl.texture,
					NULL, &itemlist[i][j].sdl.rect);
		}
	}
	}
}

// updates overlay during gameplay
void do_overlay_render()
{
	SDL_RenderCopy(rend, border.sdl.texture, NULL, &border.sdl.rect);
}

// updates sprites in main menu
void do_main_menu_render()
{
//	draw_widgets();
//	initDemo();
	SDL_SetRenderDrawColor(rend, 0,0,0, 0);
	SDL_RenderClear(rend);
	draw_text(text_reimu, SCREEN_WIDTH / 2., SCREEN_HEIGHT / 2. - 20);
	draw_text(text_marisa, SCREEN_WIDTH / 2., SCREEN_HEIGHT / 2. + 20);
//	SDL_RenderCopy(rend, main_menu.sdl.texture, NULL, &main_menu.sdl.rect);

	SDL_RenderPresent(rend);
}

// updates sprites during gameplay
void do_play_render()
{
	// clear screen
	SDL_RenderClear(rend);
	
	// updates all active sprites
	do_shot_render();
	do_player_render();
	do_item_render();
	do_enemy_render();
	do_bullet_render();
	do_overlay_render();

	// end
	SDL_RenderPresent(rend);
}

// update objects on screen
void do_screen()
{
	switch (state_menu) {
	case STATE_PLAY:
		do_play_render();
		break;
	case STATE_MAIN_MENU:
		do_main_menu_render();
		break;
	default:
		break;
	}
}

