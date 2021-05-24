//================================================================================

#include "Object.h"

//================================================================================

void Object::onDestroy()
{
	vector< ObjectPtr > objects = getObjects( shared_from_this(), "" );
	for( ObjectPtr object : objects )
		object->destroy();

}

//--------------------------------------------------------------------------------

void Object::processCollisions( vector< ObjectPtr > targets )
{
	for( ObjectPtr target : targets )
	{
		const Collision::CollisionResult result = isColliding( target );
		if( result.success )
			target->onCollision( result, target );
	}
}

//--------------------------------------------------------------------------------

void Object::resolveCollisions( vector< ObjectPtr > targets, bool notify )
{
	using collisionPair = pair< ObjectPtr, Collision::CollisionResult >;

	// Broad Phase
	vector < collisionPair > results;
	for( ObjectPtr target : targets )
	{
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
	for( pair< ObjectPtr, Collision::CollisionResult > collision : results )
	{
		// Check again, in case a previous resolution means we aren't colliding anymore
		const Collision::CollisionResult result = isColliding( collision.first );
		if( result.success )
		{
			// Resolve the collision
			Collision::resolveCollision( result );

			if( notify )
			{
				// Notify both targets
				onCollision( result, collision.first );
				collision.first->onCollision( result, shared_from_this() );
			}
		}
	}
}

//================================================================================

vector< ObjectPtr > Object::s_objects;
vector< ObjectPtr > Object::s_markedForDeletion;

//================================================================================