/*
 * main.c
 *
 * gcc main.c -lm -lSDL2 -lSDL2_image -lSDL2_mixer -o omain.out
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "logic.h"
#include "random.h"
#include "sounds.h"

// (480x560 | 384x448)
#define FIELD_WIDTH 384
#define FIELD_HEIGHT 448
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FIELD_OFFSET_X 32
#define FIELD_OFFSET_Y 16

#define D_RECT_X 0
#define D_RECT_Y 1
#define NUM_BULLETS 1000
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078
#define GOLDEN_RATIO 0.6180339887498948482045868343656381177203091798057628621
#define INVERSE_SQRT_2 0.7071067811865475244008444

// invoke as dest = f(d_dest, speed, turns)
double *launch_straight(double *d_dest, double speed, double turns)
{
	d_dest[D_RECT_X] += speed * cos(turns);
	d_dest[D_RECT_Y] += speed * sin(turns);
	return d_dest;
}

// simple moving average (SMA)
double rolling_average(double *points, int n)
{
	int k = 60;
	double sum;
	int i;

	for (i = n - k + 1; i <= n; i++) 
		sum += points[i];

	return (1. / k) * sum;
}

int main(int argc, char **argv)
{
	int i, j;

	// begin SDL2 setup
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("ERROR %s\n", SDL_GetError());
		return 1;
	}

	// create window
	SDL_Window *win = SDL_CreateWindow("Sentimental Experimentation",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_CENTERED,
				SCREEN_WIDTH, SCREEN_HEIGHT,
				SDL_WINDOW_OPENGL);

	// check that surface was retrieved
	if(win == NULL){
		SDL_Quit();
		return 1;
	}

	// flag setup
	// https://studios.ptilouk.net/superfluous-returnz/blog/2023-03-14_vsync.html
	Uint32 render_flags =
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);

	/*
	Uint32 reimu_render_flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer *reimu_rend = SDL_CreateRenderer(win, -1, reimu_render_flags);
	*/


	/* image loader */

	struct player reimu;
	struct ball dest[NUM_BULLETS];
	int num_fairies = 10;
	struct enemy fairies[num_fairies];
	bool active_fairies[num_fairies];
	for (i = 0; i < num_fairies; i++)
		active_fairies[i] = 1;
	const int num_player_bullets = 64;
	struct ball player_bullets[num_player_bullets];

	// load background
	SDL_Surface *border_surface;	
	border_surface = IMG_Load("assets/window.png");
	SDL_Texture *border_tex = SDL_CreateTextureFromSurface(rend, border_surface);
	SDL_FreeSurface(border_surface);
	
	// load player
	reimu.sdl.surface = IMG_Load("assets/reimu.png"); // path to sprite

	// load image to memory
	reimu.sdl.texture =
		SDL_CreateTextureFromSurface(rend, reimu.sdl.surface);
	SDL_FreeSurface(reimu.sdl.surface);

	// load fairies
	for (i = 0; i < num_fairies; i++) {
		fairies[i].sdl.surface = IMG_Load("assets/fairy.png");
		fairies[i].sdl.texture = SDL_CreateTextureFromSurface(rend,
				fairies[i].sdl.surface);
		SDL_FreeSurface(fairies[i].sdl.surface);
	}

	// load bullets
	SDL_Surface *surface = IMG_Load("assets/bullet_snow.png");
	SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);

	// load player bullets
	SDL_Surface *player_bullet_surface =
		IMG_Load("assets/player_bullet.png");
	SDL_Texture *player_bullet_texture = SDL_CreateTextureFromSurface(rend,
			player_bullet_surface);
	SDL_FreeSurface(player_bullet_surface);

	
	/* positioning hitboxes */

	// background
	SDL_Rect border_dest;
	SDL_QueryTexture(border_tex, NULL, NULL, &border_dest.w,
				&border_dest.h);
	border_dest.x = 0;
	

	// player
	// create object
	SDL_QueryTexture(reimu.sdl.texture, NULL, NULL, &reimu.sdl.rect.w,
				&reimu.sdl.rect.h);
	reimu.hitbox.x = FIELD_WIDTH / 2. + FIELD_OFFSET_X;
	reimu.hitbox.y = FIELD_HEIGHT * 0.75 + FIELD_OFFSET_Y;
	update_player_position(&reimu);
	reimu.hitbox.r = 2.;

	// "snowball" bullets
	for (i = 0; i < NUM_BULLETS; i++) {
		SDL_QueryTexture(tex, NULL, NULL, &dest[i].sdl.rect.w,
				&dest[i].sdl.rect.h);
//		dest[i].w /= scale;
//		dest[i].h /= scale;
		dest[i].hitbox.x = FIELD_WIDTH / 2 + FIELD_OFFSET_X;
		dest[i].hitbox.y = FIELD_HEIGHT * (1. / 4) + FIELD_OFFSET_Y;
		dest[i].hitbox.r = 3.8;
		update_ball_position(&dest[i]);
	}


	// fairy
	for (i = 0; i < num_fairies; i++) {
		SDL_QueryTexture(fairies[i].sdl.texture, NULL, NULL,
				&fairies[i].sdl.rect.w, &fairies[i].sdl.rect.h);
		fairies[i].hitbox.x = FIELD_OFFSET_X + randint(0, FIELD_WIDTH);
		fairies[i].hitbox.y = FIELD_OFFSET_Y;
		update_enemy_position(&fairies[i]);
		fairies[i].hitbox.r = 8.;
		fairies[i].health = 2;
	}

	// player_bullets
	for (i = 0; i < num_player_bullets; i++) {
		SDL_QueryTexture(player_bullet_texture, NULL, NULL,
				&player_bullets[i].sdl.rect.w, &player_bullets[i].sdl.rect.h);
		player_bullets[i].hitbox.x = reimu.hitbox.x;
		player_bullets[i].hitbox.y = reimu.hitbox.y;
		player_bullets[i].hitbox.r = 8.;
		player_bullets[i].power = 1;
	}

	/* music loader */
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	init_sounds();
	play_track("02");


	/* numerical setup */
	
	// JELLYFISH
	// set bullet speeds
	double speed[NUM_BULLETS];
	double cap_speed;
	for (i = 1; i < NUM_BULLETS; i++) {
		if (i % 40 < 20)
			cap_speed = 2.;
		else
			cap_speed = 3.;
		if (i % 2)
			speed[i] = (double) 1. + i / 50. / 6.;
		else
			speed[i] = (double) 1. + i / 67. / 6.;
		if (speed[i] > cap_speed)
			speed[i] = cap_speed;
	}

	// NORMAL (SPIRAL SPEEDS
	// set bullet speeds
	/*
	double speed[NUM_BULLETS];
	for (i = 0; i < NUM_BULLETS; i++) 
		speed[i] = (double) i / 50;
	*/
	
	// create double version of dest for subpixel precision
	double angles[NUM_BULLETS];
	for (i = 0; i < NUM_BULLETS; i++) {
		update_ball_position(&dest[i]);
		if (i % 2)
			angles[i] = i * GOLDEN_RATIO;
		else
			angles[i] = i * 1.3 * GOLDEN_RATIO;
	}
	
	// variables for bullet movement
	double r;
	double phi = 0.;
	double a = 1;
	double moving = 1;
	
	// player speed
	// https://en.touhouwiki.net/wiki/User:Arcorann/Character_Speeds#Massive_chart
	double reimu_default_speed = 4.;
	const double focus_factor = 1.6 / reimu_default_speed;
	double marisa_speed = reimu_default_speed * 1.5;
	// for lshift focus
	double factored_speed = reimu_default_speed;
	double diagonal;

	// player bullet
	double player_bullet_speed = 8.;

	/* main loop */

	// events management
	bool close = 0;
	SDL_Event event;
	bool key_down = 0;
	int scanned_key = 0;

	// timing
	double frames[10000];
	struct timespec start_game;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start_game);
	int hour, minute, second;

	struct timespec dt_start;
	struct timespec dt_end;
	uint64_t dt; // in microseconds
	clock_gettime(CLOCK_MONOTONIC_RAW, &dt_start);
	clock_gettime(CLOCK_MONOTONIC_RAW, &dt_end);
	Uint64 ticks = 0;

	// prevent movement at field border
	bool in_left; 
	bool in_down;
	bool in_up;
	bool in_right;

	// extra margins for player at bottom of field
	int player_bottom_margin = 16;

	// player bullet go! queue
	bool active_player_bullets[num_player_bullets];
	for (i = 0; i < num_player_bullets; i++)
		active_player_bullets[i] = false;
	int nth_player_bullet = 0; // global variable
	
	// space between player bullet in ticks
	const int MAX_RELOAD = 5;
	int reload = MAX_RELOAD;

	// main loop
	while (!close) {
		//		SDL_EnableKeyRepeat(0, 0);
		// questions/1252976

		const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
		
		// mechanism for player to grind at field border
		in_left	= reimu.sdl.rect.x > FIELD_OFFSET_X;
		in_up = reimu.sdl.rect.y > FIELD_OFFSET_Y;
		in_down = reimu.sdl.rect.y < FIELD_OFFSET_Y + FIELD_HEIGHT
			- reimu.sdl.rect.h + player_bottom_margin;
		in_right = reimu.sdl.rect.x < FIELD_OFFSET_X + FIELD_WIDTH
			- reimu.sdl.rect.w;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {

			// close button
			case SDL_QUIT:
				close = 1;
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
				close = 1;


			/* movement */

			if (keyboard_states[SDL_SCANCODE_LSHIFT])
				factored_speed =
					focus_factor * reimu_default_speed;
			else
				factored_speed = reimu_default_speed;
			diagonal = factored_speed * (INVERSE_SQRT_2 - 1);

			// non-diagonal movement
			if (keyboard_states[SDL_SCANCODE_LEFT] && in_left)
				reimu.hitbox.x -= factored_speed;
			if (keyboard_states[SDL_SCANCODE_DOWN] && in_down)
				reimu.hitbox.y += factored_speed;
			if (keyboard_states[SDL_SCANCODE_UP] && in_up)
				reimu.hitbox.y -= factored_speed;
			if (keyboard_states[SDL_SCANCODE_RIGHT] && in_right)
				reimu.hitbox.x += factored_speed;

			// diagonal movement
			if (keyboard_states[SDL_SCANCODE_LEFT] &&
					keyboard_states[SDL_SCANCODE_DOWN]) {
				if (in_down)
					reimu.hitbox.y += diagonal;
				if (in_left)
					reimu.hitbox.x -= diagonal;
			}
			if (keyboard_states[SDL_SCANCODE_LEFT] &&
					keyboard_states[SDL_SCANCODE_UP]) {
				if (in_left)
					reimu.hitbox.x -= diagonal;
				if (in_up)
					reimu.hitbox.y -= diagonal;
			}
			if (keyboard_states[SDL_SCANCODE_RIGHT] &&
					keyboard_states[SDL_SCANCODE_DOWN]) {
				if (in_right)
					reimu.hitbox.x += diagonal;
				if (in_down)
					reimu.hitbox.y += diagonal;
			}
			if (keyboard_states[SDL_SCANCODE_RIGHT] &&
					keyboard_states[SDL_SCANCODE_UP]) {
				if (in_right)
					reimu.hitbox.x += diagonal;
				if (in_up)
					reimu.hitbox.y -= diagonal;
			}

//			reimu.sdl.rect.x = reimu.hitbox.x;
//			reimu.sdl.rect.y = reimu.hitbox.y;
			update_player_position(&reimu);

			
			/* shooting */

			// https://www.parallelrealities.co.uk/tutorials/shooter/shooter5.php
			if (keyboard_states[SDL_SCANCODE_Z]) {
				// activate one bullet
				if (reload == 0)
					active_player_bullets[nth_player_bullet]
						= 1;
				// cycles through array
				while (active_player_bullets[nth_player_bullet]) {
					nth_player_bullet++;
					nth_player_bullet %= num_player_bullets;
				}
			}
			reload--;
			if (reload < 0)
				reload = MAX_RELOAD;
		}

		// update player bullets
		for (i = 0; i < num_player_bullets; i++) {
			// update bullets if they fall out of bounds
			if (player_bullets[i].hitbox.y < 0 ||
					!active_player_bullets[i]) {
				active_player_bullets[i] = 0;
				player_bullets[i].hitbox.x = reimu.hitbox.x;
				player_bullets[i].hitbox.y = reimu.hitbox.y;
			}
			if (active_player_bullets[i]) {
				player_bullets[i].hitbox.y -= player_bullet_speed;
				update_ball_position(&player_bullets[i]);
			}
		}

		/* BULLET MOVEMENTS */

		/*
		// sine movement
		double period = 300;
		double amplitude = 100;
		for (i = 0; i < NUM_BULLETS; i++) {
			d_dest[i][D_RECT_Y] += amplitude * 2 * PI / period * 
				cos(2 * PI * dest[i].x / period);
 			dest[i].y = (int)d_dest[i][D_RECT_Y];
			dest[i].x = (int)(d_dest[i][D_RECT_X] += speed[i]);
		}
		*/
		
		/* spiral TRAIL movement
		r = a * phi;
		for (i = 0; i < NUM_BULLETS; i++) {
			dest[i].y = (d_dest[i][D_RECT_Y] = speed[i] * r * cos(phi) + SCREEN_HEIGHT / 2);
			dest[i].x = (d_dest[i][D_RECT_X] = speed[i] * r * sin(phi) + SCREEN_WIDTH / 2);
		}
		phi += 0.05;
		*/

		/*
		// sunflower spirals
		for (i = 0; i < moving; i++) {
			double p = 1.;
			d_dest[i][D_RECT_X] += 3. * cos(angles[i] / p);
			d_dest[i][D_RECT_Y] += 3. * sin(angles[i] / p);
			dest[i].x = d_dest[i][D_RECT_X];
			dest[i].y = d_dest[i][D_RECT_Y];
		}
		
		if (moving < NUM_BULLETS - 60) {
			moving += 5 * GOLDEN_RATIO;
//			printf("%lf\n", moving);
		} else {
			printf("Recycle your bullets!\n");
			break;
		}
		*/


		// Jellyfish
		// speed[i] = (double)i / 100;
		for (i = 0; i < moving; i++) {
			dest[i].hitbox.x += speed[i] * cos(angles[i]);
			dest[i].hitbox.y += speed[i] * sin(angles[i]);
			update_ball_position(&dest[i]);
		}
		
		if (moving < NUM_BULLETS)
			moving += 2;


		/* ENEMY MOVEMENT */

		int stopping_line = FIELD_OFFSET_Y + 200;
		double fairy_speed = 1.;
		
		for (i = 0; i < num_fairies; i++) {
			double diff = stopping_line - fairies[i].hitbox.y;
			if (fairies[i].hitbox.y < stopping_line)
				fairies[i].hitbox.y += fairy_speed ;

			update_enemy_position(&fairies[i]);
		}
		

		/* COLLISION DETECTION*/
		// TODO: quadtree hitbox detection
		// questions/21650246/sdl-2-collision-detetection
		// github.com/arpit2297/Collision-Detection-using-Quad-Trees
		
		// player -- enemy bullet
		for (i = 0; i < NUM_BULLETS; i++) {
			if (is_hit(dest[i].hitbox, reimu.hitbox)) {
				printf("%d: HIT\n", i);
				if (i % 2)
					printf("T\n");
			}
		}

		// player -- fairy -- player bullet
		for (i = 0; i < num_fairies; i++) {
			// fairy hits player
			if (is_hit(fairies[i].hitbox, reimu.hitbox))
				printf("FAIRY HITT  \n");

			// player bullet hits fairy
			for (j = 0; j < num_player_bullets; j++) {
				// fairy takes damage
				if (active_player_bullets[j] && active_fairies[i] && is_hit(fairies[i].hitbox,
							player_bullets[j].hitbox)) {
					printf("PLAYER BULLET HTIS FAIRY\n");

					// damage dealt based on player bullet power
					fairies[i].health -= player_bullets[j].power;
					
					// unload bullet
					player_bullets[j].hitbox.y = 0;

					// fairy dies
					if (fairies[i].health == 0) {
						active_fairies[i] = 0;
						active_player_bullets[j] = 0;
					}
				}

			}
		}
		

		/* appendix */

		// clear screen
		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, reimu.sdl.texture, NULL, &reimu.sdl.rect);
		for (i = 0; i < NUM_BULLETS; i++)
			SDL_RenderCopy(rend, tex, NULL, &dest[i].sdl.rect);
		for (i = 0; i < num_fairies; i++) {
			if (active_fairies[i]) {
				SDL_RenderCopy(rend, fairies[i].sdl.texture,
						NULL, &fairies[i].sdl.rect);
			}
		}
		for (i = 0; i < num_player_bullets; i++)
			if (active_player_bullets[i])
				SDL_RenderCopy(rend, player_bullet_texture,
						NULL,
						&player_bullets[i].sdl.rect);

		SDL_RenderCopy(rend, border_tex, NULL, &border_dest);
	
		// double buffer
		SDL_RenderPresent(rend);
	}

	/* program termination */
	// delay termination
//	Uint32 delay = 0; // delay in ms
//	SDL_Delay(delay);

	// TODO: Implement free music
//	Mix_FreeMusic(music);
	Mix_Quit();
	// close all windows and quit
	SDL_DestroyWindow(win);
	SDL_Quit();
	
	return 0;
}

