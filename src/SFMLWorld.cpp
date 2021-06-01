//================================================================================

#include "SFMLWorld.h"

//--------------------------------------------------------------------------------

#include "Sprite.h"
#include "Tileset.h"
#include "RigidRect.h"
#include "Map.h"
#include "System.h"
#include "Player.h"
#include "Debug.h"

//================================================================================

namespace Worlds {

//================================================================================

void SFMLWorld::onSpawnChildren() {
	World::onSpawnChildren();

	m_camera.setDistance( 256.0f );

	makeObject< Debug::DebugWindow >( this );

	m_timer = makeObject< Gfx::GameTimer >( this );
	m_timer->setVisibility( false );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onStart() {
	Gfx::Map::loadMap( "Dungeon 1-1" );
	Gfx::Map::constructMap( "Dungeon 1-1", this );

	m_backgroundColor = Math::Color( sf::Color( 22u, 22u, 22u, 255u ) );

	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;
	const shared_ptr< Game::Player > player = Object::getObjects< Game::Player >().at( 0 );
	m_levelStart = player->getSpawn();

	m_timer->reset();
}

//--------------------------------------------------------------------------------

void SFMLWorld::onRender() {
	if( m_visibility );
		Gfx::Map::renderMap( "Dungeon 1-1" );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onUpdate( sf::Time deltaTime ) {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;

	const shared_ptr< Game::Player > player = Object::getObjects< Game::Player >().at( 0 );
	Math::Vec2 pos = player->getPosition();
	pos.y += 0.5;

	const Math::Vec2 direction = pos - m_camera.getPosition();
	if( direction.length() ) {
		const Math::Vec2 diff = direction * 6.0 * deltaTime.asSeconds();

		Math::Vec2 newPos = m_camera.getPosition() + diff;
		if( direction.length() < diff.length() )
			newPos = pos;

		m_camera.setPosition( newPos );
	}
}

//--------------------------------------------------------------------------------

void SFMLWorld::onEvent( sf::Event e ) {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;

	const shared_ptr< Game::Player > player = Object::getObjects< Game::Player >().at( 0 );

	if( e.type == e.KeyPressed ) {
		if( e.key.code == sf::Keyboard::R ) {
			if( e.key.shift )
				player->setSpawn( m_levelStart );
			player->kill();
		}
	}
}

//--------------------------------------------------------------------------------

void SFMLWorld::onMessage( string message ) {
	if( message == "Checkpoint" ) {
		Debug::addMessage( "Checkpoint Hit" );
		m_timer->split();
		return;
	}
	if( message == "Level End" ) {
		Debug::addMessage( "Level End Hit" );
		m_timer->stop();
		return;
	}
}

//--------------------------------------------------------------------------------

Timers::TimerID playerSpawnTimer;
void SFMLWorld::reset() {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;

	const shared_ptr< Game::Player > player = Object::getObjects< Game::Player >().at( 0 );
	Math::Vec2 spawn = player->getSpawn();
	player->destroy();

	if( Timers::timerStillActive( playerSpawnTimer ) )
		return;

	playerSpawnTimer = Timers::addTimer( 1000, nullptr,
					  [spawn, this] {
						  shared_ptr< Game::Player > player = Object::makeObject< Game::Player >( this );
						  player->setPosition( spawn );
						  player->setSpawn( spawn );
						  if( spawn == m_levelStart ) {
							  m_timer->reset();


							  vector< shared_ptr< Object > > checkpoints = Object::getObjects( nullptr, "Checkpoint" );
							  const vector< shared_ptr< Object > > levelEnds = Object::getObjects( nullptr, "Level End" );
							  checkpoints.insert( checkpoints.end(), levelEnds.begin(), levelEnds.end() );
							  for( shared_ptr< Object > checkpoint : checkpoints )
								  checkpoint->setParent( this );
						  }
					  },
					  false );
}

//================================================================================

}

//================================================================================