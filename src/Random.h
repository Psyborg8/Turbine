//================================================================================

#pragma once

//================================================================================

namespace Random {

//================================================================================

template < class T >
class RandomNumbers : public stack< T > {
public:
	inline RandomNumbers() : stack< T >() {}

public:
	inline T topnpop() { T out = stack< T >::top(); stack< T >::pop(); return out; }
};

//================================================================================

	int getRandomIntInRange( int min, int max );
	int getRandomIntInRange( int min, int max, function< int( int randomNumber ) > func );
	RandomNumbers< int > getRandomIntInRange( int min, int max, size_t count );
	RandomNumbers< int > getRandomIntInRange( int min, int max, size_t count, function< int( int randomNumber ) > func );

	float getRandomFloatInRange( float min, float max );
	float getRandomFloatInRange( float min, float max, function< float( float randomNumber ) > func );
	RandomNumbers< float > getRandomFloatInRange( float min, float max, size_t count );
	RandomNumbers< float > getRandomFloatInRange( float min, float max, size_t count, function< float( float randomNumber ) > func );

	uint16_t getSeed();
	void setSeed( uint16_t seed );
};

//================================================================================