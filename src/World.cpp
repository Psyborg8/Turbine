//================================================================================

#include "World.h"

//================================================================================

World::World() : Object(), m_gravity( Vec2() ), m_b2World( m_gravity )
{

}

//--------------------------------------------------------------------------------

void World::onSpawnChildren()
{
	{
		Camera camera = Camera( b2Vec2( 0.0f, 0.0f ), 1.0f );
		m_camera = makeObject< Camera >( camera, this );
	}
}

//--------------------------------------------------------------------------------

void World::onUpdate( float deltaTime )
{
	//
}

//================================================================================
