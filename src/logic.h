#pragma once
#include <SDL2/SDL.h>
#include <math.h>

struct circlebox {
	double r;
	double x; // hitbox core position
	double y; // ditto
};

struct squarebox {
	double l;
	double x;
	double y;
};

// experimental
struct _ellipsebox {
	double eccentricity;
	double major;
	double minor;
	double angle; // in radians
};

struct sdl_types {
	SDL_Surface *surface; // image loads into a surface
	SDL_Texture *texture;
	SDL_Rect rect; // full sprite
};

// has a hitbox; players, enemies, by extension the boss
struct player {
	struct circlebox hitbox;
	struct sdl_types sdl;
	int health;
	double level;
	double speed;
	double diagonal;
};

struct position {
	double x;
	double y;
};

struct entity {
	struct position hitbox;
	struct sdl_types sdl;
};

struct enemy {
	struct circlebox hitbox;
	struct sdl_types sdl;
	int health;
};

struct ball {
	struct circlebox hitbox;
	struct sdl_types sdl;
};

/*
bool in_open(double num, double lower, double upper)
{
	if (lower < num && num < upper)
		return true;
	else
		return false;
}

// returns top-left of the hitbox as a dest
double topleft(double core, double radius)
{
	return core - radius;
}

double bottomright(double core, double radius)
{
	return core + radius;
}
*/

// assuming circle bullet hitbox
// FIXME: code looks awful
// FIXME: allow hitbox adjustment in y-axis of sprite
// FIXME: Pythagorean expression optimizable?
// https://silentmatt.com/rectangle-intersection/
// basic circle-circle collision
// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
bool is_hit(struct circlebox dest, struct circlebox src)
{
	double dx = src.x + src.r - (dest.x + dest.r);
	double dy = src.y + src.r - (dest.y + dest.r);
	double distance = sqrt(dx * dx + dy * dy);
	return distance < src.r + dest.r;
}

// set sdl int position to match the float position
void update_ball_position(struct ball *p)
{
	p -> sdl.rect.x = (int)(p -> hitbox.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> hitbox.y - p -> sdl.rect.h / 2.);
}

// set sdl int position to match the float position
void update_player_position(struct player *p)
{
	p -> sdl.rect.x = (int)(p -> hitbox.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> hitbox.y - p -> sdl.rect.h / 2.);
}

// set sdl int position to match the float position
void update_enemy_position(struct enemy *p)
{
	p -> sdl.rect.x = (int)(p -> hitbox.x - p -> sdl.rect.w / 2.);
	p -> sdl.rect.y = (int)(p -> hitbox.y - p -> sdl.rect.h / 2.);
}

/*
bool is_hit(struct player p, struct bullet b)
{

}
*/

// FIXME: create a bullet model for QueryTexture to invoke
void load_bullet_model()
{

}

