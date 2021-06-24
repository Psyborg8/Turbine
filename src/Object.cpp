//================================================================================

#include "Object.h"

//--------------------------------------------------------------------------------

#include "Debug.h"

//================================================================================

void Object::processCollisions( vector< shared_ptr< Object > > targets ) {
	for( shared_ptr< Object > target : targets ) {
		const Collision::CollisionResult result = isColliding( target );
		if( result.success ) {
			onCollision( result, target );
			target->onCollision( result, shared_from_this() );
		}
	}
}

//--------------------------------------------------------------------------------

void Object::resolveCollisions( vector< shared_ptr< Object > > targets, bool notify ) {
	using collisionPair = pair< shared_ptr< Object >, Collision::CollisionResult >;

	Debug::startTimer( "Collision::Broad Phase" );

	// Broad Phase
	vector < collisionPair > results;
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = isColliding( target );
		if( result.success )
			results.push_back( make_pair( target, result ) );
	}

	Debug::stopTimer( "Collision::Broad Phase" );
	Debug::startTimer( "Collision::Sorting" );

	// Sort by distance
	std::sort( results.begin(), results.end(),
			   []( const collisionPair& a, const collisionPair& b ) {
				   return a.second.distance < b.second.distance;
			   } );

	Debug::stopTimer( "Collision::Sorting" );
	Debug::startTimer( "Collision::Narrow Phase" );

	// Narrow Phase
	for( pair< shared_ptr< Object >, Collision::CollisionResult > collision : results ) {
		// Check again, in case a previous resolution means we aren't colliding anymore
		const Collision::CollisionResult result = isColliding( collision.first );
		if( result.success ) {
			// Resolve the collision
			Collision::resolveCollision( result );

			if( notify ) {
				// Notify both targets
				onCollision( result, collision.first );
				collision.first->onCollision( result, shared_from_this() );
			}
		}
	}

	Debug::stopTimer( "Collision::Narrow Phase" );
}

//--------------------------------------------------------------------------------

Math::Vec2 Object::getWorldPosition() const {
	if( m_parent == nullptr )
		return getPosition();

	return getPosition() + m_parent->getWorldPosition();
}

//--------------------------------------------------------------------------------

void Object::setWorldPosition( Math::Vec2 position ) {
	if( m_parent != nullptr )
		position -= m_parent->getWorldPosition();

	setPosition( position );
}

//================================================================================

vector< shared_ptr< Object > > Object::s_objects;
vector< shared_ptr< Object > > Object::s_markedForDeletion;

//================================================================================