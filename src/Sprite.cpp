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

vector< sf::Texture > textures;
map< string, ID > textureMap;

//--------------------------------------------------------------------------------

ID get( string path ) {
	if( textureMap.count( path ) )
		return textureMap[ path ];

	textures.push_back( sf::Texture() );
	if( !textures.rbegin()->loadFromFile( path ) )
		Debug::addMessage( "Texture " + path + " doesn't exist", DebugType::Error );

	ID id = textures.size() - 1u;
	textureMap[ path ] = id;
	return id;
}

//--------------------------------------------------------------------------------

sf::Texture& get( ID id ) {
	if( textures.size() <= id )
		return textures.at( 0 );

	return textures.at( id );
}

//================================================================================

} // Sprite
} // Gfx

//================================================================================