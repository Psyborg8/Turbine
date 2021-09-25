//================================================================================

#include "MathTypes.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

namespace Math {

//================================================================================

// LOCAL - Color

//================================================================================

float maxCol( Color color ) {
	float max = color.r;

	if( color.g > max )
		max = color.g;
	if( color.b > max )
		max = color.b;

	return max;
}

//--------------------------------------------------------------------------------

float minCol( Color color ) {
	float min = color.r;

	if( color.g < min )
		min = color.g;
	if( color.b < min )
		min = color.b;

	return min;
}

//================================================================================

// CLASS - Color

//================================================================================

Color& Color::fromHSV( float h, float s, float v, float _a ) {
	float c = v * s;
	float x = c * ( 1.0f - std::abs( fmod( h * 6.0f, 2.0f ) -1.0f ) );
	float m = v - c;

	h *= 360.0f;

	if( h >= 0.0f && h < 60.0f ) {
		r = c + m;
		g = x + m;
		b = m;
	}
	else if( h >= 60.0f && h < 120.0f ) {
		r = x + m;
		g = c + m;
		b = m;
	}
	else if( h >= 120.0f && h < 180.0f ) {
		r = m;
		g = c + m;
		b = x + m;
	}
	else if( h >= 180.0f && h < 240.0f ) {
		r = m;
		g = x + m;
		b = c + m;
	}
	else if( h >= 240.0f && h < 300.0f ) {
		r = x + m;
		g = m;
		b = c + m;
	}
	else if( h >= 300.0f && h < 360.0f ) {
		r = c + m;
		g = m;
		b = x + m;
	}

	a = _a;
	return *this;
}

//--------------------------------------------------------------------------------

float Color::saturation() const {
	float maxC = maxCol( *this );
	float minC = minCol( *this );

	if( minC == maxC )
		return 0.0f;

	float brightness = 0.5f * ( minC + maxC );

	if( brightness < 0.5f )
		return ( maxC - minC ) / ( maxC - minC );

	return ( maxC - minC ) / ( 2.0f - maxC - minC );
}

//--------------------------------------------------------------------------------

float Color::brightness() const {
	float maxC = maxCol( *this );
	float minC = minCol( *this );

	return 0.5f * ( minC + maxC );
}

//--------------------------------------------------------------------------------

float Color::hue() const {
	float out;

	float maxC = maxCol( *this );
	float minC = minCol( *this );

	if( minC == maxC )
		out = 0.0f;
	if( maxC == r )
		out = 0.6f * ( g - b ) / ( maxC - minC );
	if( maxC == g )
		out = 0.6f * ( b - r ) / ( maxC - minC ) + 1.2f;
	if( maxC == b )
		out = 0.6f * ( r - g ) / ( maxC - minC ) + 2.4f;

	if( out < 0.0f )
		out += 3.6f;

	return out / 3.6f;
}

//--------------------------------------------------------------------------------

sf::Color Color::sf() const {
	sf::Color out;

	out.r = sf::Uint8( r * 255.0f );
	out.g = sf::Uint8( g * 255.0f );
	out.b = sf::Uint8( b * 255.0f );
	out.a = sf::Uint8( a * 255.0f );

	return out;
}

//--------------------------------------------------------------------------------

bool Color::operatorbool() const {
	return r + g + b != 0.0f;
}

//--------------------------------------------------------------------------------

bool Color::operator==( const Color& rh ) const {
	return r == rh.r && g == rh.g && b == rh.b && a == rh.a;
}

//--------------------------------------------------------------------------------

Color Color::operator+( const Color& rh ) const {
	Color out = *this;

	out.r += rh.r;
	out.g += rh.g;
	out.b += rh.b;

	return out;
}

//--------------------------------------------------------------------------------

Color& Color::operator+=( const Color& rh ) {
	r += rh.r;
	g += rh.g;
	b += rh.b;

	return *this;
}

//--------------------------------------------------------------------------------

Color Color::operator-( const Color& rh ) const {
	Color out = *this;

	out.r -= rh.r;
	out.g -= rh.g;
	out.b -= rh.b;

	return out;
}

//--------------------------------------------------------------------------------

Color& Color::operator-=( const Color& rh ) {
	r -= rh.r;
	g -= rh.g;
	b -= rh.b;

	return *this;
}

//--------------------------------------------------------------------------------

Color Color::operator*( const float& rh ) const {
	Color out = *this;

	out.r *= rh;
	out.g *= rh;
	out.b *= rh;

	return out;
}

//--------------------------------------------------------------------------------

Color Color::operator/( const float& rh ) const {
	Color out = *this;

	out.r /= rh;
	out.g /= rh;
	out.b /= rh;

	return out;
}

//================================================================================

// CLASS - Vec2

//================================================================================

float Vec2::length() const {
	return std::sqrt( x * x + y * y );
}

//--------------------------------------------------------------------------------

float Vec2::dot( const Vec2& rh ) const {
	return x * rh.x + y * rh.y;
}

//--------------------------------------------------------------------------------

float Vec2::determinant( const Vec2& rh ) const {
	return x * rh.y - y * rh.x;
}

//--------------------------------------------------------------------------------


Vec2 Vec2::normalize() const {
	if( !x && !y )
		return Math::Vec2( 0.0f, 0.0f );
	return *this / length();
}

//--------------------------------------------------------------------------------

Vec2 Vec2::abs() const {
	Vec2 out;

	out.x = std::abs( x );
	out.y = std::abs( y );

	return out;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::inverse() const {
	Vec2 out;

	out.x = 1.0f / x;
	out.y = 1.0f / y;

	return out;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::trim() const {
	Vec2 out;

	out.x = float( int( x ) );
	out.y = float( int( y ) );

	return out;
}

//--------------------------------------------------------------------------------

sf::Vector2f Vec2::sf() const {
	return sf::Vector2f( x, y );
}

//--------------------------------------------------------------------------------

sf::Vector2i Vec2::sfi() const {
	sf::Vector2i out;

	out.x = int( x );
	out.y = int( y );

	return out;
}

//--------------------------------------------------------------------------------

sf::Vector2u Vec2::sfu() const {
	sf::Vector2u out;

	out.x = uint32_t( x );
	out.y = uint32_t( y );

	return out;
}

//--------------------------------------------------------------------------------

bool Vec2::operatorbool() const {
	return x && y;
}

//--------------------------------------------------------------------------------

bool Vec2::operator==( const Vec2& rh ) const {
	return x == rh.x && y == rh.y;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::operator+( const Vec2& rh ) const {
	Vec2 out;

	out.x = x + rh.x;
	out.y = y + rh.y;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator+=( const Vec2& rh ) {
	x += rh.x;
	y += rh.y;

	return *this;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::operator-( const Vec2& rh ) const {
	Vec2 out;

	out.x = x - rh.x;
	out.y = y - rh.y;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator-=( const Vec2& rh ) {
	x -= rh.x;
	y -= rh.y;

	return *this;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::operator/( const Vec2& rh ) const
{
	Vec2 out;

	out.x = x / rh.x;
	out.y = y / rh.y;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator/=( const Vec2& rh ) {
	x /= rh.x;
	y /= rh.y;

	return *this;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::operator*( const Vec2& rh ) const {
	Vec2 out;

	out.x = x * rh.x;
	out.y = y * rh.y;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator*=( const Vec2& rh ) {
	x *= rh.x;
	y *= rh.y;

	return *this;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::operator*( const float& rh ) const {
	Vec2 out;

	out.x = x * rh;
	out.y = y * rh;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator*=( const float& rh ) {
	x *= rh;
	y *= rh;

	return *this;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::operator/( const float& rh ) const {
	Vec2 out;

	out.x = x / rh;
	out.y = y / rh;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator/=( const float& rh ) {
	x /= rh;
	y /= rh;

	return *this;
}

//--------------------------------------------------------------------------------

// ValueSet

//--------------------------------------------------------------------------------

void processSet( Math::ValueSet< int >& set ) {
	if( set.random )
		set.value = Random::getRandomIntInRange( set.min, set.max );
	else
		set.value = set.min;

	if( set.inverse )
		if( Random::getRandomBool() )
			set.value = -set.value;
}

//--------------------------------------------------------------------------------

void processSet( Math::ValueSet< float >& set ) {
	if( set.random )
		set.value = Random::getRandomFloatInRange( set.min, set.max );
	else
		set.value = set.min;

	if( set.inverse )
		if( Random::getRandomBool() )
			set.value = -set.value;
}

//--------------------------------------------------------------------------------

void processSet( Math::ValueSet< Math::Vec2 >& set ) {
	if( set.random )
		if( set.lock )
			set.value = mix( set.min, set.max, Random::getRandomFloatInRange( 0.0f, 1.0f ) );
		else
			set.value = Random::getRandomVec2InRange( set.min, set.max );
	else
		set.value = set.min;

	if( set.inverse ) {
		if( Random::getRandomBool() )
			set.value.x = -set.value.x;
		if( Random::getRandomBool() )
			set.value.y = -set.value.y;
	}
}

//--------------------------------------------------------------------------------

void processSet( Math::ValueSet< Math::Color >& set ) {
	if( set.random ) {
		Random::RandomColorType type;

		if( set.lock )
			if( set.hsv )
				type = Random::RandomColorType::MixHSV;
			else
				type = Random::RandomColorType::MixRGB;
		else
			if( set.hsv )
				type = Random::RandomColorType::ShuffleHSV;
			else
				type = Random::RandomColorType::ShuffleRGB;

		set.value = Random::getRandomColorInRange( set.min, set.max, Random::RandomColorType::MixRGB );
	}
	else
		set.value = set.min;
}

//================================================================================

} // namespace Math

//================================================================================