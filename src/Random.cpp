//================================================================================

#include "Random.h"

//--------------------------------------------------------------------------------

#include <cstdlib>

//================================================================================

namespace Random {

//================================================================================

int seed;

//================================================================================

int getRandomIntInRange( int min, int max ) {
	if( min == max )
		return min;

	int out = rand();
	out %= ( max - min );
	out += min;

	return out;
}

//--------------------------------------------------------------------------------

int getRandomIntInRange( int min, int max, function< int( int ) > func ) {
	if( min == max )
		return func( min );

	int out = getRandomIntInRange( min, max );
	out = func( out );

	return out;
}

//--------------------------------------------------------------------------------


RandomNumbers< int > getRandomIntInRange( int min, int max, size_t count ) {
	RandomNumbers< int > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getRandomIntInRange( min, max ) );

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< int > getRandomIntInRange( int min, int max, size_t count, function< int( int ) > func ) {
	RandomNumbers< int > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getRandomIntInRange( min, max, func ) );

	return out;
}

//--------------------------------------------------------------------------------

float getRandomFloatInRange( float min, float max ) {
	if( min == max )
		return min;

	float out = static_cast< float >( rand() );
	out /= static_cast< float >( RAND_MAX ) / ( max - min );
	out += min;

	return out;
}

//--------------------------------------------------------------------------------

float getRandomFloatInRange( float min, float max, function< float( float ) > func ) {
	if( min == max )
		return func( min );

	float out = getRandomFloatInRange( min, max );
	out = func( out );

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< float > getRandomFloatInRange( float min, float max, size_t count ) {
	RandomNumbers < float > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getRandomFloatInRange( min, max ) );

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< float > getRandomFloatInRange( float min, float max, size_t count, function< float( float ) > func ) {
	RandomNumbers< float > out;

	for( size_t i = 0u; i < count; ++i )
		out.push( getRandomFloatInRange( min, max, func ) );

	return out;
}

//--------------------------------------------------------------------------------

Math::Vec2 getRandomVec2InRange( Math::Vec2 min, Math::Vec2 max ) {
	if( min == max )
		return min;

	const float x = getRandomFloatInRange( min.x, max.x );
	const float y = getRandomFloatInRange( min.y, max.y );

	return Math::Vec2( x, y );
}

//--------------------------------------------------------------------------------

Math::Color getRandomColorInRange( Math::Color a, Math::Color b, RandomColorType type ) {
	if( a == b )
		return a;

	Math::Color out;
	if( type == RandomColorType::ShuffleRGB ) {
		out.r = getRandomFloatInRange( a.r, b.r );
		out.g = getRandomFloatInRange( a.g, b.g );
		out.b = getRandomFloatInRange( a.b, b.b );
		out.a = getRandomFloatInRange( a.a, b.a );
		return out;
	}

	if( type == RandomColorType::MixRGB ) {
		const float alpha = getRandomFloatInRange( 0.0f, 1.0f );
		out.r = a.r + ( b.r - a.r ) * alpha;
		out.g = a.g + ( b.g - a.g ) * alpha;
		out.b = a.b + ( b.b - a.b ) * alpha;
		out.a = a.a + ( b.a - a.a ) * alpha;
		return out;
	}

	if( type == RandomColorType::ShuffleHSV ) {
		float hA = a.hue();
		float hB = b.hue();
		if( isnan( hA ) )
			hA = hB;
		if( isnan( hB ) )
			hB = hA;

		const float sA = a.saturation();
		const float sB = b.saturation();

		const float bA = a.brightness();
		const float bB = b.brightness();

		float hO = getRandomFloatInRange( hA, hB );
		if( isnan( hO ) )
			hO = 0.0f;

		const float sO = getRandomFloatInRange( sA, sB );
		const float bO = getRandomFloatInRange( bA, bB );
		const float aO = getRandomFloatInRange( a.a, b.a );

		Math::Color out;
		return out.fromHSV( hO, sO, bO, aO );
	}

	if( type == RandomColorType::MixHSV ) {
		float hA = a.hue();
		float hB = b.hue();

		if( isnan( hA ) )
			hA = hB;
		if( isnan( hB ) )
			hB = hA;

		const float sA = a.saturation();
		const float sB = b.saturation();

		const float bA = a.brightness();
		const float bB = b.brightness();

		const float alpha = getRandomFloatInRange( 0.0f, 1.0f );

		float hO = hA + ( hB - hA ) * alpha;
		if( isnan( hO ) )
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

RandomNumbers< Math::Color > getRandomColorInRange( Math::Color a, Math::Color b, RandomColorType type, size_t count ) {
	return RandomNumbers< Math::Color >();
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

//================================================================================

} // Random

//================================================================================