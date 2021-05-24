//================================================================================

#include "RigidRect.h"

//--------------------------------------------------------------------------------

#include "Collision.h"

//================================================================================

namespace Game {

//================================================================================

RigidRect::RigidRect() : Gfx::Rect(), Object() {
	//
}

//--------------------------------------------------------------------------------

RigidRect::RigidRect( Math::Vec2 _position, Math::Vec2 _size, Math::Color _color = Colors::WHITE ) : Gfx::Rect(), Object() {
	size = _size;
	fillColor = _color;
}

//--------------------------------------------------------------------------------

Collision::CollisionResult RigidRect::isColliding( shared_ptr< Object > target ) {
	Collision::CollisionResult out;

	if( target == shared_from_this() )
		return out;

	{
		Collision::Rect* ptr = dynamic_cast< Rect* >( target.get() );
		if( ptr != nullptr )
			out = Collision::collision( ptr, this );
	}
	{
		Collision::Ray* ptr = dynamic_cast< Collision::Ray* >( target.get() );
		if( ptr != nullptr )
			out = Collision::collision( *ptr, *this );
	}

	return out;
}

//================================================================================

} // Game

//================================================================================