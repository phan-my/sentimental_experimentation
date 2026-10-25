/*
 * texts.h is the headerfile for texts.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef TEXTS_H
#define TEXTS_H

// includes
#include <SDL2/SDL_ttf.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "logic.h"

// macros
#define MAX_FONT 1
#define NUM_GLYPHS 1024

// structs

// for ttf functionality
struct text {
	// render
	struct sdl_types sdl;
	
	double x;
	double y;
	
	// formal requirements
	TTF_Font *font;
	SDL_Color color;
	const char *string;
};


// globals
extern TTF_Font *fonts[MAX_FONT];
extern SDL_Rect glyphs[MAX_FONT][NUM_GLYPHS];
extern SDL_Texture *fontTextures[MAX_FONT];
extern struct text text_reimu;
extern struct text text_marisa;
extern struct text text_main_menu_pointer;

// functions
char *_concat_2(const char *s1, const char *s2);
char *concat(int count, ...);
void initialize_ttf();
void draw_text(struct text t, int xcoord, int ycoord);
// void draw_text(char *text, int x, int y, int r, int g, int b, int font_type);

#endif /* TEXTS_H */
