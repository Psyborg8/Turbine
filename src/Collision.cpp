//================================================================================

#include "Collision.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "RigidRect.h"

//================================================================================

namespace Collision {

//================================================================================

bool staticRectCollision( shared_ptr< Game::RigidRect > rectA, shared_ptr< Game::RigidRect > rectB );

//================================================================================

bool collision( Math::Vec2 point, shared_ptr< Game::RigidRect > rect ) {
	return ( point.x >= rect->getWorldPosition().x ) &&
		( point.x <= rect->getWorldPosition().x + rect->getSize().x ) &&
		( point.y >= rect->getWorldPosition().y ) &&
		( point.y <= rect->getWorldPosition().y + rect->getSize().y );
}

//--------------------------------------------------------------------------------

CollisionResult collision( Ray ray, shared_ptr< Game::RigidRect > rect ) {
	CollisionResult out;
	out.success = false;

	// Calculate inverse 
	Math::Vec2 inverseDirection = ray.direction().inverse();

	// Find near and far collisions
	Math::Vec2 tNear = ( Math::Vec2( rect->getWorldPosition() ) - ray.start ) * inverseDirection;
	Math::Vec2 tFar = ( Math::Vec2( rect->getWorldPosition() ) +
						Math::Vec2( rect->getSize() ) - ray.start ) * inverseDirection;

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
	float tHitNear = std::max( tNear.x, tNear.y );
	float tHitFar = std::min( tFar.x, tFar.y );

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

CollisionResult collision( shared_ptr< Game::RigidRect > a, shared_ptr< Game::RigidRect > b ) {
	CollisionResult out;
	out.success = false;

	sf::RectangleShape& rectA = a->getRect();
	sf::RectangleShape& rectB = b->getRect();

	// Dynamic-Dynamic collision not yet implemented.
	if( a->getVelocity().length() > 0.0 && b->getVelocity().length() > 0.0 )
		return out;

	// Narrow Phase
	shared_ptr< Game::RigidRect > d = a->getVelocity().length() > 0.0 ? a : b;
	shared_ptr< Game::RigidRect > s = a->getVelocity().length() == 0.0 ? a : b;

	if( &s == &d ) {
		// Static collision
		out.success = staticRectCollision( d, s );
		return out;
	}

	// Create a combined radius
	Game::RigidRect e;
	sf::RectangleShape& eRect = e.getRect();
	e.setPosition( s->getWorldPosition() - s->getSize() / 2.0 );
	e.setSize( s->getSize() + d->getSize() );

	// Do a ray cast to the combined rect
	Ray rayCollider;
	rayCollider.start = s->getWorldPosition() + s->getSize() / 2.0;
	rayCollider.end = rayCollider.start + ( d->getVelocity() * System::getDeltaTime().asSeconds() );

	out = collision( rayCollider, make_shared< Game::RigidRect >( e ) );
	if( out.success ) {

		out.success = false;
		if( out.normal.x > 0.0 )
			if( d->getWorldPosition().x == s->getWorldPosition().x + s->getSize().x )
				return out;
		if( out.normal.x < 0.0 )
			if( d->getWorldPosition().x + d->getSize().x == s->getWorldPosition().x )
				return out;
		if( out.normal.y > 0.0 )
			if( d->getWorldPosition().y == s->getWorldPosition().y + s->getSize().y )
				return out;
		if( out.normal.y < 0.0 )
			if( d->getWorldPosition().y + d->getSize().y == s->getWorldPosition().y )
				return out;

		out.success = true;

		// Set point and distance data
		Math::Vec2 distance = out.point - ( e.getWorldPosition() + e.getSize() / 2.0 );
		distance = ( distance / e.getSize() ) * s->getSize();
		out.point = ( e.getWorldPosition() + e.getSize() / 2.0 ) + distance;

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

	shared_ptr< Game::RigidRect > d = result.dynamic.dynamicRect;
	shared_ptr< Game::RigidRect > s = result.dynamic.staticRect;

	d->setVelocity( d->getVelocity() + result.normal * d->getVelocity().abs() * ( 1.0f - result.distance ) * 1.001f );
}

//================================================================================

bool staticRectCollision( shared_ptr< Game::RigidRect > rectA, shared_ptr< Game::RigidRect > rectB ) {
	bool out = true;

	const Math::Vec2 midPointA = rectA->getWorldPosition() + ( rectA->getSize() / 2.0 );

	const Math::Vec2 midPointB = rectB->getWorldPosition() + ( rectB->getSize() / 2.0 );

	out &= abs( midPointA.x - midPointB.x ) < ( rectA->getWorldPosition().x / 2.0 ) + ( rectB->getSize().x / 2.0 );
	out &= abs( midPointA.y - midPointB.y ) < ( rectA->getWorldPosition().y / 2.0 ) + ( rectB->getSize().y / 2.0 );

	return out;
}

//================================================================================

} // namespace Collision

//================================================================================