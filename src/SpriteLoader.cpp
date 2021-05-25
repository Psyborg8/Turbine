//================================================================================

#include "SpriteLoader.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

void Sprite::render( Math::Vec2 pos ) {
	
	const png::pixel_buffer< png::rgba_pixel >& buffer = image.get_pixbuf();

	glRasterPos2d( pos.x, pos.y );
	glDrawPixels( image.get_width(), image.get_height(), GL_RGBA, GL_BYTE, &( buffer ) );
}

//--------------------------------------------------------------------------------

namespace SpriteLoader {

//================================================================================

vector< shared_ptr< Sprite > > sprites;

//================================================================================

void loadSprite( string path, string name ) {

	const string sPath = Folders::Sprites + path + ".png";

	// Replace the image data if it already exists
	shared_ptr< Sprite > ptr = getSprite( name );
	if( ptr != nullptr ) {
		png::image< png::rgba_pixel > image{ sPath };
		if( image.get_width() && image.get_height() )
			ptr->image = image;
		return;
	}

	// Load image from path
	Sprite sprite;
	sprite.image = png::image< png::rgba_pixel >( sPath );
	
	if( !sprite.image.get_width() || !sprite.image.get_height() )
		return;

	sprite.name = name;

	sprites.push_back( make_shared< Sprite >( sprite ) );
}

//--------------------------------------------------------------------------------

void unloadSprite( string name ) {
	const auto it = std::find_if( sprites.begin(), sprites.end(),
								  [name]( shared_ptr< Sprite > sprite ) {
									  return sprite->name == name;
								  } );
	if( it == sprites.end() )
		return;

	// Clear image data
	( *it )->image = png::image< png::rgba_pixel >();
	sprites.erase( it );
}

//--------------------------------------------------------------------------------

shared_ptr< Sprite > getSprite( string name ) {
	const auto it = std::find_if( sprites.begin(), sprites.end(),
								  [name]( shared_ptr< Sprite > sprite ) {
									  return sprite->name == name;
								  } );
	if( it == sprites.end() )
		return nullptr;

	return *it;
}

//================================================================================

} // SpriteLoader
} // Gfx

//================================================================================