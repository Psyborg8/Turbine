//================================================================================

#include "TilesetLoader.h"

//================================================================================

namespace Gfx {
namespace Tileset {

//--------------------------------------------------------------------------------

struct Tileset {
	string name;
	vector< sf::Texture > texture;

	size_t length;
	Math::Vec2 tileSize;

	void render( size_t index, Math::Vec2 pos, Math::Vec2 size = Math::Vec2( 1.0, 1.0 ) ) const {
		sf::Sprite sprite( texture.at( index ) );
	}
};

//--------------------------------------------------------------------------------

Tileset getTileset( string name );

//--------------------------------------------------------------------------------

vector< Tileset > tilesets;

//================================================================================

void loadTileset( string name, string folder ) {
	Tileset tileset = getTileset( name );
	
	if( tileset.name == name )
		return;

	tileset.name = name;
	
	sf::Texture texture = 
}

//================================================================================

} // Tileset
} // Gfx

//================================================================================