//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"

namespace Game { class RigidRect; }

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
	shared_ptr< Game::RigidRect > staticRect;
	shared_ptr< Game::RigidRect > dynamicRect;
};

//--------------------------------------------------------------------------------

struct CollisionResult {
	bool success = false;
	Math::Vec2 normal;
	Math::Vec2 point;
	Math::Vec2 velocity;
	float distance;
	DynamicCollision dynamic;
};

//================================================================================

bool collision( Math::Vec2 point, shared_ptr< Game::RigidRect > rect );

CollisionResult collision( Ray ray, shared_ptr< Game::RigidRect > rect );
CollisionResult collision( shared_ptr< Game::RigidRect > rectA, shared_ptr< Game::RigidRect > rectB );

void resolveCollision( CollisionResult result );

//================================================================================

} // namespace Collision

//================================================================================
