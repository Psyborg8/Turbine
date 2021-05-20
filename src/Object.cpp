//================================================================================

#include "Object.h"

//================================================================================

// STATIC

//================================================================================

void Object::checkCollisions( vector< shared_ptr< Object > > objects )
{
	for( vector< shared_ptr< Object > >::iterator it = objects.begin(); it != objects.end(); ++it )
	{
		const shared_ptr< Object > object = *it;
		if( object->getCollisionType() == CollisionType::None )
			continue;

		// Avoid checking the same collision twice
		vector< shared_ptr< Object > >::iterator jt = it;
		jt++;

		while( jt != objects.end() )
		{
			shared_ptr< Object > target = *jt;
			jt++;

			if( target->getCollisionType() == CollisionType::None )
				continue;

			if( object->getCollisionType() == CollisionType::StaticBlocking &&
				target->getCollisionType() == CollisionType::StaticBlocking )
				continue;

			Vec2 normal;
			const bool isColliding = object->isColliding( target, normal );
			if( isColliding )
			{
				object->onCollision( target, normal );
				target->onCollision( object, normal * -1.0 ); // Also notify the target with the inverse normal
			}
		}
	}
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Object > > Object::s_objects;
vector< shared_ptr< Object > > Object::s_markedForDeletion;

//================================================================================