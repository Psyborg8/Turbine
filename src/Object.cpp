//================================================================================

#include "Object.h"

//================================================================================

void Object::processCollisions( vector< shared_ptr< Object > > targets )
{
	for( shared_ptr< Object > target : targets )
	{
		const Collision::CollisionResult result = isColliding( target );
		if( result.success )
		{
			onCollision( result );
			target->onCollision( result );
		}
	}
}

//--------------------------------------------------------------------------------

void Object::resolveCollisions( vector< shared_ptr< Object > > targets, bool notify )
{
	using collisionPair = pair< shared_ptr< Object >, Collision::CollisionResult >;

	vector < collisionPair > results;
	for( shared_ptr< Object > target : targets )
	{
		Collision::CollisionResult result = isColliding( target );
		if( result.success )
			results.push_back( make_pair( target, result ) );
	}

	std::sort( results.begin(), results.end(),
			   []( const collisionPair& a, const collisionPair& b ) {
				   return a.second.distance < b.second.distance;
			   } );

	for( pair< shared_ptr< Object >, Collision::CollisionResult > collision : results )
	{
		const Collision::CollisionResult result = isColliding( collision.first );
		if( result.success )
		{
			Collision::resolveCollision( result );

			if( notify )
			{
				onCollision( result );
				collision.first->onCollision( result );
			}
		}
	}
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Object > > Object::sortByDistance( vector< shared_ptr< Object > > targets )
{
	vector< shared_ptr< Object > > out = targets;
	vector< pair< shared_ptr< Object >, double > > distances;

	for( shared_ptr< Object > target : targets )
	{
		const Collision::CollisionResult result = isColliding( target );
		if( result.success )
			distances.push_back( make_pair( target, result.distance ) );
	}

	std::sort( distances.begin(), distances.end(),
			   []( const pair< shared_ptr< Object >, double >& a, const pair< shared_ptr< Object >, double >& b )
			   {
				   return a.second < b.second;
			   } );

	return out;
}

//================================================================================

vector< shared_ptr< Object > > Object::s_objects;
vector< shared_ptr< Object > > Object::s_markedForDeletion;

//================================================================================