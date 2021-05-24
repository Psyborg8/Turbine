//================================================================================

#include "MathTypes.h"

//--------------------------------------------------------------------------------

#include "System.h"

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

double Vec2::length() const {
	return std::sqrt( x * x + y * y );
}

//--------------------------------------------------------------------------------

double Vec2::dot( const Vec2& rh ) const {
	return x * rh.x + y * rh.y;
}

//--------------------------------------------------------------------------------

double Vec2::determinant( const Vec2& rh ) const {
	return x * rh.y - y * rh.x;
}

//--------------------------------------------------------------------------------


Vec2 Vec2::normalize() const {
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

	out.x = 1.0 / x;
	out.y = 1.0 / y;

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

Vec2 Vec2::operator*( const double& rh ) const{
	Vec2 out;

	out.x = x * rh;
	out.y = y * rh;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator*=( const double& rh ) {
	x *= rh;
	y *= rh;

	return *this;
}

//--------------------------------------------------------------------------------

Vec2 Vec2::operator/( const double& rh ) const {
	Vec2 out;

	out.x = x / rh;
	out.y = y / rh;

	return out;
}

//--------------------------------------------------------------------------------

Vec2& Vec2::operator/=( const double& rh ) {
	x /= rh;
	y /= rh;

	return *this;
}

//================================================================================