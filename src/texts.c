/*
 * texts.c includes tools for ingame dialogue, as well as string manip for code.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */

#include "texts.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "render.h"

/* GLOBALS */
TTF_Font *fontlist[MAX_FONT];
SDL_Rect glyphs[MAX_FONT][NUM_GLYPHS];
SDL_Texture *font_textures[MAX_FONT];

struct text text_reimu;
struct text text_marisa;
struct text text_main_menu_pointer;

/* FUNCTIONS */

// UNUSED: join twos trings into one string
char *_concat_2(const char *s1, const char *s2)
{
	// https://stackoverflow.com/questions/8465006/ddg#8465083
	//
	// +1 for the null-terminator
	char *result = malloc(strlen(s1) + strlen(s2) + 1);
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

// join multiple strings into one string
// count: number of strings to parse
// ...: provided strings
char* concat(int count, ...)
{
	// https://stackoverflow.com/posts/11394336/revisions
	va_list ap;
	int i;

	// Find required length to store merged string
	int len = 1; // room for NULL
	va_start(ap, count);
	for(i = 0; i < count ; i++)
		len += strlen(va_arg(ap, char*));
	va_end(ap);

	// Allocate memory to concat strings
	char *merged = calloc(sizeof(char), len);
	int null_pos = 0;

	// Actually concatenate strings
	va_start(ap, count);
	for(i = 0; i < count; i++) {
		char *s = va_arg(ap, char*);
		strcpy(merged+null_pos, s);
		null_pos += strlen(s);
	}
	va_end(ap);

	return merged;
}

// exits program if SDL initialization fails, prints error
void check_ttf_initialization()
{
	// https://thenumb.at/cpp-course/sdl2/07/07.html
	if (TTF_Init() < 0) {
		printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
		exit(1);
	}
}

// initializes fonts for game
void initialize_ttf()
{
	// https://thenumb.at/cpp-course/sdl2/07/07.html
	check_ttf_initialization();
	
	// initiates list of fonts
	fontlist[0] = TTF_OpenFont("assets/fonts/DejaVuSans.ttf", 24);
	
	/* text fields */
	
	// sets up values to texts
	text_reimu.color.r = 255; // set to white
	text_reimu.color.g = 255;
	text_reimu.color.b = 255;
	text_reimu.font = fontlist[0];
	text_reimu.string = "Reimu (Press 1)";
	
	text_marisa.color.r = 255; // set to white
	text_marisa.color.g = 255;
	text_marisa.color.b = 255;
	text_marisa.font = fontlist[0];
	text_marisa.string = "Marisa (Press 2)";
	
	text_main_menu_pointer.color.r = 128;
	text_main_menu_pointer.color.g = 0;
	text_main_menu_pointer.color.b = 0;
	text_main_menu_pointer.font = fontlist[0];
	text_main_menu_pointer.string = ">";
	
	
}

void draw_text(struct text t, int xcoord, int ycoord)
{
	// https://thenumb.at/cpp-course/sdl2/07/07.html
	SDL_Surface* text_surf = TTF_RenderText_Solid(t.font, t.string, t.color);
	if (!text_surf)
		printf("Failed to render text: %s\n", TTF_GetError());
	text_reimu.sdl.texture = SDL_CreateTextureFromSurface(rend, text_surf);
	text_reimu.sdl.rect.x = xcoord;
	text_reimu.sdl.rect.y = ycoord;
	text_reimu.sdl.rect.w = text_surf -> w;
	text_reimu.sdl.rect.h = text_surf -> h;
		
	SDL_RenderCopy(rend, text_reimu.sdl.texture, NULL, &text_reimu.sdl.rect);
	SDL_FreeSurface(text_surf);
}

/*
// 
void draw_text(char *text, int x, int y, int r, int g, int b, int font_type)
{
	// https://www.parallelrealities.co.uk/tutorials/ttf/ttf2.php
	int i = 0;
	int character;
	SDL_Rect *glyph, dest;

	SDL_SetTextureColorMod(font_textures[font_type], r, g, b);

	// TODO: change this to do ... while?
	character = text[i++];

	// drawing each individual character (glyph)
	while (character) {
		glyph = &glyphs[font_type][character];

		dest.x = x;
		dest.y = y;
		dest.w = glyph -> w;
		dest.h = glyph -> h;

		SDL_RenderCopy(rend, font_textures[font_type], glyph, &dest);

		x += glyph -> w;

		character = text[i++];
	}
}
*/

/*
// https://stackoverflow.com/questions/22886500/ddg#22889483
void tmp()
{
	//this opens a font style and sets a size
	TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);

	// this is the color in rgb format,
	// maxing out all would give you the color white,
	// and it will be your text's color
	SDL_Color White = {255, 255, 255};

	// as TTF_RenderText_Solid could only be used on
	// SDL_Surface then you have to create the surface first
	SDL_Surface* surfaceMessage =
		TTF_RenderText_Solid(Sans, "put your text here", White); 

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = 0;  //controls the rect's x coordinate 
	Message_rect.y = 0; // controls the rect's y coordinte
	Message_rect.w = 100; // controls the width of the rect
	Message_rect.h = 100; // controls the height of the rect

	// (0,0) is on the top left of the window/screen,
	// think a rect as the text's box,
	// that way it would be very simple to understand

	// Now since it's a texture, you have to put RenderCopy
	// in your game loop area, the area where the whole code executes

	// you put the renderer's name first, the Message,
	// the crop size (you can ignore this if you don't want
	// to dabble with cropping), and the rect which is the size
	// and coordinate of your texture
	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	// Don't forget to free your surface and texture
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}
*/


