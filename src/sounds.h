#pragma once
#include <SDL2/SDL_mixer.h>
// #include "texts.h"

void init_sounds()
{
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
}


void play_track(char *title)
{
	Mix_Music *music;

	// TODO: allow to choose track
	char *file = "assets/02.mp3";
	music = Mix_LoadMUS(file);

	// terminate program on mixer error
	if (!music) {
		printf("ERROR loading music: %d\n", Mix_GetError());
		exit(1);
	}
	
	Mix_PlayMusic(music, -2);
}


// loop

