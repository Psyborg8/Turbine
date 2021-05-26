//================================================================================

#include "SFMLWorld.h"

//--------------------------------------------------------------------------------

#include "SpriteLoader.h"
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
	Gfx::SpriteLoader::loadSprite( "stone" );
}

//--------------------------------------------------------------------------------

void SFMLWorld::onRender() {
	Gfx::SpriteLoader::renderSprite( "stone", Math::Vec2( 0.0f, 0.0f ), Math::Vec2( 1.0f, 1.0f ) );
}

//================================================================================

}

//================================================================================