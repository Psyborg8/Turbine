#include "Object.h"

//================================================================================

// CLASS - Object

//================================================================================

Object::Object() : m_parent( nullptr ), m_name( "" )
{
	//
}

//--------------------------------------------------------------------------------

Object::Object( string name ) : m_parent( nullptr ), m_name( name )
{
	//
}

//--------------------------------------------------------------------------------

void Object::createObservers()
{
	// Observers
	using namespace std::placeholders;
	m_mouseMoveObserver = Observer::addObserver( ObserverType::MouseMove, AxisCallback( std::bind( &Object::onMouseMove, this, _1, _2 ) ) );
	m_mousePressObserver = Observer::addObserver( ObserverType::MousePress, KeyCallback( std::bind( &Object::onMousePress, this, _1 ) ) );
	m_mouseReleaseObserver = Observer::addObserver( ObserverType::MouseRelease, KeyCallback( std::bind( &Object::onMouseRelease, this, _1 ) ) );

	m_keyboardPressObserver = Observer::addObserver( ObserverType::KeyPress, KeyCallback( std::bind( &Object::onKeyboardPress, this, _1 ) ) );
	m_keyboardReleaseObserver = Observer::addObserver( ObserverType::KeyRelease, KeyCallback( std::bind( &Object::onKeyboardRelease, this, _1 ) ) );

	m_windowResizeObserver = Observer::addObserver( ObserverType::WindowResize, Vec2Callback( std::bind( &Object::onWindowResize, this, _1, _2 ) ) );
}

//--------------------------------------------------------------------------------

void Object::onDestroy()
{
	Observer::removeObserver( m_mouseMoveObserver );
	Observer::removeObserver( m_mousePressObserver );
	Observer::removeObserver( m_mouseReleaseObserver );
	Observer::removeObserver( m_keyboardPressObserver );
	Observer::removeObserver( m_keyboardReleaseObserver );
	Observer::removeObserver( m_windowResizeObserver );
}

//================================================================================

// STATIC

//================================================================================

void Object::checkCollisions( vector< shared_ptr< Object > > objects )
{
	for( vector< shared_ptr< Object > >::iterator it = objects.begin(); it != objects.end(); ++it )
	{
		const shared_ptr< Object > object = *it;
		if( object->getCollisionType() == CollisionType::None )
			continue;

		// Avoid checking the same collision twice
		vector< shared_ptr< Object > >::iterator jt = it;
		jt++;

		while( jt != objects.end() )
		{
			shared_ptr< Object > target = *jt;
			jt++;

			if( target->getCollisionType() == CollisionType::None )
				continue;

			Vec2 normal;
			const bool isColliding = object->isColliding( target, normal );
			if( isColliding )
			{
				object->onCollision( target, normal );
				target->onCollision( object, normal * -1.0 ); // Also notify the target with the inverse normal
			}
		}
	}
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Object > > Object::s_objects;

//================================================================================