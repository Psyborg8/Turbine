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

	m_camera.setDistance( 196.0f );

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

	m_cameraTarget.y += 0.5;

	// Position
	{
		const Math::Vec2 direction = m_cameraTarget - m_camera.getPosition();
		if( direction.length() ) {
			const Math::Vec2 diff = direction * 3.0f * deltaTime.asSeconds();

			Math::Vec2 newPos = m_camera.getPosition() + diff;
			if( direction.length() < diff.length() )
				newPos = m_cameraTarget;

			m_camera.setPosition( newPos );
		}
	}

	// Distance
	{
		const float direction = m_cameraDistanceTarget - m_camera.getDistance();
		if( direction ) {
			const float diff = direction * 2.0f * deltaTime.asSeconds();

			float newDistance = m_camera.getDistance() + diff;
			if( abs( direction ) < abs( diff ) )
				newDistance = m_cameraDistanceTarget;

			m_camera.setDistance( newDistance );
		}
	}

	Debug::stopTimer( "World::Update" );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onProcessCollisions() {
	vector< shared_ptr< Object > > cameras = Gfx::Map::getObjects( m_currentMap, "Camera" );

	bool collision = false;
	for( shared_ptr< Object > camera : cameras ) {
		Collision::CollisionResult result = m_player->isColliding( camera );
		if( result.success ) {
			shared_ptr< Game::RigidRect > volume = std::dynamic_pointer_cast< Game::RigidRect >( camera );
			m_cameraTarget = volume->getPosition() + volume->getSize() / 2.0f;
			m_cameraDistanceTarget = volume->getSize().y;
			collision = true;
			break;
		}
	}

	if( !collision ) {
		m_cameraTarget = m_player->getPosition() + m_player->getSize() / 2.0f;
		m_cameraDistanceTarget = 196.0f;
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