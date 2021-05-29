//================================================================================

#include "Object.h"

//--------------------------------------------------------------------------------

#include "Debug.h"

//================================================================================

void Object::onDestroy() {
	vector< shared_ptr< Object > > objects = getObjects( shared_from_this(), "" );
	for( shared_ptr< Object > object : objects )
		object->destroy();

}

//--------------------------------------------------------------------------------

void Object::processCollisions( vector< shared_ptr< Object > > targets ) {
	for( shared_ptr< Object > target : targets ) {
		const Collision::CollisionResult result = isColliding( target );
		if( result.success )
			target->onCollision( result, target );
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
			// Notify first so we have collision speed
			if( notify ) {
				// Notify both targets
				onCollision( result, collision.first );
				collision.first->onCollision( result, shared_from_this() );
			}

			// Resolve the collision
			Collision::resolveCollision( result );
		}
	}

	Debug::stopTimer( "Collision::Narrow Phase" );
}

//================================================================================

vector< shared_ptr< Object > > Object::s_objects;
vector< shared_ptr< Object > > Object::s_markedForDeletion;

//================================================================================