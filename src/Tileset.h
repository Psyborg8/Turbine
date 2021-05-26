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
	sf::Vector2u tileSize;
	sf::Vector2u mapSize;
};

//--------------------------------------------------------------------------------

// Load the tileset from file into memory
void loadTileset( string name, sf::Vector2u tileSize, string folder = Folders::Tilesets );

// Unload tileset from memory
void unloadTileset( string name );

// Draw tile to screen
void renderTile( string name, size_t index, Math::Vec2 pos = Math::Vec2(), Math::Vec2 scale = Math::Vec2( 1.0, 1.0 ) );

// Draw tiles to screen
void renderTiles( string name, Tilemap map, Math::Vec2 pos = Math::Vec2() );

//--------------------------------------------------------------------------------

} // Tileset
} // Gfx

//================================================================================