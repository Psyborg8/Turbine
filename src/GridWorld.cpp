//================================================================================

#include "GridWorld.h"

//--------------------------------------------------------------------------------

#include "Player.h"

//================================================================================

GridWorld::GridWorld() : World()
{
	m_worldData = {
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
		"W                                     W",
		"W                           WWWWWWWWWWW",
		"W      WWW                  W         W",
		"W      W                    W         W",
		"W    WWWWW      WWWWWWWWWWWWW         W",
		"W      W                              W",
		"WWW    W        WWW   WWW  WWW      WWW",
		"W P                                   W",
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	};

	m_gravity = Vec2( 0.0, -6.0 );
}

//--------------------------------------------------------------------------------

void GridWorld::onSpawnChildren()
{
	World::onSpawnChildren();

	for( int y = 0; y < m_worldData.size(); ++y )
	{
		for( int x = 0; x < m_worldData.at( y ).size(); ++x )
		{
			const Vec2 pos = Vec2( x, m_worldData.size() - y ) * m_gridSize;
			const char block = m_worldData.at( y ).at( x );

			if( block == 'W' )
			{
				shared_ptr< RigidRect > wall = makeObject< RigidRect >( this );
				wall->position = pos;
				wall->size = Vec2( m_gridSize, m_gridSize );
			}
			else if( block == 'P' )
			{
				shared_ptr< Player > player = makeObject< Player >( this );
				player->position = pos;
			}
		}
	}
}

void GridWorld::onStart()
{
	const shared_ptr< Player > player = Object::getObjectsByType< Player >().at( 0 );
	m_camera->setDistance( 4.0 );
	m_camera->setPosition( player->position );
}

//--------------------------------------------------------------------------------

void GridWorld::onUpdate( double deltaTime )
{
	const shared_ptr< Player > player = Object::getObjectsByType< Player >().at( 0 );
	Vec2 pos = player->position;
	pos.y += 0.5;

	const Vec2 direction = pos - m_camera->getPosition();
	if( direction.length() )
	{
		const Vec2 diff = direction * m_cameraMoveMultiplier * deltaTime;

		Vec2 newPos = m_camera->getPosition() + diff;
		if( direction.length() < diff.length() )
			newPos = pos;

		m_camera->setPosition( newPos );
	}
}