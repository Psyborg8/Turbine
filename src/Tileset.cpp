//================================================================================

#include "Tileset.h"

//--------------------------------------------------------------------------------

#include <filesystem>
#include "System.h"

//================================================================================

namespace Gfx {
namespace Tileset {

struct Tile {
	int id;
	shared_ptr< sf::Sprite > sprite;
};

//--------------------------------------------------------------------------------

struct Tileset {
	string name;
	shared_ptr< sf::Texture > texture;
	vector< Tile > tiles;
};

//--------------------------------------------------------------------------------

Tileset getTileset( string name );

//--------------------------------------------------------------------------------

vector< Tileset > tilesets;

//================================================================================

void loadTileset( string name, sf::Vector2u tileSize, string path ) {
	Tileset tileset = getTileset( name );
	
	if( tileset.name == name )
		return;

	tileset.name = name;

	if( !std::filesystem::exists( std::filesystem::path( path ) ) )
		return;

	tileset.texture = make_shared< sf::Texture >();
	tileset.texture->loadFromFile( path );
	tileset.texture->setSmooth( true );

	sf::Vector2u mapSize;
	mapSize.x = tileset.texture->getSize().x / tileSize.x;
	mapSize.y = tileset.texture->getSize().y / tileSize.y;

	for( size_t y = 0u; y < mapSize.y; ++y )
		for( size_t x = 0u; x < mapSize.x; ++x ) {
			sf::IntRect rect;
			rect.left = int( x * tileSize.x );
			rect.top = int( y * tileSize.y );
			rect.width = int( tileSize.x );
			rect.height = int( tileSize.y );

			Tile tile;

			tile.sprite = std::make_shared< sf::Sprite >( *tileset.texture, rect );
			tile.id = int( y * mapSize.x + x );
			tileset.tiles.push_back( tile );
		}

	tilesets.push_back( tileset );
}

//--------------------------------------------------------------------------------

void unloadTileset( string name ) {
	const auto it = std::find_if( tilesets.begin(), tilesets.end(),
								  [name]( const Tileset& tileset ) {
									  return tileset.name == name;
								  } );
	if( it == tilesets.end() )
		return;

	tilesets.erase( it );
}

//--------------------------------------------------------------------------------

void renderTile( string name, size_t index, sf::RenderTarget* target, Math::Vec2 pos, Math::Vec2 scale ) {
	Tileset tileset = getTileset( name );
	if( tileset.name != name )
		return;

	const Tile& tile = tileset.tiles.at( index );
	tile.sprite->setPosition( pos.sf() );
	target->draw( *tile.sprite );
}

//--------------------------------------------------------------------------------

Tileset getTileset( string name ) {
	const auto it = std::find_if( tilesets.begin(), tilesets.end(),
								  [name]( const Tileset& tileset ) {
									  return tileset.name == name;
								  } );

	if( it == tilesets.end() )
		return Tileset();

	return *it;
}

//================================================================================

} // Tileset
} // Gfx

//================================================================================