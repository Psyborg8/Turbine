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

	m_camera.setDistance( 196.0f );

	/*
	shared_ptr< Game::RigidRect > rect = makeObject< Game::RigidRect >( this );
	rect->setSize( Math::Vec2( 250.0f, 250.0f ) );
	rect->setPosition( Math::Vec2( 0.0f, 0.0f ) );
	rect->setColor( Colors::WHITE );
	*/

	makeObject< Debug::DebugWindow >( this );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onStart() {
	Gfx::Map::loadMap( "DungeonMap" );
	Gfx::Map::constructMap( "DungeonMap", this );

	m_backgroundColor = Math::Color( sf::Color( 22u, 22u, 22u, 255u ) );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onRender() {
	Gfx::Map::renderMap( "DungeonMap" );
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

void SFMLWorld::reset() {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;

	const shared_ptr< Game::Player > player = Object::getObjects< Game::Player >().at( 0 );
	Math::Vec2 spawn = player->getSpawn();
	player->destroy();

	Timers::addTimer( 1000, nullptr,
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