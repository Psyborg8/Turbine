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

using mathfuVec2 = mathfu::Vector< float, 2 >;
struct Vec2 : public mathfuVec2
{
public:
	inline Vec2() : mathfuVec2( 0.0, 0.0 ) {}
	inline Vec2( float x, float y ) : mathfuVec2( x, y ) {}
	inline Vec2( const mathfuVec2& mfv2 ) : mathfuVec2( mfv2 ) {}
	inline Vec2( const b2Vec2& b2v2 ) : mathfuVec2( static_cast< float >( b2v2.x ), static_cast< float >( b2v2.y ) ) {}

	Vec2 toScreenRatio();

public:
	inline operator mathfuVec2()
	{
		return mathfuVec2( x, y );
	}

	inline operator b2Vec2()
	{
		return b2Vec2( static_cast< float >( x ), static_cast< float >( y ) );
	}
};

//================================================================================

struct Transform
{
	Vec2 position;
	Vec2 anchor;
	Vec2 scale;
	float rotation;

	Transform() : position(), anchor(), scale(), rotation( 0.0f ) {}
	Transform( Vec2 position, Vec2 anchor, Vec2 scale, float rotation ) :
		position( position ), anchor( anchor ), scale( scale ), rotation( rotation ) {}

	bool operator==( const Transform& rh )
	{
		return position == rh.position &&
			anchor == rh.anchor &&
			scale == rh.scale &&
			rotation == rh.rotation;
	}
};

//================================================================================