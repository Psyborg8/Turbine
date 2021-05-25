//================================================================================

#pragma once

//================================================================================

#include "png.hpp"

#include "MathTypes.h"

//================================================================================

namespace Gfx {
namespace SpriteLoader {

//--------------------------------------------------------------------------------

// Load the sprite from file into memory
void loadSprite( string name, string path );

// Unload sprite from memory
void unloadSprite( string name );

// Draw sprite to screen
void renderSprite( string name, Math::Vec2 pos );

//--------------------------------------------------------------------------------

} // SpriteLoader
} // Gfx

//================================================================================