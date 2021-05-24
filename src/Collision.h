//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"

//================================================================================

namespace Collision {

//================================================================================

struct Ray {
	Math::Vec2 start;
	Math::Vec2 end;

	inline Math::Vec2 direction() { return end - start; };
	inline double length() { return direction().length(); };
};

//--------------------------------------------------------------------------------

struct Rect {
	Math::Vec2 position;
	Math::Vec2 size;
	Math::Vec2 velocity;

	inline Math::Vec2 midpoint() { return position + ( size / 2.0 ); }
	inline void setMidpoint( Math::Vec2 midpoint ) { position = midpoint - ( size / 2.0 ); }
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
	Math::Vec2 normal;
	Math::Vec2 point;
	double distance;
	DynamicCollision dynamic;
};

//================================================================================

bool collision( Math::Vec2 point, Rect rect );

CollisionResult collision( Ray ray, Rect rect );
CollisionResult collision( Rect* rectA, Rect* rectB );

void resolveCollision( CollisionResult result );

//================================================================================

} // namespace Collision

//================================================================================
