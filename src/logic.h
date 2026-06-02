#pragma once
#include <SDL2/SDL.h>

// has a hitbox; players, enemies, by extension the boss
struct entity {
	double x;
	double y;
	double dx; // player hitbox diameter, square implementation
	double dy; // ditto
	double radius;
	SDL_Surface *surface; // image loads into a surface
	SDL_Texture *tex;
	SDL_Rect dest; // (x, y) as integers
	int health;
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

// assuming square bullet hitbox
// FIXME: code looks awful
// FIXME: allow hitbox adjustment in y-axis of sprite
// FIXME: optimize
// https://silentmatt.com/rectangle-intersection/
bool is_hit(SDL_Rect dest, struct entity src)
{
	// top-left corner of player hitbox
	double src_x1 = src.x + src.dest.w / 2. - src.radius;
	double src_y1 = src.y + src.dest.h / 2. - src.radius;
	//bottom-left corner of player hitbox
	double src_x2 = src.x + src.dest.w / 2. + src.radius;
	double src_y2 = src.y + src.dest.h / 2. + src.radius;

	double dest_x1 = dest.x;
	double dest_y1 = dest.y;
	double dest_x2 = dest.x + dest.w;
	double dest_y2 = dest.y + dest.h;
	
	if (src_x1 < dest_x2 && src_x2 > dest_x1 && src_y1 < dest_y2 &&
			src_y2 > dest_y1)
		return true;
	else
		return false;
}

// FIXME: create a bullet model for QueryTexture to invoke
void load_bullet_model()
{

}