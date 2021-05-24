//================================================================================

#pragma once

//================================================================================

namespace Math {

//================================================================================

struct Color {
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

//================================================================================

} // Math
namespace Colors {

//================================================================================

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
	double x;
	double y;

	Vec2() : x( 0.0 ), y( 0.0 ) {}
	Vec2( double x, double y ) : x( x ), y( y ) {}

	double length() const;
	double dot( const Vec2& rh ) const;
	double determinant( const Vec2& rh ) const;
	Vec2 normalize() const;
	Vec2 abs() const;
	Vec2 inverse() const;

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

} // Math

//================================================================================