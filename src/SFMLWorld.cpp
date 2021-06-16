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
	Gfx::Tileset::loadTileset( "Button Prompts", Math::Vec2( 64.0f, 64.0f ), Folders::Tilesets + "UI\\Button Prompts.png" );

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

	for( const string name : m_activeEvents )
		if( name == "Dash Prompt" )
			Gfx::Tileset::renderTile( "Button Prompts", 2u, System::getWindow(), m_dashPromptPosition, Math::Vec2( 0.5f, 0.5f ) );
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

	m_cameraTarget = m_player->getPosition() + m_player->getSize() / 2.0f;
	m_cameraDistanceTarget = 196.0f;

	Debug::stopTimer( "World::Update" );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onProcessCollisions() {
	vector< shared_ptr< Object > > events = Gfx::Map::getObjects( m_currentMap, "Event" );

	for( shared_ptr< Object > event : events ) {
		Collision::CollisionResult result = m_player->isColliding( event );
		if( result.success ) {
			const string name = event->getName();

			if( name == "Dash Prompt" ) {
				const float speed = m_speed;
				m_dashPromptTimer = Timers::addTimer( 100,
								  [this, speed]( float alpha ) {
									  m_speed = speed - ( 0.99f * alpha );
								  },
								  nullptr,
								  false );

				event->setCollisionType( CollisionType::None );

				m_activeEvents.push_back( "Dash Prompt" );

				shared_ptr< Game::RigidRect > ptr = std::dynamic_pointer_cast< Game::RigidRect >( event );
				m_dashPromptPosition.x = ptr->getPosition().x + 64.0f;
				m_dashPromptPosition.y = ptr->getPosition().y - 16.0f;
			}
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
	if( message == "Player Restart" ) {
		m_player->setSpawn( m_levelStart );
		return;
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

		return;
	}
	if( message == "Dash" ) {
		const auto it = std::find( m_activeEvents.begin(), m_activeEvents.end(), "Dash Prompt" );
		if( it == m_activeEvents.end() )
			return;

		Timers::removeTimer( m_dashPromptTimer );
		m_speed = 1.0f;
		m_activeEvents.erase( it );

		return;
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
												 vector< shared_ptr< Object > > events = Gfx::Map::getObjects( m_currentMap, "Event" );
												 
												 checkpoints.insert( checkpoints.end(), events.begin(), events.end() );

												 for( shared_ptr< Object > checkpoint : checkpoints )
													 checkpoint->setCollisionType( CollisionType::Static );
											 }
										 },
										 false );
}

//================================================================================

}

//================================================================================