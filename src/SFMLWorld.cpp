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

	// makeObject< Debug::DebugWindow >( this );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onStart() {
	Gfx::Map::loadMap( "Dungeon_1-1" );
	Gfx::Map::constructMap( "Dungeon_1-1", this );

	m_backgroundColor = Math::Color( sf::Color( 22u, 22u, 22u, 255u ) );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onRender() {
	Gfx::Map::renderMap( "Dungeon_1-1" );
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
					  },
					  false );
}

//================================================================================

}

//================================================================================