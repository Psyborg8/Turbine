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

map< string, sf::Texture > textures;

//--------------------------------------------------------------------------------

sf::Texture& get( string path ) {
	if( !textures.count( path ) )
		if( !textures[ path ].loadFromFile( path ) )
			Debug::addMessage( "Texture " + path + " doesn't exist", DebugType::Error );

	return textures.at( path );
}

//================================================================================

} // Sprite
} // Gfx

//================================================================================