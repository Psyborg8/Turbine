//================================================================================

#include "DevWorld.h"

//--------------------------------------------------------------------------------

#include "Box.h"
#include "Player.h"

//================================================================================

DevWorld::DevWorld() : World()
{
	m_name = "DevWorld";
}

//--------------------------------------------------------------------------------

void DevWorld::onSpawnChildren()
{
	World::onSpawnChildren();

	{
		Box wall = Box( Vec2( -1.0, -1.0 ), Vec2( 2.0, 0.1 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( -1.0, 0.9 ), Vec2( 2.0, 0.1 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( -1.0, -1.0 ), Vec2( 0.1, 2.0 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( 1.0, -1.0 ), Vec2( 0.1, 2.0 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Object::makeObject< Player >( "Player", this );
	}
}
