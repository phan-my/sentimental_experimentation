/*
 * render.h is the headerfile for render.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef RENDER_H
#define RENDER_H

// include
#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "logic.h"

// macros
#define WINDOW_TITLE "極東実験情　～ Sentimental Experimentation"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define FIELD_WIDTH 384		// (480x560 | 384x448)
#define FIELD_HEIGHT 448

#define FIELD_OFFSET_X 32
#define FIELD_OFFSET_Y 16

// globals
extern SDL_Window *screen;
extern SDL_Renderer *rend;
extern struct ball ball_8x8[MAX_BULLETS];
extern struct enemy fairies[MAX_FAIRIES];

// functions
void check_sdl_init(void);
void initialize_screen(void);
void initialize_textures(void);
void terminate_screen(void);
SDL_Texture *create_texture(char *sprite);

#endif /* RENDER_H */
