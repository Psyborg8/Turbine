//================================================================================

#include "rigidrect.h"

//--------------------------------------------------------------------------------

#include "collision.h"
#include "system.h"
#include "tileset.h"
#include "debug.h"

//================================================================================

namespace Game {

//--------------------------------------------------------------------------------

RigidRect::RigidRect() : Object() {
	//
}

//--------------------------------------------------------------------------------

RigidRect::RigidRect( Math::Vec2 position, Math::Vec2 size, Math::Color color = Colors::WHITE ) :  Object() {
	m_rect = sf::RectangleShape();
	m_rect.setSize( size.sf() );
	m_rect.setPosition( position.sf() );
	m_rect.setFillColor( color.sf() );
}

//--------------------------------------------------------------------------------

void RigidRect::onRender( sf::RenderTarget* target ) {
	target->draw( m_rect );
	Debug::incDrawCall();
}

//--------------------------------------------------------------------------------

Collision::CollisionResult RigidRect::isColliding( shared_ptr< Object > target ) {
	Collision::CollisionResult out;

	if( m_collisionType == CollisionType::None || target->getCollisionType() == CollisionType::None )
		return out;

	if( target == shared_from_this() )
		return out;

	{
		shared_ptr< RigidRect > ptr = std::dynamic_pointer_cast< RigidRect >( target );
		if( ptr != nullptr )
			out = Collision::collision( ptr, std::dynamic_pointer_cast< RigidRect >( shared_from_this() ) );
	}

	return out;
}

//--------------------------------------------------------------------------------

} // Game

//================================================================================