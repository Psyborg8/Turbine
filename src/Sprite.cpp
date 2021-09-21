//================================================================================

#include "Sprite.h"

//--------------------------------------------------------------------------------

#include <filesystem>

#include "System.h"
#include "Debug.h"

//================================================================================

namespace Gfx {
namespace Sprite {

//--------------------------------------------------------------------------------

struct Sprite {
	string name;
	sf::Texture texture;

	void render( Math::Vec2 pos, Math::Vec2 size = Math::Vec2( 1.0, 1.0 ) ) const {
		sf::Sprite sprite( texture );

		sprite.setPosition( pos.x, pos.y );
		sprite.scale( size.x, size.y );

		System::getWindow()->draw( sprite );
		Debug::incDrawCall();
	}
};

//--------------------------------------------------------------------------------

// Get pointer to sprite loaded in memory
Sprite getSprite( string name );

//--------------------------------------------------------------------------------

vector< Sprite > sprites;

//================================================================================

void loadSprite( string name, string folder ) {
	Sprite sprite;
	sprite.name = name;

	const string path = folder + name + ".png";
	Debug::addMessage( "Loading Sprite: " + path );
	
	if( std::filesystem::exists( std::filesystem::path( path ) ) ) {
		Debug::addMessage( "Sprite path " + path + " doesn't exist.", Debug::DebugType::Error );
		return;
	}

	sprite.texture.loadFromFile( path );

	sprites.push_back( sprite );
}

//--------------------------------------------------------------------------------

void unloadSprite( string name ) {
	Debug::addMessage( "Unloading Sprite: " + name );

	const auto it = std::find_if( sprites.begin(), sprites.end(),
								  [name]( const Sprite& sprite ) {
									  return sprite.name == name;
								  } );
	if( it == sprites.end() )
		return;

	sprites.erase( it );
}

//--------------------------------------------------------------------------------

void renderSprite( string name, Math::Vec2 pos, Math::Vec2 scale ) {
	const Sprite& sprite = getSprite( name );

	sprite.render( pos, scale );
}

//--------------------------------------------------------------------------------

sf::Texture getTexture( string name ) {
	Sprite sprite = getSprite( name );

	return sprite.texture;
}

//================================================================================

Sprite getSprite( string name ) {
	const auto it = std::find_if( sprites.begin(), sprites.end(),
								  [name]( const Sprite& sprite ) {
									  return sprite.name == name;
								  } );
	if( it == sprites.end() )
		return Sprite();

	return *it;
}

//================================================================================

} // Sprite
} // Gfx

//================================================================================