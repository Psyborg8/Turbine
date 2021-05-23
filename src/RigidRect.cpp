//================================================================================

#include "RigidRect.h"

//--------------------------------------------------------------------------------

#include "Collision.h"

//================================================================================

RigidRect::RigidRect() : Gfx::Rect(), Object()
{
	//
}

//--------------------------------------------------------------------------------

RigidRect::RigidRect( Vec2 _position, Vec2 _size, Color _color = Colors::WHITE ) : Gfx::Rect(), Object()
{
	position = _position;
	size = _size;
	fillColor = _color;
}

//--------------------------------------------------------------------------------

Collision::CollisionResult RigidRect::isColliding( shared_ptr< Object > target )
{
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