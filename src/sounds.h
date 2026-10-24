/*
 * sounds.h is the headerfile for sounds.c.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */

#ifndef SOUNDS_H
#define SOUNDS_H

// includes
#include <SDL2/SDL_mixer.h>

// macros
#define MAX_SONGS 3

// globals
extern Mix_Music *song;

// functions
void initialize_sounds();
void play_track_title(const char *title);
void play_sfx(char *title);
void do_sounds();
void terminate_sounds();

#endif /* SOUNDS_H */
