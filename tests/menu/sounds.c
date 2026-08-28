/*
 * sounds.c encompasses music and sound effects.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */

#include <SDL2/SDL_mixer.h>

#include "sounds.h"
#include "texts.h"


/* VARIABLES */

Mix_Music *song;


/* FUNCTIONS */

void initialize_sounds()
{
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
}

void play_track(char *title)
{

	// TODO: allow to choose track
	char *file = concat(3, "assets/", title, ".mp3");
	song = Mix_LoadMUS(file);

	// terminate program on mixer error
	if (!song) {
		printf("ERROR loading music: %s\n", Mix_GetError());
		exit(1);
	}

	Mix_PlayMusic(song, -2);
}

void terminate_sounds()
{
	Mix_FreeMusic(song);
	Mix_Quit();
}

