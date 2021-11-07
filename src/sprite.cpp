//================================================================================

#include "sprite.h"

//--------------------------------------------------------------------------------

#include <filesystem>

#include "system.h"
#include "debug.h"

//================================================================================

namespace Gfx {
namespace Sprite {

//--------------------------------------------------------------------------------

struct Texture {
	string path;
	size_t ID;
	sf::Texture texture;
};

//--------------------------------------------------------------------------------

vector< Texture > textures;
sf::Texture defaultTexture;

//--------------------------------------------------------------------------------

ID get( string path ) {
	auto it = std::find_if( textures.begin(), textures.end(),
								  [path] ( const Texture& rh ) {
									  return rh.path == path;
								  } );
	if( it == textures.end() ) {
		Texture texture;
		texture.path = path;
		texture.ID = textures.size();
		if( !texture.texture.loadFromFile( path ) ) {
			Debug::addMessage( "Texture " + path + " failed to load", DebugType::Error );
			texture.texture = defaultTexture;
		}
		
		textures.push_back( texture );
		it = textures.end() - 1u;
	}

	return it->ID;
}

//--------------------------------------------------------------------------------

sf::Texture& get( ID id ) {
	const auto it = std::find_if( textures.begin(), textures.end(),
								  [id]( const Texture& rh ) {
									  return rh.ID == id;
								  } );
	if( it == textures.end() )
		return defaultTexture;

	return it->texture;
}

//================================================================================

} // Sprite
} // Gfx

//================================================================================