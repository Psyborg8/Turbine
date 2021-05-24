//================================================================================

#include "GridWorld.h"

//--------------------------------------------------------------------------------

#include "Player.h"
#include "Timer.h"

//================================================================================

GridWorld::GridWorld() : World()
{
	pair< vector< string >, double > dashPrecisionWorld = { {
		"wwwwwwwwwwwwwwwwwwww                   ",
		"t            t     w                   ",
		"t                  w                   ",
		"t      w     t     w                   ",
		"t      wttttttwpppwwwwwwwwwwwwwwwwwwwww",
		"t      wwwwwwww   wwwwtttwwwttt       w",
		"w      wwwwwwww                       w",
		"wwwppwwwwwwwwww       ttt   ttt     ppw",
		"w P    wwwwwwww       www   www       w",
		"wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"
	}, 0.35 };

	pair< vector< string >, double > juggleTimeTower = { {
		"wwwwwwwwwwwwwww",
		"w             w",
		"w             w",
		"w             w",
		"wwwwwww wwwwwww",
		"wtttttt ttttttw",
		"w             w",
		"w             w",
		"w             w",
		"wtttttttt   ttw",
		"w             w",
		"w             w",
		"w             w",
		"w             w",
		"w             w",
		"wtt   ttttttttw",
		"w             w",
		"w             w",
		"w             w",
		"w             w",
		"w w    w    w w",
		"w             w",
		"w             w",
		"w             w",
		"w             w",
		"w             w",
		"w   ww   ww   w",
		"w            tw",
		"wttttttt     ww",
		"w      t     ww",
		"w            ww",
		"w            tw",
		"w      t  ww  w",
		"w      tttttttw",
		"w     ww      w",
		"w             w",
		"w             w",
		"w             w",
		"w             w",
		"w             w",
		"wttttt   tttttw",
		"wwwwww P wwwwww",
		"wwwwwwwwwwwwwww",
	}, 0.20 };

	const pair< vector< string >, double >& world = juggleTimeTower;

	m_worldData = world.first;
	m_gridSize = world.second;
}

//--------------------------------------------------------------------------------

void GridWorld::onSpawnChildren()
{
	World::onSpawnChildren();

	for( int y = 0; y < m_worldData.size(); ++y )
	{
		for( int x = 0; x < m_worldData.at( y ).size(); ++x )
		{
			const Vec2 pos = Vec2( double( x ), m_worldData.size() - double( y ) ) * m_gridSize;
			const char block = m_worldData.at( y ).at( x );

			if( block == 'w' )
			{
				shared_ptr< RigidRect > wall = makeObject< RigidRect >( this );
				wall->position = pos;
				wall->size = Vec2( m_gridSize, m_gridSize );
				wall->setName( "Wall" );

				continue;
			}
			if( block == 't' )
			{
				shared_ptr< RigidRect > trap = makeObject< RigidRect >( this );
				trap->position = pos;
				trap->size = Vec2( m_gridSize, m_gridSize );
				trap->fillColor = Colors::RED;
				trap->setName( "Trap" );

				continue;
			}
			if( block == 'p' )
			{
				shared_ptr< RigidRect > platform = makeObject< RigidRect >( this );
				platform->size = Vec2( m_gridSize, m_gridSize * 0.1 );
				platform->position = Vec2( pos.x, pos.y + m_gridSize * 0.9 );
				platform->setName( "Platform" );
			}
			if( block == 'P' )
			{
				shared_ptr< Player > player = makeObject< Player >( this );
				player->position = pos;

				m_playerStart = pos;

				continue;
			}
			if( block >= '1' && block <= '9' )
			{
				
			}
		}
	}
}

//--------------------------------------------------------------------------------

void GridWorld::onUpdate( double deltaTime )
{
	const vector< shared_ptr< Player > > players = Object::getObjects< Player >();
	if( players.empty() )
		return;

	const shared_ptr< Player > player = Object::getObjects< Player >().at( 0 );
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

//--------------------------------------------------------------------------------

void GridWorld::onStart()
{
	const vector< shared_ptr< Player > > players = Object::getObjects< Player >();
	if( players.empty() )
		return; 

	const shared_ptr< Player > player = players.at( 0 );

	m_camera->setDistance( 2.0 );
	m_camera->setPosition( player->position );
}

//--------------------------------------------------------------------------------

void GridWorld::reset()
{
	const vector< shared_ptr< Player > > players = Object::getObjects< Player >();
	if( players.empty() )
		return;

	const shared_ptr< Player > player = players.at( 0 );
	destroyObject( player );

	Timer::addTimer( "Reset Player",
					 1000, nullptr,
					 [this]
					 {
						 shared_ptr< Player > player = makeObject< Player >( this );
						 player->position = m_playerStart;
					 }
					 , false );
}

//================================================================================