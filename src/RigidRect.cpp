//================================================================================

#include "RigidRect.h"

//--------------------------------------------------------------------------------

#include "Collision.h"
#include "System.h"

//================================================================================

namespace Game {

//================================================================================

RigidRect::RigidRect() : Object() {
	//
}

//--------------------------------------------------------------------------------

RigidRect::RigidRect( Math::Vec2 position, Math::Vec2 size, Math::Color color = Colors::WHITE ) :  Object() {
	m_rect.setScale( size.sf() );
	m_rect.setPosition( position.sf() );
	m_color = color;
}

//--------------------------------------------------------------------------------

void RigidRect::onRender() {
	System::getWindow()->draw( m_rect );
}

//--------------------------------------------------------------------------------

Collision::CollisionResult RigidRect::isColliding( shared_ptr< Object > target ) {
	Collision::CollisionResult out;

	if( m_collisionType == CollisionType::None )
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

//================================================================================

} // Game

//================================================================================