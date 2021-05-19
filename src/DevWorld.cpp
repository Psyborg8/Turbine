//================================================================================

#include "DevWorld.h"

//--------------------------------------------------------------------------------

#include "Box.h"
#include "Player.h"

//================================================================================

DevWorld::DevWorld() : World()
{
	m_name = "DevWorld";
	m_gravity = Vec2( 0.0, -6.0 );
}

//--------------------------------------------------------------------------------

void DevWorld::onSpawnChildren()
{
	World::onSpawnChildren();

	m_camera->setDistance( 3.0 );

	{
		Box wall = Box( Vec2( -2.0, -2.0 ), Vec2( 4.0, 0.2 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( -2.0, 1.8 ), Vec2( 4.0, 0.2 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( -2.0, -2.0 ), Vec2( 0.2, 4.0 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( 2.0, -2.0 ), Vec2( 0.2, 4.0 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( -0.8, 0.0 ), Vec2( 0.6, 0.2 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Box wall = Box( Vec2( 0.4, -1.0 ), Vec2( 0.6, 0.2 ), Colors::WHITE );
		wall.setCollisionType( CollisionType::StaticBlocking );
		Object::makeObject< Box >( wall, this );
	}
	{
		Object::makeObject< Player >( "Player", this );
	}
}
