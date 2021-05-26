//================================================================================

#include "Tileset.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

namespace Gfx {
namespace Tileset {

//--------------------------------------------------------------------------------

struct Tileset {
	string name;
	vector< sf::Texture > texture;

	void render( size_t index, Math::Vec2 pos, Math::Vec2 scale = Math::Vec2( 1.0, 1.0 ) ) const {
		if( index >= texture.size() )
			return;

		const sf::Texture& tex = texture.at( index );
		sf::Sprite sprite( tex );

		sprite.scale( scale.sf() );
		sprite.setPosition( pos.sf() );

		System::getWindow()->draw( sprite );
	}
};

//--------------------------------------------------------------------------------

Tileset getTileset( string name );

//--------------------------------------------------------------------------------

vector< Tileset > tilesets;

//================================================================================

void loadTileset( string name, sf::Vector2u tileSize, string folder ) {
	Tileset tileset = getTileset( name );
	
	if( tileset.name == name )
		return;

	tileset.name = name;

	sf::Texture texture;
	const string path = folder + name + ".png";
	texture.loadFromFile( path );

	sf::Vector2u mapSize;
	mapSize.x = texture.getSize().x / tileSize.x;
	mapSize.y = texture.getSize().y / tileSize.y;

	for( size_t y = 0u; y < mapSize.y; ++y )
		for( size_t x = 0u; x < mapSize.x; ++x ) {
			sf::IntRect rect;
			rect.left = x * tileSize.x;
			rect.top = y * tileSize.y;
			rect.width = tileSize.x;
			rect.height = tileSize.y;

			sf::Texture tile;
			tile.loadFromFile( path, rect );

			tileset.texture.push_back( tile );
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

void renderTile( string name, size_t index, Math::Vec2 pos, Math::Vec2 scale ) {
	Tileset tileset = getTileset( name );
	if( tileset.name != name )
		return;

	tileset.render( index, pos, scale );
}

//--------------------------------------------------------------------------------

void renderTiles( string name, Tilemap map, Math::Vec2 pos ) {
	if( map.indices.empty() )
		return;
	if( map.indices.size() != map.mapSize.x * map.mapSize.y )
		return;

	Tileset tileset = getTileset( name );
	if( tileset.name != name )
		return;

	for( size_t x = 0u; x < map.mapSize.x; ++x )
		for( size_t y = 0u; y < map.mapSize.y; ++y ) {
			Math::Vec2 position = pos;
			position.x += x * map.tileSize.x;
			position.y += y * map.tileSize.y;

			tileset.render( map.indices.at( y * map.mapSize.x + x ), position );
		}
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