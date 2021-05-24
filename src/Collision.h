//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"

//================================================================================

class Object;

//================================================================================

namespace Collision {

//================================================================================

struct Ray {
	Vec2 start;
	Vec2 end;

	inline Vec2 direction() { return end - start; };
	inline double length() { return direction().length(); };
};

//--------------------------------------------------------------------------------

struct Rect {
	Vec2 position;
	Vec2 size;
	Vec2 velocity;

	inline Vec2 midpoint() { return position + ( size / 2.0 ); }
	inline void setMidpoint( Vec2 midpoint ) { position = midpoint - ( size / 2.0 ); }
};

//--------------------------------------------------------------------------------

struct DynamicCollision {
	bool isDynamic = false;
	Rect* staticRect;
	Rect* dynamicRect;
};

//--------------------------------------------------------------------------------

struct CollisionResult {
	bool success = false;
	Vec2 normal;
	Vec2 point;
	double distance;
	DynamicCollision dynamic;
};

//================================================================================

bool collision( Vec2 point, Rect rect );

CollisionResult collision( Ray ray, Rect rect );
CollisionResult collision( Rect* rectA, Rect* rectB );

void resolveCollision( CollisionResult result );

//================================================================================

} // namespace Collision

//================================================================================
