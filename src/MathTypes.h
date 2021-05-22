//================================================================================

#pragma once

//================================================================================

struct Color
{
	float r;
	float g;
	float b;
	float a;

	Color() : r( 0.0f ), g( 0.0f ), b( 0.0f ), a( 1.0f ) {}
	Color( float r, float g, float b, float a ) : r( r ), g( g ), b( b ), a( a ) {}

	float saturation() const;
	float brightness() const;
	float hue() const;

	bool operatorbool() const;
	bool operator==( const Color& rh ) const;

	Color operator+( const Color& rh ) const;
	Color& operator+=( const Color& rh );
	Color operator-( const Color& rh ) const;
	Color& operator-=( const Color& rh );
	
	Color operator*( const float& ) const;
	Color operator/( const float& ) const;
};

//--------------------------------------------------------------------------------

namespace Colors
{

static const Color WHITE = Color( 1.0f, 1.0f, 1.0f, 1.0f );
static const Color BLACK = Color( 0.0f, 0.0f, 0.0f, 1.0f );
static const Color RED = Color( 1.0f, 0.0f, 0.0f, 1.0f );
static const Color GREEN = Color( 0.0f, 1.0f, 0.0f, 1.0f );
static const Color BLUE = Color( 0.0f, 0.0f, 1.0f, 1.0f );
static const Color CYAN = Color( 0.0f, 1.0f, 1.0f, 1.0f );
static const Color MAGENTA = Color( 1.0f, 0.0f, 1.0f, 1.0f );
static const Color YELLOW = Color( 1.0f, 1.0f, 0.0f, 1.0f );

}

//================================================================================

struct Vec2
{
	double x;
	double y;

	Vec2() : x( 0.0 ), y( 0.0 ) {}
	Vec2( double x, double y ) : x( x ), y( y ) {}

	double length() const;
	double dot( const Vec2& rh ) const;
	double determinant( const Vec2& rh ) const;
	Vec2 normalize() const;
	Vec2 abs() const;

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

	Vec2 operator*( const double& rh ) const;
	Vec2& operator*=( const double& rh );
	Vec2 operator/( const double& rh ) const;
	Vec2& operator/=( const double& rh );
};

//================================================================================