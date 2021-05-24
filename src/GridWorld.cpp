//================================================================================

#include "GridWorld.h"

//--------------------------------------------------------------------------------

#include "Player.h"
#include "Timer.h"

//================================================================================

GridWorld::GridWorld() : World() {
	WorldData dashPrecisionWorld;
	dashPrecisionWorld.gridSize = 0.35;
	dashPrecisionWorld.cameraDistance = 3.0;
	dashPrecisionWorld.blockData = {
		"wwwwwwwwwwwwwwwwwwww                   ",
		"t            t     w                   ",
		"t                  w                   ",
		"t      w     t     w                   ",
		"t      wttttttwpppwwwwwwwwwwwwwwwwwwwww",
		"t      wwwwwwww   wwwwtttwwwttt       w",
		"w      wwwwwwww                     2 w",
		"wwwppwwwwwwwwww       ttt   ttt     ppw",
		"w P    wwwwwwww   1   www   www       w",
		"wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"
	};

	WorldData juggleTimeTower;
	juggleTimeTower.gridSize = 0.3;
	juggleTimeTower.cameraDistance = 2.0;
	juggleTimeTower.blockData = {
		"wwwwwwwwwwwwwww",
		"w             w",
		"w             w",
		"w         3   w",
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
		"w      2      w",
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
		"w         1  tw",
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
	};

	loadWorld( juggleTimeTower );
}

//--------------------------------------------------------------------------------

void GridWorld::onSpawnChildren() {
	World::onSpawnChildren();
}

//--------------------------------------------------------------------------------

void GridWorld::onUpdate( double deltaTime ) {
	const vector< shared_ptr< Player > > players = Object::getObjects< Player >();
	if( players.empty() )
		return;

	const shared_ptr< Player > player = Object::getObjects< Player >().at( 0 );
	Vec2 pos = player->position;
	pos.y += 0.5;

	const Vec2 direction = pos - m_camera->getPosition();
	if( direction.length() ) {
		const Vec2 diff = direction * m_cameraMoveMultiplier * deltaTime;

		Vec2 newPos = m_camera->getPosition() + diff;
		if( direction.length() < diff.length() )
			newPos = pos;

		m_camera->setPosition( newPos );
	}
}

//--------------------------------------------------------------------------------

void GridWorld::onStart() {
	const vector< shared_ptr< Player > > players = Object::getObjects< Player >();
	if( players.empty() )
		return; 

	const shared_ptr< Player > player = players.at( 0 );

	m_camera->setDistance( 2.0 );
	m_camera->setPosition( player->position );
}

//--------------------------------------------------------------------------------

void GridWorld::reset() {
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

//--------------------------------------------------------------------------------

void GridWorld::loadWorld( WorldData data ) {
	for( int y = 0; y < data.blockData.size(); ++y ) {
		for( int x = 0; x < data.blockData.at( y ).size(); ++x ) {
			const Vec2 pos = Vec2( double( x ), data.blockData.size() - double( y ) ) * data.gridSize;
			const char block = data.blockData.at( y ).at( x );

			if( block == 'w' ) {
				shared_ptr< RigidRect > wall = makeObject< RigidRect >( this );
				wall->position = pos;
				wall->size = Vec2( data.gridSize, data.gridSize );
				wall->setName( "Wall" );

				continue;
			}
			if( block == 't' ) {
				shared_ptr< RigidRect > trap = makeObject< RigidRect >( this );
				trap->position = pos;
				trap->size = Vec2( data.gridSize, data.gridSize );
				trap->fillColor = Colors::RED;
				trap->setName( "Trap" );

				continue;
			}
			if( block == 'p' ) {
				shared_ptr< RigidRect > platform = makeObject< RigidRect >( this );
				platform->size = Vec2( data.gridSize, data.gridSize * 0.1 );
				platform->position = Vec2( pos.x, pos.y + data.gridSize * 0.9 );
				platform->setName( "Platform" );
			}
			if( block == 'P' ) {
				shared_ptr< Player > player = makeObject< Player >( this );
				player->position = pos;

				m_playerStart = pos;

				continue;
			}
			if( block >= '1' && block <= '9' ) {

			}
		}
	}
}

//--------------------------------------------------------------------------------

void GridWorld::unloadWorld() {
	vector< ObjectPtr > objects = getObjects( shared_from_this(), "", true );
	for( ObjectPtr object : objects )
		object->destroy();
}

//================================================================================