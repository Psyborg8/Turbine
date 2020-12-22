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
	{
		Player player = Player();
		player.setColor( Color( 1.0, 1.0, 1.0, 1.0 ) );
		
		m_player = Object::makeObject< Player >( player, this );
	}

	{
		Color white = Color( 0.7f, 0.7f, 0.7f, 1.0 );

		{
			Wall wall = Wall( 1.6f, 0.1f, Vec2( 0.0f, 0.0f ), 0.0f, white );
			m_walls.push_back( makeObject< Wall >( wall, this ) );
		}

		{
			Wall wall = Wall( 0.1f, 1.6f, Vec2( 0.0f, 1.0f ), 0.0f, white );
			m_walls.push_back( makeObject< Wall >( wall, this ) );
		}

		{
			Wall wall = Wall( 1.6f, 0.1f, Vec2( 1.6f, 0.0f ), 0.0f, white );
			m_walls.push_back( makeObject< Wall >( wall, this ) );
		}

		{
			Wall wall = Wall( 0.1f, 1.6f, Vec2( 0.0f, 0.0f ), 0.0f, white );
			m_walls.push_back( makeObject< Wall >( wall, this ) );
		}
	}
}

//--------------------------------------------------------------------------------

void DevWorld::onStart()
{
	//
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