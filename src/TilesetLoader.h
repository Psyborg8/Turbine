//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"

//================================================================================

namespace Gfx {
namespace TilesetLoader {

//--------------------------------------------------------------------------------

// Load the sprite from file into memory
void loadTileset( string name, Math::Vec2 size, string folder = Folders::Sprites );

// Unload sprite from memory
void unloadTileset( string name );

// Draw sprite to screen
void renderTile( string name, size_t index, Math::Vec2 pos, Math::Vec2 scale = Math::Vec2( 1.0, 1.0 ) );

//--------------------------------------------------------------------------------

} // Tileset
} // Gfx

//================================================================================