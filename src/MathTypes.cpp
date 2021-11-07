//================================================================================

#include "mathtypes.h"

//--------------------------------------------------------------------------------

#include "system.h"
#include "random.h"

#include "json.h"
#include "imgui-utils.h"

//================================================================================

namespace Math {

//--------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------

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

	if( maxC == 0.f )
		return 0.f;
	else
		return ( maxC - minC ) / maxC;
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

Color Color::operator*( const Color& rh ) const {
	Color out = *this;

	out.r *= rh.r;
	out.g *= rh.g;
	out.b *= rh.b;

	return out;
}

//--------------------------------------------------------------------------------

Color& Color::operator*=( const Color& rh ) {
	r *= rh.r;
	g *= rh.g;
	b *= rh.b;

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

float Vec2::angle() const {
	return angle( Math::Vec2( 0.f, 1.f ) );
}

//--------------------------------------------------------------------------------

float Vec2::angle( const Vec2& rh ) const {
	const float dot = Vec2::dot( rh );
	const float det = Vec2::determinant( rh );
	const float atan = atan2( dot, det );
	const float angle = atan * 180 / PI;

	return angle;
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

//================================================================================

void processSet( Math::ValueSet< int >& set ) {
	if( set.random )
		set.value = Random::getInt( set.min, set.max );
	else
		set.value = set.min;

	if( set.inverse )
		if( Random::getBool() )
			set.value = -set.value;
}

//--------------------------------------------------------------------------------

void processSet( Math::ValueSet< float >& set ) {
	if( set.random )
		set.value = Random::getFloat( set.min, set.max );
	else
		set.value = set.min;

	if( set.inverse )
		if( Random::getBool() )
			set.value = -set.value;
}

//--------------------------------------------------------------------------------

void processSet( Math::ValueSet< Math::Vec2 >& set ) {
	if( set.random )
		if( set.lock )
			set.value = mix( set.min, set.max, Random::getFloat( 0.0f, 1.0f ) );
		else
			set.value = Random::getVec2( set.min, set.max );
	else
		set.value = set.min;

	if( set.inverse ) {
		if( Random::getBool() )
			set.value.x = -set.value.x;
		if( Random::getBool() )
			set.value.y = -set.value.y;
	}
}

//--------------------------------------------------------------------------------

void processSet( Math::ValueSet< Math::Color >& set ) {
	if( set.random ) {
		RandomColorType type;

		if( set.lock )
			if( set.hsv )
				type = RandomColorType::MixHSV;
			else
				type = RandomColorType::MixRGB;
		else
			if( set.hsv )
				type = RandomColorType::ShuffleHSV;
			else
				type = RandomColorType::ShuffleRGB;

		set.value = Random::getColor( set.min, set.max, type );
	}
	else
		set.value = set.min;
}

//================================================================================

Math::Vec2 ValueVelocity::process() {
	Math::processSet( value );
	return value.value;
}

rapidjson::Value ValueVelocity::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "value", json::getValue( value ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void ValueVelocity::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;
	if( v.HasMember( "value" ) )
		json::getValue( v[ "value" ], value );
}
//--------------------------------------------------------------------------------

bool ValueVelocity::render() {
	bool out = false;
	ImGui::PushID( "ValueVelocity" );
	out |= ImGui::render( value, "Value" );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 AngleVelocity::process() {
	Math::processSet( angle );
	Math::processSet( power );

	return Math::Vec2::fromAngle( angle.value, power.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value AngleVelocity::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "angle", json::getValue( angle ), json::getAllocator() );
	out.AddMember( "power", json::getValue( power ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void AngleVelocity::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "angle" ) )
		json::getValue( v[ "angle" ], angle );
	if( v.HasMember( "power" ) )
		json::getValue( v[ "power" ], power );
}

//--------------------------------------------------------------------------------

bool AngleVelocity::render() {
	bool out = false;
	ImGui::PushID( "AngleVelocity" );
	out |= ImGui::render( angle, "Angle" );
	out |= ImGui::render( power, "Power" );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 PointVelocity::process( Math::Vec2 position ) {
	Math::processSet( power );

	const Math::Vec2 diff = target - position;

	return diff.normalize() * power.value;
}

//--------------------------------------------------------------------------------

rapidjson::Value PointVelocity::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "target", json::getValue( target ), json::getAllocator() );
	out.AddMember( "power", json::getValue( power ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void PointVelocity::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "target" ) )
		json::getValue( v[ "target" ], target );
	if( v.HasMember( "power" ) )
		json::getValue( v[ "power" ], power );
}

//--------------------------------------------------------------------------------

bool PointVelocity::render() {
	bool out = false;
	ImGui::PushID( "PointVelocity" );

	ImGui::Text( "Target" );
	out |= ImGui::InputFloat2( "##Target", &target.x, 3 );
	out |= ImGui::render( power, "Power" );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 Velocity::process( Math::Vec2 position ) {
	Math::Vec2 out;

	switch( type ) {
	case Type::ByValue:
		out = value.value.process();
		break;
	case Type::ByAngle:
		out = value.angle.process();
		break;
	case Type::ByPoint:
		out = value.point.process( position );
		break;
	}

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value Velocity::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "type", json::getValue( ( int )type ), json::getAllocator() );

	rapidjson::Value v;
	switch( type ) {
	case Type::ByValue: v = value.value.getValue(); break;
	case Type::ByPoint: v = value.point.getValue(); break;
	case Type::ByAngle: v = value.angle.getValue(); break;
	}

	out.AddMember( "value", v, json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void Velocity::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "type" ) ) {
		int t;
		json::getValue( v[ "type" ], t );
		type = ( Type )t;
	}

	if( !v.HasMember( "value" ) )
		return;

	switch( type ) {
	case Type::ByValue: value.value.setValue( v[ "value" ] ); break;
	case Type::ByPoint: value.point.setValue( v[ "value" ] ); break;
	case Type::ByAngle: value.angle.setValue( v[ "value" ] ); break;
	}
}

//--------------------------------------------------------------------------------

bool Velocity::render( const char* id ) {
	bool out = false;
	ImGui::PushID( id );
	if( ImGui::Button( "Value" ) ) {
		out = true;
		type = Type::ByValue;
		value.value = ValueVelocity();
	}
	ImGui::SameLine();
	if( ImGui::Button( "Angle" ) ) {
		out = true;
		type = Type::ByAngle;
		value.angle = AngleVelocity();
	}
	ImGui::SameLine();
	if( ImGui::Button( "Point" ) ) {
		out = true;
		type = Type::ByPoint;
		value.point = PointVelocity();
	}

	switch( type ) {
	case Type::ByValue: out |= value.value.render(); break;
	case Type::ByAngle: out |= value.angle.render(); break;
	case Type::ByPoint: out |= value.point.render(); break;
	}

	ImGui::PopID();
	return out;
}

//================================================================================
//================================================================================

rapidjson::Value PointPosition::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "value", json::getValue( position ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void PointPosition::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "value" ) )
		json::getValue( v[ "value" ], position );
}

//--------------------------------------------------------------------------------

bool PointPosition::render() {
	bool out = false;
	ImGui::PushID( "PointPosition" );
	out |= ImGui::InputFloat2( "Value", &position.x, 3 );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 CirclePosition::process() {
	Math::processSet( minRadius );
	Math::processSet( maxRadius );
	Math::processSet( minAngle );
	Math::processSet( maxAngle );

	const float radius = Random::getFloat( minRadius.value, maxRadius.value );
	const float angle = Random::getFloat( minAngle.value, maxAngle.value );

	const Math::Vec2 dist = Math::Vec2::fromAngle( angle, radius );
	const Math::Vec2 out = center + dist;
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value CirclePosition::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "minRadius", json::getValue( minRadius ), json::getAllocator() );
	out.AddMember( "maxRadius", json::getValue( maxRadius ), json::getAllocator() );
	out.AddMember( "minAngle", json::getValue( minAngle ), json::getAllocator() );
	out.AddMember( "maxAngle", json::getValue( maxAngle ), json::getAllocator() );
	out.AddMember( "center", json::getValue( center ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void CirclePosition::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "minRadius" ) )
		json::getValue( v[ "minRadius" ], minRadius );
	if( v.HasMember( "maxRadius" ) )
		json::getValue( v[ "maxRadius" ], maxRadius );
	if( v.HasMember( "minAngle" ) )
		json::getValue( v[ "minAngle" ], minAngle );
	if( v.HasMember( "maxAngle" ) )
		json::getValue( v[ "maxAngle" ], maxAngle );
	if( v.HasMember( "center" ) )
		json::getValue( v[ "center" ], center );
}

//--------------------------------------------------------------------------------

bool CirclePosition::render() {
	bool out = false;
	ImGui::PushID( "CirclePosition" );
	ImGui::Text( "Center" );
	out |= ImGui::InputFloat2( "##Center", &center.x, 3 );
	out |= ImGui::render( minRadius, "Inner Radius" );
	out |= ImGui::render( maxRadius, "Outer Radius" );
	out |= ImGui::render( minAngle, "Min Angle" );
	out |= ImGui::render( maxAngle, "Max Angle" );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 UniformCirclePosition::process( int index, int total ) {
	Math::processSet( radius );
	Math::processSet( minAngle );
	Math::processSet( maxAngle );

	const float alpha = ( float )index / float( total );
	const float angle = Math::mix( minAngle.value, maxAngle.value, alpha );

	const Math::Vec2 dist = Math::Vec2::fromAngle( angle, radius.value );
	const Math::Vec2 out = center + dist;
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value UniformCirclePosition::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "radius", json::getValue( radius ), json::getAllocator() );
	out.AddMember( "minAngle", json::getValue( minAngle ), json::getAllocator() );
	out.AddMember( "maxAngle", json::getValue( maxAngle ), json::getAllocator() );
	out.AddMember( "center", json::getValue( center ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void UniformCirclePosition::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "radius" ) )
		json::getValue( v[ "radius" ], radius );
	if( v.HasMember( "minAngle" ) )
		json::getValue( v[ "minAngle" ], minAngle );
	if( v.HasMember( "maxAngle" ) )
		json::getValue( v[ "maxAngle" ], maxAngle );
	if( v.HasMember( "center" ) )
		json::getValue( v[ "center" ], center );
}

//--------------------------------------------------------------------------------

bool UniformCirclePosition::render() {
	bool out = false;
	ImGui::PushID( "UniformCirclePosition" );
	ImGui::Text( "Center" );
	out |= ImGui::InputFloat2( "##Center", &center.x, 3 );
	out |= ImGui::render( radius, "Radius" );
	out |= ImGui::render( minAngle, "Min Angle" );
	out |= ImGui::render( maxAngle, "Max Angle" );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 SquarePosition::process() {
	Math::processSet( minSize );
	Math::processSet( maxSize );

	Math::Vec2 dist;
	dist.x = Random::getFloat( minSize.value.x, maxSize.value.x );
	dist.y = Random::getFloat( minSize.value.y, maxSize.value.y );

	const Math::Vec2 out = center + dist;
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value SquarePosition::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "minSize", json::getValue( minSize ), json::getAllocator() );
	out.AddMember( "maxSize", json::getValue( maxSize ), json::getAllocator() );
	out.AddMember( "center", json::getValue( center ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void SquarePosition::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "minSize" ) )
		json::getValue( v[ "minSize" ], minSize );
	if( v.HasMember( "maxSize" ) )
		json::getValue( v[ "maxSize" ], maxSize );
	if( v.HasMember( "center" ) )
		json::getValue( v[ "center" ], center );
}

//--------------------------------------------------------------------------------

bool SquarePosition::render() {
	bool out = false;
	ImGui::PushID( "SquarePosition" );
	ImGui::Text( "Center" );
	out |= ImGui::InputFloat2( "##Center", &center.x, 3 );
	out |= ImGui::render( minSize, "Min Size" );
	out |= ImGui::render( maxSize, "Max Size" );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 UniformSquarePosition::process( int index, int total ) {
	Math::processSet( size );

	array< float, 4u > steps;
	if( proportional ) {
		const float total = size.value.x * 2.f + size.value.y * 2.f;
		steps[ 0 ] = size.value.x / total;
		steps[ 1 ] = ( size.value.x + size.value.y ) / total;
		steps[ 2 ] = ( size.value.x * 2 + size.value.y ) / total;
		steps[ 3 ] = 1.f;
	}
	else {
		steps = { 0.25f, 0.5f, 0.75f, 1.0f };
	}

	Math::Vec2 dist;

	const float alpha = ( float )index / ( float )total;
	if( alpha < steps[ 0 ] ) {
		const float localAlpha = alpha / steps[ 0 ];
		dist.x = Math::mix( -size.value.x / 2.f, size.value.x / 2.f, localAlpha );
		dist.y = -size.value.y / 2.f;
	}
	else if( alpha < steps[ 1 ] ) {
		const float localAlpha = ( alpha - steps[ 0 ] ) / ( steps[ 1 ] - steps[ 0 ] );
		dist.y = Math::mix( -size.value.y / 2.f, size.value.y / 2.f, localAlpha );
		dist.x = size.value.x / 2.f;
	}
	else if( alpha < steps[ 2 ] ) {
		const float localAlpha = ( alpha - steps[ 1 ] ) / ( steps[ 2 ] - steps[ 1 ] );
		dist.x = Math::mix( size.value.x / 2.f, -size.value.x / 2.f, localAlpha );
		dist.y = size.value.y / 2.f;
	}
	else {
		const float localAlpha = ( alpha - steps[ 2 ] ) / ( steps[ 3 ] - steps[ 2 ] );
		dist.y = Math::mix( size.value.y / 2.f, -size.value.y / 2.f, localAlpha );
		dist.x = -size.value.x / 2.f;
	}

	const Math::Vec2 out = center + dist;
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value UniformSquarePosition::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "size", json::getValue( size ), json::getAllocator() );
	out.AddMember( "center", json::getValue( center ), json::getAllocator() );
	out.AddMember( "proportional", json::getValue( proportional ), json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void UniformSquarePosition::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "size" ) )
		json::getValue( v[ "size" ], size );
	if( v.HasMember( "center" ) )
		json::getValue( v[ "center" ], center );
	if( v.HasMember( "proportional" ) )
		json::getValue( v[ "proportional" ], proportional );
}

//--------------------------------------------------------------------------------

bool UniformSquarePosition::render() {
	bool out = false;
	ImGui::PushID( "UniformSquarePosition" );
	out |= ImGui::Checkbox( "Proportional", &proportional );
	ImGui::Text( "Center" );
	out |= ImGui::InputFloat2( "##Center", &center.x, 3 );
	out |= ImGui::render( size, "Size" );
	ImGui::PopID();
	return out;
}

//================================================================================

Math::Vec2 Position::process( int index, int total ) {
	Math::Vec2 out;

	switch( type ) {
	case Type::Point:
		out = value.point.position;
		break;
	case Type::Circle:
		out = value.circle.process();
		break;
	case Type::UniformCircle:
		out = value.uniformCircle.process( index, total );
		break;
	case Type::Square:
		out = value.square.process();
		break;
	case Type::UniformSquare:
		out = value.uniformSquare.process( index, total );
		break;
	}

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value Position::getValue() {
	rapidjson::Value out;
	out.SetObject();
	out.AddMember( "type", json::getValue( ( int )type ), json::getAllocator() );

	rapidjson::Value v;
	switch( type ) {
	case Type::Point: v = value.point.getValue(); break;
	case Type::Circle: v = value.circle.getValue(); break;
	case Type::UniformCircle: v = value.uniformCircle.getValue(); break;
	case Type::Square: v = value.square.getValue(); break;
	case Type::UniformSquare: v = value.square.getValue(); break;
	}

	out.AddMember( "value", v, json::getAllocator() );
	return out;
}

//--------------------------------------------------------------------------------

void Position::setValue( const rapidjson::Value& v ) {
	if( !v.IsObject() )
		return;

	if( v.HasMember( "type" ) ) {
		int t;
		json::getValue( v[ "type" ], t );
		type = ( Type )t;
	}

	if( !v.HasMember( "value" ) )
		return;

	const rapidjson::Value& v1 = v[ "value" ];
	switch( type ) {
	case Type::Point: value.point.setValue( v1 ); break;
	case Type::Circle: value.circle.setValue( v1 ); break;
	case Type::UniformCircle: value.uniformCircle.setValue( v1 ); break;
	case Type::Square: value.square.setValue( v1 ); break;
	case Type::UniformSquare: value.uniformSquare.setValue( v1 ); break;
	}
}

//--------------------------------------------------------------------------------

bool Position::render() {
	bool out = false;
	ImGui::PushID( "Position" );

	string text;
	switch( type ) {
	case Type::Point: text = "Type: Point"; break;
	case Type::Circle: text = "Type: Circle"; break;
	case Type::UniformCircle: text = "Type: Uniform Circle"; break;
	case Type::Square: text = "Type: Square"; break;
	case Type::UniformSquare: text = "Type: Uniform Square"; break;
	}
	if( ImGui::Button( text.c_str() ) )
		ImGui::OpenPopup( "Type" );

	if( ImGui::BeginPopupContextWindow( "Type", 1, false ) ) {
		if( ImGui::Selectable( "Point" ) ) {
			out = true;
			type = Type::Point;
			value.point = PointPosition();
			ImGui::CloseCurrentPopup();
		}
		if( ImGui::Selectable( "Circle" ) ) {
			out = true;
			type = Type::Circle;
			value.circle = CirclePosition();
			ImGui::CloseCurrentPopup();
		}
		if( ImGui::Selectable( "Uniform Circle" ) ) {
			out = true;
			type = Type::UniformCircle;
			value.uniformCircle = UniformCirclePosition();
			ImGui::CloseCurrentPopup();
		}
		if( ImGui::Selectable( "Square" ) ) {
			out = true;
			type = Type::Square;
			value.square = SquarePosition();
			ImGui::CloseCurrentPopup();
		}
		if( ImGui::Selectable( "Uniform Square" ) ) {
			out = true;
			type = Type::UniformSquare;
			value.uniformSquare = UniformSquarePosition();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	switch( type ) {
	case Type::Point: out |= value.point.render(); break;
	case Type::Circle: out |= value.circle.render(); break;
	case Type::UniformCircle: out |= value.uniformCircle.render(); break;
	case Type::Square: out |= value.square.render(); break;
	case Type::UniformSquare: out |= value.uniformSquare.render(); break;
	}

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

} // namespace Math

//================================================================================