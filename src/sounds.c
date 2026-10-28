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
#include "menu.h"
#include "texts.h"
#include "stage.h"
#include "collision.h"


/* VARIABLES */

Mix_Music *song;
Mix_Chunk *sound;

const char *songlist[MAX_SONGS];
int current_song;

/* FUNCTIONS */

// plays the given audio file on repeat
// *title: name of audio file in directory assets/
void play_track_title(const char *title)
{
	const char *file = concat(2, "assets/", title);
	song = Mix_LoadMUS(file);

	// terminate program on mixer error
	if (!song) {
		printf("ERROR loading music: %s\n", Mix_GetError());
		exit(1);
	}

	Mix_PlayMusic(song, -2);
}

// plays a song in songlist with given index
// n: tracklist, starts at 0
int play_track(int n)
{
	play_track_title(songlist[n]);
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

// update current_song according to context
void update_track()
{
	if (is_boss)
		current_song = 1 + current_stage + 1;
	else
		current_song = 1 + current_stage;
}

// opens mixer
void initialize_sounds()
{
	// https://thenumb.at/cpp-course/sdl2/06/06.html#mixer
	int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	current_song = 0;
	songlist[0] = "bgm/01_title.opus";
	songlist[1] = "bgm/02_stage1.opus";
	songlist[2] = "bgm/03_stage1boss.opus";

	play_track(0); // title screen
}

// plays sounds according to context
void do_sounds()
{
	/* music */
	switch (state_menu) {
	case STATE_LOADING:
		update_track();
		play_track(current_song);
		break;
	default:
		break;
	}
	
	/* sfx */
	int i, j;

	// loop through entire enemylist
	// FIXME: player invincibility takes precedence over sfx
/*
	for (i = 0; i < MAX_ENEMIES; i++) {
	for (j = 0; j < MAX_ENEMY_MULTIPLE; j++) {
		// checks collision
		if (circle_in_circle(enemylist[i][j].motion.hb,
					player[curr_player].motion.hb)
				&& !player[curr_player].invincible
				&& enemylist[i][j].active) {
			play_sfx("sfx/death.ogx"); // death sound
		}
	
		// grazing
		if (circle_in_circle(enemylist[i][j].motion.hb,
					player[curr_player].grazebox)
				&& enemylist[i][j].active
				&& !player[curr_player].invincible
				&& !enemylist[i][j].grazed) {
			play_sfx("sfx/graze.ogx"); // graze sound
		}
	}
	}

	// loop through entire bulletlist
	for (i = 0; i < MAX_BULLETS; i++) {
	for (j = 0; j < MAX_BULLET_MULTIPLE; j++) {
		// collision
		if (circle_in_circle(bulletlist[i][j].motion.hb,
					player[curr_player].motion.hb)
				&& !player[curr_player].invincible
				&& bulletlist[i][j].active) {
			play_sfx("sfx/death.ogx"); // death sound
		}

		// grazing
		if (circle_in_circle(bulletlist[i][j].motion.hb,
					player[curr_player].grazebox)
				&& bulletlist[i][j].active
				&& !player[curr_player].invincible
				&& !bulletlist[i][j].grazed) {
			play_sfx("sfx/graze.ogx"); // graze sound
		}
	}
	}
*/
}

// terminates music and quits mixer
void terminate_sounds()
{
	Mix_FreeMusic(song);
	Mix_Quit();
}

