//================================================================================

#include "SpriteLoader.h"

//--------------------------------------------------------------------------------

#include <filesystem>

//================================================================================

namespace Gfx {
namespace SpriteLoader {

//--------------------------------------------------------------------------------

struct Sprite {
	string name;
	shared_ptr< png::image< png::rgba_pixel > > image;

	void render( Math::Vec2 pos ) const {

		if( image == nullptr )
			return;

		// Construct image data
		size_t idx = 0u;
		Byte* buffer = new Byte[ image->get_width() * image->get_height() * 4 ];
		for( size_t y = 0u; y < image->get_width(); ++y )
			for( size_t x = 0u; x < image->get_height(); ++x )
			{
				png::rgba_pixel pixel = image->get_pixel( x, y );
				buffer[ idx++ ] = pixel.red;
				buffer[ idx++ ] = pixel.green;
				buffer[ idx++ ] = pixel.blue;
				buffer[ idx++ ] = pixel.alpha;
			}

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glRasterPos2d( pos.x, pos.y );
		glPixelZoom( 3.0f, 3.0f );
		glDrawPixels( image->get_width(), image->get_height(), GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	}
};

//--------------------------------------------------------------------------------

// Get pointer to sprite loaded in memory
Sprite getSprite( string name );

//--------------------------------------------------------------------------------

vector< Sprite > sprites;

//================================================================================

void loadSprite( string path, string name ) {

	std::filesystem::path sPath{ Folders::Sprites + name + ".png" };

	if( !std::filesystem::exists( sPath ) )
		return;

	// Load image from path
	Sprite sprite;
	sprite.image = make_shared< png::image< png::rgba_pixel > >( sPath.string() );

	if( sprite.image == nullptr )
		return;

	// Replace the image data if it already exists
	Sprite tmp = getSprite( name );
	if( tmp.image != nullptr ) {
		tmp.image = sprite.image;
		return;
	}

	sprite.name = name;

	sprites.push_back( sprite );
}

//--------------------------------------------------------------------------------

void unloadSprite( string name ) {
	const auto it = std::find_if( sprites.begin(), sprites.end(),
								  [name]( const Sprite& sprite ) {
									  return sprite.name == name;
								  } );
	if( it == sprites.end() )
		return;

	// Clear image data
	it->image = nullptr;
	sprites.erase( it );
}

//--------------------------------------------------------------------------------

void renderSprite( string name, Math::Vec2 pos ) {
	const Sprite& sprite = getSprite( name );
	if( sprite.image == nullptr )
		return;

	sprite.render( pos );
}

//--------------------------------------------------------------------------------

Sprite getSprite( string name ) {
	const auto it = std::find_if( sprites.begin(), sprites.end(),
								  [name]( const Sprite& sprite ) {
									  return sprite.name == name;
								  } );
	if( it == sprites.end() )
		return Sprite();

	if( it->image == nullptr )
		return Sprite();

	return *it;
}

//================================================================================

} // SpriteLoader
} // Gfx

//================================================================================