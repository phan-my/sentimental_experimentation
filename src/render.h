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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// macros
#define WINDOW_TITLE "極東実験情　～ Sentimental Experimentation"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define FIELD_WIDTH 384		// (480x560 | 384x448)
#define FIELD_HEIGHT 448

// globals
extern SDL_Window *screen;
extern SDL_Renderer *rend;

// functions
void check_sdl_init(void);
void initialize_screen(void);
void terminate_screen(void);
SDL_Texture *create_texture(char *sprite);

#endif /* RENDER_H */
