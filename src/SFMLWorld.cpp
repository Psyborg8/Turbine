//================================================================================

#include "SFMLWorld.h"

//--------------------------------------------------------------------------------

#include "Sprite.h"
#include "Tileset.h"
#include "RigidRect.h"
#include "Map.h"
#include "System.h"

//================================================================================

namespace Worlds {

//================================================================================

void SFMLWorld::onSpawnChildren() {
	World::onSpawnChildren();

	m_camera.setDistance( 512.0f );
	m_camera.setPosition( Math::Vec2( 256.0f, 256.0f ) );

	/*
	shared_ptr< Game::RigidRect > rect = makeObject< Game::RigidRect >( this );
	rect->setSize( Math::Vec2( 250.0f, 250.0f ) );
	rect->setPosition( Math::Vec2( 0.0f, 0.0f ) );
	rect->setColor( Colors::WHITE );
	*/
}

//--------------------------------------------------------------------------------

void SFMLWorld::onStart() {
	Gfx::Map::loadMap( "DungeonMap" );
	Gfx::Map::constructMap( "DungeonMap", this );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onRender() {
	Gfx::Map::renderMap( "DungeonMap" );
	Gfx::Tileset::renderTile( "Dungeon Terrain", 50, Math::Vec2() );
}

//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------

void SFMLWorld::onUpdate( sf::Time deltaTime ) {
	m_camera.setPosition( m_camera.getPosition() + Math::Vec2( 16.0f, 0.0f ) * deltaTime.asSeconds() );
	sf::Vector2i screenPos = sf::Mouse::getPosition();
	m_camera.setPosition( Math::Vec2( screenPos.x / 100.0f, screenPos.y / 100.0f ) + Math::Vec2( 128.0f, 128.0f ) );
}

//================================================================================

}

//================================================================================