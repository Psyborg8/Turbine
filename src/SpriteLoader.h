//================================================================================

#pragma once

//================================================================================

#include "png.hpp"

#include "MathTypes.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

struct Sprite {
	string name;
	png::image< png::rgba_pixel > image;

	void render( Math::Vec2 pos );
};

//--------------------------------------------------------------------------------

namespace SpriteLoader {

//--------------------------------------------------------------------------------

// Load the sprite from file into memory
void loadSprite( string name, string path );

// Unload sprite from memory
void unloadSprite( string name );

// Get pointer to sprite loaded in memory
shared_ptr< Sprite > getSprite( string name );

//--------------------------------------------------------------------------------

} // SpriteLoader
} // Gfx

//================================================================================