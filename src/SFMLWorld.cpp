//================================================================================

#include "SFMLWorld.h"

//--------------------------------------------------------------------------------

#include "Sprite.h"
#include "Tileset.h"
#include "RigidRect.h"
#include "Map.h"
#include "System.h"
#include "Debug.h"

//================================================================================

namespace Worlds {

//================================================================================

void SFMLWorld::onSpawnChildren() {
	World::onSpawnChildren();

	m_camera.setDistance( 256.0f );

	m_physicsPage = makeObject< Debug::PhysicsWindow >( this );
	m_physicsPage->setVisibility( false );

	m_performancePage = makeObject< Debug::PerformanceWindow >( this );
	m_performancePage->setVisibility( false );

	m_joystickPage = makeObject< Debug::JoystickWindow >( this );
	m_joystickPage->setVisibility( false );

	m_timer = makeObject< Gfx::GameTimer >( this );
	m_timer->setVisibility( true );

	m_visibility = true;
}

//--------------------------------------------------------------------------------

void SFMLWorld::onStart() {
	m_currentMap = "Dungeon 1-2";
	Gfx::Map::loadMap( m_currentMap );
	Gfx::Map::constructMap( m_currentMap, this );
	m_backgroundColor = Math::Color( sf::Color( 22u, 22u, 22u, 255u ) );

	m_player = Object::getObjects< Game::Player >().at( 0 );
	m_levelStart = m_player->getSpawn();

	m_timer->reset();
}

//--------------------------------------------------------------------------------

void SFMLWorld::onRender() {
	if( m_visibility )
		Gfx::Map::renderMap( m_currentMap );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onUpdate( sf::Time deltaTime ) {
	Debug::startTimer( "World::Update" );
	Math::Vec2 pos = m_player->getPosition();
	pos.y += 0.5;

	const Math::Vec2 direction = pos - m_camera.getPosition();
	if( direction.length() ) {
		const Math::Vec2 diff = direction * 6.0 * deltaTime.asSeconds();

		Math::Vec2 newPos = m_camera.getPosition() + diff;
		if( direction.length() < diff.length() )
			newPos = pos;

		m_camera.setPosition( newPos );
	}
	Debug::stopTimer( "World::Update" );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onEvent( sf::Event e ) {
	//
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
	if( message == "Player Restart" ) {
		m_player->setSpawn( m_levelStart );
	}
	if( message == "Flip Debug Page" ) {
		m_timer->setVisibility( false );
		m_physicsPage->setVisibility( false );
		m_performancePage->setVisibility( false );
		m_joystickPage->setVisibility( false );

		if( m_debugPage == 0 )
			m_physicsPage->setVisibility( true );
		else if( m_debugPage == 1 )
			m_performancePage->setVisibility( true );
		else if( m_debugPage == 2 )
			m_joystickPage->setVisibility( true );
		else
			m_timer->setVisibility( true );

		m_debugPage++;
		if( m_debugPage > 3 )
			m_debugPage = 0;
	}
}

//--------------------------------------------------------------------------------

Timers::TimerID playerSpawnTimer;
void SFMLWorld::reset() {
	Math::Vec2 spawn = m_player->getSpawn();
	m_player->destroy();

	if( Timers::timerStillActive( playerSpawnTimer ) )
		return;

	playerSpawnTimer = Timers::addTimer( 1000, nullptr,
										 [spawn, this] {
											 m_player = Object::makeObject< Game::Player >( this );
											 m_player->setPosition( spawn );
											 m_player->setSpawn( spawn );
											 if( spawn == m_levelStart ) {
												 m_timer->reset();

												 vector< shared_ptr< Object > > checkpoints = Gfx::Map::getObjects( m_currentMap, "Checkpoint" );
												 for( shared_ptr< Object > checkpoint : checkpoints )
													 checkpoint->setCollisionType( CollisionType::Static );
											 }
										 },
										 false );
}

//================================================================================

}

//================================================================================