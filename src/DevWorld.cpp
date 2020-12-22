//================================================================================

#include "DevWorld.h"

//================================================================================

// CLASS - DevWorld

//================================================================================

DevWorld::DevWorld() : World()
{
	//
}

//--------------------------------------------------------------------------------

void DevWorld::onSpawnChildren()
{
	World::onSpawnChildren();

	{
		Player player = Player();
		player.setColor( Color( 1.0, 1.0, 1.0, 1.0 ) );
		
		m_player = makeObject< Player >( player, this );
	}

	{
		Wall wall = Wall( 0.5f, 20.5f, b2Vec2( -40.0f, 0.0f ), 0.0f );
		m_walls.push_back( makeObject< Wall >( wall, this ) );
	}

	{
		Wall wall = Wall( 40.5f, 0.5f, b2Vec2( 0.0f, 20.0f ), 0.0f );
		m_walls.push_back( makeObject< Wall >( wall, this ) );
	}

	{
		Wall wall = Wall( 0.5f, 20.5f, b2Vec2( 40.0f, 0.0f ), 0.0f );
		m_walls.push_back( makeObject< Wall >( wall, this ) );
	}

	{
		Wall wall = Wall( 40.5f, 0.5f, b2Vec2( 0.0f, -20.0f ), 0.0f );
		m_walls.push_back( makeObject< Wall >( wall, this ) );
	}
}

//--------------------------------------------------------------------------------

void DevWorld::onStart()
{
	m_camera->setPosition( b2Vec2( 0.0f, 0.0f ) );
	m_camera->setDistance( 40.0f );
}

//--------------------------------------------------------------------------------

void DevWorld::onUpdate( float deltaTime )
{
	//
}

//--------------------------------------------------------------------------------

void DevWorld::onRender()
{
	//
}

//================================================================================