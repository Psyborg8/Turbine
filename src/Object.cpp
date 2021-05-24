//================================================================================

#include "Object.h"

//================================================================================

namespace Game {

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

	// Broad Phase
	vector < collisionPair > results;
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = isColliding( target );
		if( result.success )
			results.push_back( make_pair( target, result ) );
	}

	// Sort by distance
	std::sort( results.begin(), results.end(),
			   []( const collisionPair& a, const collisionPair& b ) {
				   return a.second.distance < b.second.distance;
			   } );

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
}

//================================================================================

vector< shared_ptr< Object > > Object::s_objects;
vector< shared_ptr< Object > > Object::s_markedForDeletion;

//================================================================================

} // Game

//================================================================================