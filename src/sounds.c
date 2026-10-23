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
Mix_Chunk *sound;


/* FUNCTIONS */

// opens mixer
void initialize_sounds()
{
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
}

// plays the given audio file on repeat
// *title: name of audio file in directory assets/
void play_track(char *title)
{
	char *file = concat(2, "assets/", title);
	song = Mix_LoadMUS(file);

	// terminate program on mixer error
	if (!song) {
		printf("ERROR loading music: %s\n", Mix_GetError());
		exit(1);
	}

	Mix_PlayMusic(song, -2);
}

// plays the given sfx file on repeat
// *title: name of sfx file in directory assets/
void play_sfx(char *title)
{
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	char *file = concat(2, "assets/", title);
	sound = Mix_LoadWAV(file);
	if (!sound) {
		printf("Error loading sound: %s\n", Mix_GetError());
		exit(1);
	}
	if (strcmp(title, "sfx/graze.ogx") == 0)
		sound -> volume = 36;
	else
		sound -> volume = 128;
	int channel = Mix_PlayChannel(-1, sound, 0);
}

// terminates music and quits mixer
void terminate_sounds()
{
	Mix_FreeMusic(song);
	Mix_Quit();
}

