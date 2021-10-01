//================================================================================

#pragma once

//================================================================================

#include "globaL.h"

//================================================================================

namespace Math {

//================================================================================

struct Color {
	Color( const sf::Color& color ) : 
		r( float( color.r ) / 255.0f ), 
		g( float( color.g ) / 255.0f ), 
		b( float( color.b ) / 255.0f ), 
		a( float( color.a ) / 255.0f ) 
	{}

	float r;
	float g;
	float b;
	float a;

	Color() : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 1.0f ) {}
	Color( float r, float g, float b, float a ) : r( r ), g( g ), b( b ), a( a ) {}

	Color& fromHSV( float h, float s, float v, float a );

	float saturation() const;
	float brightness() const;
	float hue() const;

	sf::Color sf() const;

	bool operatorbool() const;
	bool operator==( const Color& rh ) const;

	Color operator+( const Color& rh ) const;
	Color& operator+=( const Color& rh );
	Color operator-( const Color& rh ) const;
	Color& operator-=( const Color& rh );

	Color operator*( const float& ) const;
	Color operator/( const float& ) const;
};

//================================================================================

} // Math
namespace Colors {

//================================================================================

static const Math::Color CLEAR{ 0.0f, 0.0f, 0.0f, 0.0f };
static const Math::Color WHITE{ 1.0f, 1.0f, 1.0f, 1.0f };
static const Math::Color BLACK{ 0.0f, 0.0f, 0.0f, 1.0f };
static const Math::Color RED{ 1.0f, 0.0f, 0.0f, 1.0f };
static const Math::Color GREEN{ 0.0f, 1.0f, 0.0f, 1.0f };
static const Math::Color BLUE{ 0.0f, 0.0f, 1.0f, 1.0f };
static const Math::Color CYAN{ 0.0f, 1.0f, 1.0f, 1.0f };
static const Math::Color MAGENTA{ 1.0f, 0.0f, 1.0f, 1.0f };
static const Math::Color YELLOW{ 1.0f, 1.0f, 0.0f, 1.0f };

}

//================================================================================

namespace Math {

//================================================================================

struct Vec2 {
	Vec2( sf::Vector2f v ) : x{ v.x }, y{ v.y } {}
	Vec2( sf::Vector2i v ) : x{ float( v.x ) }, y{ float( v.y ) } {}
	Vec2( sf::Vector2u v ) : x{ float( v.x ) }, y{ float( v.y ) } {}
	Vec2( array< float, 2u > v ) : x{ v[ 0 ] }, y{ v[ 1 ] } {}

	float x;
	float y;

	Vec2() : x( 0.0 ), y( 0.0 ) {}
	Vec2( float x, float y ) : x( x ), y( y ) {}

	float length() const;
	float dot( const Vec2& rh ) const;
	float determinant( const Vec2& rh ) const;
	Vec2 normalize() const;
	Vec2 abs() const;
	Vec2 inverse() const;
	Vec2 trim() const;
	sf::Vector2f sf() const;
	sf::Vector2i sfi() const;
	sf::Vector2u sfu() const;

	bool operatorbool() const;
	bool operator==( const Vec2& rh ) const;

	Vec2 operator+( const Vec2& rh ) const;
	Vec2& operator+=( const Vec2& rh );
	Vec2 operator-( const Vec2& rh ) const;
	Vec2& operator-=( const Vec2& rh );
	Vec2 operator/( const Vec2& rh ) const;
	Vec2& operator/=( const Vec2& rh );
	Vec2 operator*( const Vec2& rh ) const;
	Vec2& operator*=( const Vec2& rh );

	Vec2 operator*( const float& rh ) const;
	Vec2& operator*=( const float& rh );
	Vec2 operator/( const float& rh ) const;
	Vec2& operator/=( const float& rh );
};

//================================================================================

template< class T >
struct ValueSet {
	ValueSet( T def ) : min( def ), max( def ), value( def ){}

	T min;
	T max;
	T value;

	bool random{ false };
	bool lock{ false };
	bool hsv{ false };
	bool inverse{ false };
};

void processSet( Math::ValueSet< int >& set );
void processSet( Math::ValueSet< float >& set );
void processSet( Math::ValueSet< Math::Vec2 >& set );
void processSet( Math::ValueSet< Math::Color >& set );

//================================================================================

template< class T >
T mix( T a, T b, float alpha ) {
	return a + ( b - a ) * alpha;
}

//================================================================================

} // Math

//================================================================================