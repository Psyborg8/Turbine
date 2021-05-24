//================================================================================

#include "Collision.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

namespace Collision {

//================================================================================

bool staticRectCollision( Rect rectA, Rect rectB );

//================================================================================

bool collision( Math::Vec2 point, Rect rect ) {
	return ( point.x >= rect.position.x ) &&
		( point.x <= rect.position.x + rect.size.x ) &&
		( point.y >= rect.position.y ) &&
		( point.y <= rect.position.y + rect.size.y );
}

//--------------------------------------------------------------------------------

CollisionResult collision( Ray ray, Rect rect ) {
	CollisionResult out;
	out.success = false;

	// Calculate inverse 
	Math::Vec2 inverseDirection = ray.direction().inverse();

	// Find near and far collisions
	Math::Vec2 tNear = ( rect.position - ray.start ) * inverseDirection;
	Math::Vec2 tFar = ( rect.position + rect.size - ray.start ) * inverseDirection ;

	if( std::isnan( tFar.y ) || std::isnan( tFar.x ) )
		return out;
	if( std::isnan( tNear.y ) || std::isnan( tNear.x ) )
		return out;

	// Account for negative collision
	if( tNear.x > tFar.x )
		std::swap( tNear.x, tFar.x );
	if( tNear.y > tFar.y )
		std::swap( tNear.y, tFar.y );

	// Collision order tells us if there was a collision
	if( tNear.x >= tFar.y || tNear.y >= tFar.x )
		return out;

	// Ignore perfect diagonal collision
	if( tNear.x == tNear.y )
		return out;

	// The alphas of the collisions
	double tHitNear = std::max( tNear.x, tNear.y );
	double tHitFar = std::min( tFar.x, tFar.y );

	// Collision is happening out of range
	if( tHitFar <= 0.0 || tHitNear >= 1.0 )
		return out;

	// Output collision alpha and collision point
	out.distance = tHitNear;
	out.point = ray.start + ( ray.direction() * tHitNear );

	// Find normal
	if( tNear.x > tNear.y )
		out.normal = inverseDirection.x < 0.0 ? Math::Vec2{ 1, 0 } : Math::Vec2{ -1, 0 };
	else
		out.normal = inverseDirection.y < 0.0 ? Math::Vec2{ 0, 1 } : Math::Vec2{ 0, -1 };

	out.success = true;

	return out;
}

//--------------------------------------------------------------------------------

CollisionResult collision( Rect* rectA, Rect* rectB ) {
	CollisionResult out;
	out.success = false;

	// Dynamic-Dynamic collision not yet implemented.
	if( rectA->velocity.length() > 0.0 && rectB->velocity.length() > 0.0 )
		return out;

	// Narrow Phase
	Rect* d; // Dynamic
	Rect* s; // Static

	// Find the dynamic object
	d = rectA->velocity.length() > 0.0 ? rectA : rectB;
	s = rectA->velocity.length() == 0.0 ? rectA : rectB;

	if( s == d ) {
		// Static collision
		out.success = staticRectCollision( *rectA, *rectB );
		return out;
	}

	// Create a combined radius
	Rect e;
	e.position = s->position - ( d->size / 2.0 );
	e.size = s->size + d->size;

	// Do a ray cast to the combined rect
	Ray rayCollider;
	rayCollider.start = d->midpoint();
	rayCollider.end = rayCollider.start + ( d->velocity * System::getDeltaTime() );

	out = collision( rayCollider, e );
	if( out.success ) {

		out.success = false;
		if( out.normal.x > 0.0 )
			if( d->position.x == s->position.x + s->size.x )
				return out;
		if( out.normal.x < 0.0 )
			if( d->position.x + d->size.x == s->position.x )
				return out;
		if( out.normal.y > 0.0 )
			if( d->position.y == s->position.y + s->size.y )
				return out;
		if( out.normal.y < 0.0 )
			if( d->position.y + d->size.y == s->position.y )
				return out;

		out.success = true;

		// Set point and distance data
		Math::Vec2 distance = out.point - e.midpoint();
		distance = ( distance / e.size ) * s->size;
		out.point = e.midpoint() + distance;

		// Set Dynamic collision data
		DynamicCollision dynamic;
		dynamic.isDynamic = true;
		dynamic.staticRect = s;
		dynamic.dynamicRect = d;

		out.dynamic = dynamic;
	}
	return out;
}

//--------------------------------------------------------------------------------

void resolveCollision( CollisionResult result ) {
	// There was no collision
	if( !result.success )
		return;

	// Static-Static and Dynamic-Dynamic can't be resolved
	if( !result.dynamic.isDynamic )
		return;

	Rect* d = result.dynamic.dynamicRect;
	Rect* s = result.dynamic.staticRect;

	d->velocity += result.normal * d->velocity.abs() * ( 1.0 - result.distance ) * 1.0000000001;
}

//================================================================================

bool staticRectCollision( Rect rectA, Rect rectB ) {
	bool out = true;

	const Math::Vec2 midPointA = rectA.position + ( rectA.size / 2.0 );

	const Math::Vec2 midPointB = rectB.position + ( rectB.size / 2.0 );

	out &= abs( midPointA.x - midPointB.x ) < ( rectA.size.x / 2.0 ) + ( rectB.size.x / 2.0 );
	out &= abs( midPointA.y - midPointB.y ) < ( rectA.size.y / 2.0 ) + ( rectB.size.y / 2.0 );

	return out;
}

//================================================================================

} // namespace Collision

//================================================================================