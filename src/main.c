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

// (480x560 | 384x448)
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 560

#define D_RECT_X 0
#define D_RECT_Y 1
#define NUM_BULLETS 10000
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
//	printf("Hello, World\n");
	printf("I have %d argument(s), namely:\n", argc);
	int i;
	for (i = 0; i < argc; i++)
		printf("%s\n", argv[i]);


	// begin SDL2 setup
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("ERROR %s\n", SDL_GetError());
		return 1;
	}

	// create window
	SDL_Window *win = SDL_CreateWindow("Sentimental Experiments",
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
	SDL_Renderer *rend = SDL_CreateRenderer(win, -2, render_flags);

	/*
	Uint32 reimu_render_flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer *reimu_rend = SDL_CreateRenderer(win, -1, reimu_render_flags);
	*/


	/* image loader */

	struct entity player;
	int num_fairies = 100;
	struct entity fairies[num_fairies];
	
	// load player
	player.surface = IMG_Load("assets/reimu.png");	// path to sprite

	// load image to memory
	// FIXME?: initially SDL_Texture *player_tex = ...
	player.tex = SDL_CreateTextureFromSurface(rend, player.surface);
	SDL_FreeSurface(player.surface);

	// load fairies
	for (i = 0; i < num_fairies; i++) {
		fairies[i].surface = IMG_Load("assets/fairy.png");
		fairies[i].tex = SDL_CreateTextureFromSurface(rend,
				fairies[i].surface);
		SDL_FreeSurface(fairies[i].surface);
	}

	// load bullets
	SDL_Surface *surface = IMG_Load("assets/bullet_snow.png");
	SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);

	
	/* positioning hitboxes */

	double scale = 1; 
	// create object
	SDL_QueryTexture(player.tex, NULL, NULL, &player.dest.w,
				&player.dest.h);
	player.dest.w /= scale;
	player.dest.h /= scale;
	player.x = player.dest.x = SCREEN_WIDTH / 2.;
	player.y = player.dest.y = SCREEN_HEIGHT / 2.;
	player.dx = 3.;
	player.dy = 3.;
	player.radius = 2.;

	// "snowball" bullets
	SDL_Rect dest[NUM_BULLETS];
	double snowball_width = 8.;
	double snowball_height = 8.;
	
	for (i = 0; i < NUM_BULLETS; i++) {
		SDL_QueryTexture(tex, NULL, NULL, &dest[i].w, &dest[i].h);
		dest[i].w /= scale;
		dest[i].h /= scale;
		dest[i].x = SCREEN_WIDTH / 2;
		dest[i].y = SCREEN_HEIGHT * (1. / 4);
	}

	
	/* music loader */
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_Music *music;
	music = Mix_LoadMUS("assets/02.mp3");

	if (!music)
		printf("ERROR loading music: %d\n", Mix_GetError());
	Mix_PlayMusic(music, -2);


	/* numerical setup */
	
	// set bullet speeds
	double speed[NUM_BULLETS];
	for (i = 0; i < NUM_BULLETS; i++) 
		speed[i] = (double) i / 50;
	
	// create double version of dest for subpixel precision
	double d_dest[NUM_BULLETS][2];
	double angles[NUM_BULLETS];
	for (i = 0; i < NUM_BULLETS; i++) {
		d_dest[i][D_RECT_X] = (double)dest[i].x;
		d_dest[i][D_RECT_Y] = (double)dest[i].y;
		angles[i] = i * GOLDEN_RATIO;
	}
	
	// variables for bullet movement
	double r;
	double phi = 0.;
	double a = 1;
	double moving = 1;
	
	// player speed
	// https://en.touhouwiki.net/wiki/User:Arcorann/Character_Speeds#Massive_chart
	double reimu_default_speed = 4.5;
	const double focus_factor = 2. / 4.5;
	double marisa_speed = reimu_default_speed * 1.5;
	// for lshift focus
	double factored_speed = reimu_default_speed;
	double diagonal;


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

	while (!close) {
		//		SDL_EnableKeyRepeat(0, 0);
		// questions/1252976

		const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);

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
				factored_speed = focus_factor * reimu_default_speed;
			else
				factored_speed = reimu_default_speed;
			diagonal = factored_speed * (INVERSE_SQRT_2 - 1);

			// non-diagonal movement
			if (keyboard_states[SDL_SCANCODE_LEFT])
				player.x -= factored_speed;
			if (keyboard_states[SDL_SCANCODE_DOWN])
				player.y += factored_speed;
			if (keyboard_states[SDL_SCANCODE_UP])
				player.y -= factored_speed;
			if (keyboard_states[SDL_SCANCODE_RIGHT])
				player.x += factored_speed;

			// diagonal movement
			if (keyboard_states[SDL_SCANCODE_LEFT] &&
					keyboard_states[SDL_SCANCODE_DOWN]) {
					player.x -= diagonal;
					player.y += diagonal;
			}
			if (keyboard_states[SDL_SCANCODE_LEFT] &&
					keyboard_states[SDL_SCANCODE_UP]) {
					player.x -= diagonal;
					player.y -= diagonal;
			}
			if (keyboard_states[SDL_SCANCODE_RIGHT] &&
					keyboard_states[SDL_SCANCODE_DOWN]) {
					player.x += diagonal;
					player.y += diagonal;
			}
			if (keyboard_states[SDL_SCANCODE_RIGHT] &&
					keyboard_states[SDL_SCANCODE_UP]) {
					player.x += diagonal;
					player.y -= diagonal;
			}

			// very unoptimized player position update

			player.dest.x = (int)(player.x);
			player.dest.y = (int)(player.y);
		}

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

		/* Jellyfish??
		// speed[i] = (double)i / 100;
		for (i = 0; i < moving; i++) {
			d_dest[i][D_RECT_X] += speed[i] * cos(angles[i]);
			d_dest[i][D_RECT_Y] += speed[i] * sin(angles[i]);
			dest[i].x = d_dest[i][D_RECT_X];
			dest[i].y = d_dest[i][D_RECT_Y];

		}
		
		if (moving < NUM_BULLETS)
			moving++;
		*/

		
		/* unoptimized collision detection */

		// todo: snowball hitbox -> circle
		// todo: quadtree hitbox detection
		// questions/21650246/sdl-2-collision-detetection
		// github.com/arpit2297/Collision-Detection-using-Quad-Trees
		

		for (i = 0; i < NUM_BULLETS; i++) {
			if (is_hit(dest[i], player)) {
/*
				printf("%d: HIT\n", i);
				if (i % 2)
					printf("T\n");
*/
			}
		}


		/* appendix */

		// clear screen
		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, player.tex, NULL, &player.dest);
		for (i = 0; i < NUM_BULLETS; i++)
			SDL_RenderCopy(rend, tex, NULL, &dest[i]);
	
		// double buffer
		SDL_RenderPresent(rend);
	}

	/* program termination */
	// delay termination
	Uint32 delay = 0; // delay in ms
	SDL_Delay(delay);

	Mix_FreeMusic(music);
	Mix_Quit();
	// close all windows and quit
	SDL_DestroyWindow(win);
	SDL_Quit();
	
	return 0;
}

