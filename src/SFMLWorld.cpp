//================================================================================

#include "SFMLWorld.h"

//--------------------------------------------------------------------------------

#include "Sprite.h"
#include "Tileset.h"
#include "RigidRect.h"

//================================================================================

namespace Worlds {

//================================================================================

void SFMLWorld::onSpawnChildren() {
	World::onSpawnChildren();

	m_camera.setDistance( 100.0f );
	m_camera.setPosition( Math::Vec2() );

	/*
	shared_ptr< Game::RigidRect > rect = makeObject< Game::RigidRect >( this );
	rect->setSize( Math::Vec2( 250.0f, 250.0f ) );
	rect->setPosition( Math::Vec2( 0.0f, 0.0f ) );
	rect->setColor( Colors::WHITE );
	*/
}

//--------------------------------------------------------------------------------

void SFMLWorld::onStart() {
	Gfx::Sprite::loadSprite( "stone" );
	Gfx::Tileset::loadTileset( "Stronghold_Terrain_Tileset", sf::Vector2u( 16, 16 ) );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onRender() {
	Gfx::Sprite::renderSprite( "stone", Math::Vec2( 0.0f, 0.0f ), Math::Vec2( 1.0f, 1.0f ) );
	Gfx::Tileset::renderTile( "Stronghold_Terrain_Tileset", 10, Math::Vec2( 32.0f, 32.0f ) );
	Gfx::Tileset::renderTile( "Stronghold_Terrain_Tileset", 23, Math::Vec2( 32.0f, 0.0f ) );
}

//================================================================================

}

//================================================================================