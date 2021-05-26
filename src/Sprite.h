//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"

//================================================================================

namespace Gfx {
namespace Sprite {

//--------------------------------------------------------------------------------

// Load the sprite from file into memory
void loadSprite( string name, string folder = Folders::Sprites );

// Unload sprite from memory
void unloadSprite( string name );

// Draw sprite to screen
void renderSprite( string name, Math::Vec2 pos, Math::Vec2 scale = Math::Vec2( 1.0, 1.0 ) );

// Get the SFML texture object
sf::Texture getTexture( string name );

//--------------------------------------------------------------------------------

} // Sprite
} // Gfx

//================================================================================