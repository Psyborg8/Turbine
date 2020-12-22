//================================================================================

#include "Random.h"

//--------------------------------------------------------------------------------

#include <cstdlib>

//================================================================================

// CLASS - Random

//================================================================================

Random::Random() : m_seed( 0 )
{
	//
}

//--------------------------------------------------------------------------------

Random::Random( unsigned int seed ) : m_seed( seed )
{
	srand( m_seed );
}

//--------------------------------------------------------------------------------

int Random::getRandomIntInRange( int min, int max ) const
{
	int out = rand();
	out %= ( max - min );
	out += min;

	return out;
}

//--------------------------------------------------------------------------------

int Random::getRandomIntInRange( int min, int max, function< int( int ) > func ) const
{
	int out = getRandomIntInRange( min, max );
	out = func( out );

	return out;
}

//--------------------------------------------------------------------------------


RandomNumbers< int > Random::getRandomIntInRange( int min, int max, size_t count ) const
{
	RandomNumbers< int > out;

	for( size_t i = 0u; i < count; ++i )
	{
		out.push( getRandomIntInRange( min, max ) );
	}

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< int > Random::getRandomIntInRange( int min, int max, size_t count, function< int( int ) > func ) const
{
	RandomNumbers< int > out;

	for( size_t i = 0u; i < count; ++i )
	{
		out.push( getRandomIntInRange( min, max, func ) );
	}

	return out;
}

//--------------------------------------------------------------------------------

float Random::getRandomFloatInRange( float min, float max ) const
{
	float out = static_cast< float >( rand() );
	out /= static_cast< float >( RAND_MAX / ( max - min ) );
	out += min;
	

	return out;
}

//--------------------------------------------------------------------------------

float Random::getRandomFloatInRange( float min, float max, function< float( float ) > func ) const
{
	float out = getRandomFloatInRange( min, max );
	out = func( out );

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< float > Random::getRandomFloatInRange( float min, float max, size_t count ) const
{
	RandomNumbers < float > out;

	for( size_t i = 0u; i < count; ++i )
	{
		out.push( getRandomFloatInRange( min, max ) );
	}

	return out;
}

//--------------------------------------------------------------------------------

RandomNumbers< float > Random::getRandomFloatInRange( float min, float max, size_t count, function< float( float ) > func ) const
{
	RandomNumbers< float > out;

	for( size_t i = 0u; i < count; ++i )
	{
		out.push( getRandomFloatInRange( min, max, func ) );
	}

	return out;
}

//================================================================================