//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"

//================================================================================

namespace Gfx {
namespace Tileset {

//--------------------------------------------------------------------------------

struct Tilemap {
	vector< int > indices;
	Math::Vec2 tileSize;
	Math::Vec2 mapSize;
};

//--------------------------------------------------------------------------------

// Load the tileset from file into memory
void loadTileset( string name, Math::Vec2 tileSize, string path );

// Unload tileset from memory
void unloadTileset( string name );

// Draw tile to render target
void renderTile( string name, size_t index, sf::RenderTarget* target, Math::Vec2 pos = Math::Vec2(), Math::Vec2 scale = Math::Vec2( 1.0f, 1.0f ) );

//--------------------------------------------------------------------------------

} // Tileset
} // Gfx

//================================================================================