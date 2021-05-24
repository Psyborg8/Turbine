//================================================================================

#include "GridWorld.h"

//--------------------------------------------------------------------------------

#include "Player.h"
#include "Timer.h"
#include "Checkpoint.h"

//================================================================================

namespace Worlds {

//================================================================================

GridWorld::GridWorld() : World() {
	WorldData dashPrecisionWorld;
	dashPrecisionWorld.name = "Dash Precision World";
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
	juggleTimeTower.name = "Juggle Time Tower";
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

	m_worldData.push_back( dashPrecisionWorld );
	m_worldData.push_back( juggleTimeTower );
}

//--------------------------------------------------------------------------------

void GridWorld::onSpawnChildren() {
	World::onSpawnChildren();
}

//--------------------------------------------------------------------------------

void GridWorld::onUpdate( double deltaTime ) {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;

	const shared_ptr< Game::Player > player = Object::getObjects< Game::Player >().at( 0 );
	Math::Vec2 pos = player->position;
	pos.y += 0.5;

	const Math::Vec2 direction = pos - m_camera->getPosition();
	if( direction.length() ) {
		const Math::Vec2 diff = direction * m_cameraMoveMultiplier * deltaTime;

		Math::Vec2 newPos = m_camera->getPosition() + diff;
		if( direction.length() < diff.length() )
			newPos = pos;

		m_camera->setPosition( newPos );
	}
}

//--------------------------------------------------------------------------------

void GridWorld::onStart() {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;

	const shared_ptr< Game::Player > player = players.at( 0 );

	m_camera->setDistance( 2.0 );
	m_camera->setPosition( player->position );
}

//--------------------------------------------------------------------------------

void GridWorld::reset() {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;

	const shared_ptr< Game::Player > player = players.at( 0 );
	destroyObject( player );

	Timers::addTimer( "Reset Player",
					 1000, nullptr,
					 [this]
					 {
						 shared_ptr< Game::Player > player = makeObject< Game::Player >( this );
						 player->position = m_playerStart;
					 }
					 , false );
}

//--------------------------------------------------------------------------------

void GridWorld::loadWorld( string name ) {
	const auto it = std::find_if( m_worldData.begin(), m_worldData.end(), 
								  [name]( const WorldData& data ) { 
									  return data.name == name; 
								  } );

	if( it == m_worldData.end() )
		return;

	const WorldData& data = *it;

	for( int y = 0; y < data.blockData.size(); ++y ) {
		for( int x = 0; x < data.blockData.at( y ).size(); ++x ) {
			const Math::Vec2 pos = Math::Vec2( double( x ), data.blockData.size() - double( y ) ) * data.gridSize;
			const char block = data.blockData.at( y ).at( x );

			if( block == 'w' ) {
				shared_ptr< Game::RigidRect > wall = makeObject< Game::RigidRect >( this );
				wall->position = pos;
				wall->size = Math::Vec2( data.gridSize, data.gridSize );
				wall->setName( "Wall" );

				continue;
			}
			if( block == 't' ) {
				shared_ptr< Game::RigidRect > trap = makeObject< Game::RigidRect >( this );
				trap->position = pos;
				trap->size = Math::Vec2( data.gridSize, data.gridSize );
				trap->fillColor = Colors::RED;
				trap->setName( "Trap" );

				continue;
			}
			if( block == 'p' ) {
				shared_ptr< Game::RigidRect > platform = makeObject< Game::RigidRect >( this );
				platform->size = Math::Vec2( data.gridSize, data.gridSize * 0.1 );
				platform->position = Math::Vec2( pos.x, pos.y + data.gridSize * 0.9 );
				platform->setName( "Platform" );

				continue;
			}
			if( block == 'P' ) {
				shared_ptr< Game::Player > player = makeObject< Game::Player >( this );
				player->position = pos;

				m_playerStart = pos;

				continue;
			}
			if( block >= '1' && block <= '9' ) {
				shared_ptr< Game::Checkpoint > checkpoint = makeObject< Game::Checkpoint >( this );

				const Math::Vec2 midpoint = Math::Vec2( pos.x + data.gridSize / 2.0, 0.0 );
				checkpoint->setMidpoint( midpoint );
				checkpoint->position.y = pos.y;
				checkpoint->size = Math::Vec2( data.gridSize * 0.1, data.gridSize * 1.5 );
				checkpoint->setNumber( size_t( block ) );

				continue;
			}
		}
	}
}

//--------------------------------------------------------------------------------

void GridWorld::unloadWorld() {
	vector< shared_ptr< Object > > objects = getObjects( shared_from_this(), "", true );
	for( shared_ptr< Object > object : objects )
		object->destroy();
}

//--------------------------------------------------------------------------------

void GridWorld::setCheckpoint( size_t checkpoint ) {
	vector< shared_ptr< Game::Checkpoint > > checkpoints = getObjects< Game::Checkpoint >( shared_from_this(), "", true );

	const auto it = std::find_if( checkpoints.begin(), checkpoints.end(), 
								  [checkpoint]( shared_ptr< Game::Checkpoint > in ) {
									  return in->getNumber() == checkpoint;
								  } );

	if( it == checkpoints.end() )
		return;

	m_playerStart = ( *it )->position;
}

//================================================================================

} // Worlds

//================================================================================