//================================================================================

#pragma once

//================================================================================

struct Color
{
	float r;
	float g;
	float b;
	float a;

	Color() : r( 0.0 ), g( 0.0 ), b( 0.0 ), a( 1.0 ) {}
	Color( float r, float g, float b, float a ) : r( r ), g( g ), b( b ), a( a ) {}

	float saturation();
	float brightness();
	float hue();

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