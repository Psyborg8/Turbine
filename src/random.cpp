//================================================================================

#include "random.h"

//--------------------------------------------------------------------------------

#include <cstdlib>

//================================================================================

namespace Random {

//--------------------------------------------------------------------------------

int seed{ 0 };

//================================================================================

int getInt( int min, int max ) {
	if( min == max )
		return min;

	int out = rand();
	out %= ( max - min );
	out += min;

	return out;
}

//--------------------------------------------------------------------------------

int getInt( int min, int max, function< int( int ) > func ) {
	if( min == max )
		return func( min );

	int out = getInt( min, max );
	out		= func( out );

	return out;
}

//--------------------------------------------------------------------------------


RandomNumbers< int > getInt( int min, int max, size_t count ) {
	RandomNumbers< int > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getInt( min, max ) );

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< int > getInt( int min, int max, size_t count, function< int( int ) > func ) {
	RandomNumbers< int > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getInt( min, max, func ) );

	return out;
}

//--------------------------------------------------------------------------------

float getFloat( float min, float max ) {
	if( min == max )
		return min;

	float out = static_cast< float >( rand() );
	out /= static_cast< float >( RAND_MAX ) / ( max - min );
	out += min;

	return out;
}

//--------------------------------------------------------------------------------

float getFloat( float min, float max, function< float( float ) > func ) {
	if( min == max )
		return func( min );

	float out = getFloat( min, max );
	out		  = func( out );

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< float > getFloat( float min, float max, size_t count ) {
	RandomNumbers< float > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getFloat( min, max ) );

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< float >
	getFloat( float min, float max, size_t count, function< float( float ) > func ) {
	RandomNumbers< float > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getFloat( min, max, func ) );

	return out;
}

//--------------------------------------------------------------------------------

Math::Vec2 getVec2( Math::Vec2 min, Math::Vec2 max ) {
	if( min == max )
		return min;

	const float x = getFloat( min.x, max.x );
	const float y = getFloat( min.y, max.y );

	return Math::Vec2( x, y );
}

//--------------------------------------------------------------------------------

Math::Color getColor( Math::Color a, Math::Color b, RandomColorType type ) {
	if( a == b )
		return a;

	Math::Color out;
	if( type == RandomColorType::ShuffleRGB ) {
		out.r = getFloat( a.r, b.r );
		out.g = getFloat( a.g, b.g );
		out.b = getFloat( a.b, b.b );
		out.a = getFloat( a.a, b.a );
		return out;
	}

	if( type == RandomColorType::MixRGB ) {
		const float alpha = getFloat( 0.0f, 1.0f );
		out.r			  = a.r + ( b.r - a.r ) * alpha;
		out.g			  = a.g + ( b.g - a.g ) * alpha;
		out.b			  = a.b + ( b.b - a.b ) * alpha;
		out.a			  = a.a + ( b.a - a.a ) * alpha;
		return out;
	}

	if( type == RandomColorType::ShuffleHSV ) {
		float hA = a.hue();
		float hB = b.hue();
		if( std::isnan( hA ) )
			hA = hB;
		if( std::isnan( hB ) )
			hB = hA;

		const float sA = a.saturation();
		const float sB = b.saturation();

		const float bA = a.brightness();
		const float bB = b.brightness();

		float hO = getFloat( hA, hB );
		if( std::isnan( hO ) )
			hO = 0.0f;

		const float sO = getFloat( sA, sB );
		const float bO = getFloat( bA, bB );
		const float aO = getFloat( a.a, b.a );

		Math::Color out;
		return out.fromHSV( hO, sO, bO, aO );
	}

	if( type == RandomColorType::MixHSV ) {
		float hA = a.hue();
		float hB = b.hue();

		if( std::isnan( hA ) )
			hA = hB;
		if( std::isnan( hB ) )
			hB = hA;

		const float sA = a.saturation();
		const float sB = b.saturation();

		const float bA = a.brightness();
		const float bB = b.brightness();

		const float alpha = getFloat( 0.0f, 1.0f );

		float hO = hA + ( hB - hA ) * alpha;
		if( std::isnan( hO ) )
			hO = 0.0f;
		const float sO = sA + ( sB - sA ) * alpha;
		const float bO = bA + ( bB - bA ) * alpha;
		const float aO = a.a + ( b.a - a.a ) * alpha;

		Math::Color out;
		return out.fromHSV( hO, sO, bO, aO );
	}

	return a;
}

//--------------------------------------------------------------------------------

RandomNumbers< Math::Color >
	getColor( Math::Color a, Math::Color b, RandomColorType type, size_t count ) {
	return RandomNumbers< Math::Color >();
}

//--------------------------------------------------------------------------------

bool getBool() {
	int out = rand();
	return out % 2;
}

//--------------------------------------------------------------------------------

uint16_t getSeed() {
	return seed;
}

//--------------------------------------------------------------------------------

void setSeed( uint16_t number ) {
	seed = number;
	srand( seed );
}

//--------------------------------------------------------------------------------

}	 // namespace Random

//================================================================================
