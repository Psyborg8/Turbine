//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "mathtypes.h"

//================================================================================

namespace Random {

//--------------------------------------------------------------------------------

template < class T >
class RandomNumbers : public stack< T > {
public:
	inline RandomNumbers() : stack< T >() {}

public:
	inline T topnpop() { T out = stack< T >::top(); stack< T >::pop(); return out; }
};

//================================================================================

int getInt( int min, int max );
int getInt( int min, int max, function< int( int randomNumber ) > func );
RandomNumbers< int > getInt( int min, int max, size_t count );
RandomNumbers< int > getInt( int min, int max, size_t count, function< int( int randomNumber ) > func );

float getFloat( float min, float max );
float getFloat( float min, float max, function< float( float randomNumber ) > func );
RandomNumbers< float > getFloat( float min, float max, size_t count );
RandomNumbers< float > getFloat( float min, float max, size_t count, function< float( float randomNumber ) > func );

bool getBool();

Math::Vec2 getVec2( Math::Vec2 min, Math::Vec2 max );

Math::Color getColor( Math::Color a, Math::Color b, RandomColorType type );
RandomNumbers< Math::Color > getColor( Math::Color a, Math::Color b, RandomColorType type, size_t count );

//--------------------------------------------------------------------------------

uint16_t getSeed();
void setSeed( uint16_t seed );

//--------------------------------------------------------------------------------

} // Random

//================================================================================