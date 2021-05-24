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
	int out = rand();
	out %= ( max - min );
	out += min;

	return out;
}

//--------------------------------------------------------------------------------

int getRandomIntInRange( int min, int max, function< int( int ) > func ) {
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
	float out = static_cast< float >( rand() );
	out /= static_cast< float >( RAND_MAX ) / ( max - min );
	out += min;

	return out;
}

//--------------------------------------------------------------------------------

float getRandomFloatInRange( float min, float max, function< float( float ) > func ) {
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