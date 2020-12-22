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
}

//--------------------------------------------------------------------------------

void Object::onDestroy()
{
	Observer::removeObserver( m_mouseMoveObserver );
	Observer::removeObserver( m_mousePressObserver );
	Observer::removeObserver( m_mouseReleaseObserver );
	Observer::removeObserver( m_keyboardPressObserver );
	Observer::removeObserver( m_keyboardReleaseObserver );
}

//================================================================================

// STATIC

//================================================================================

vector< shared_ptr< Object > > Object::s_objects;

//================================================================================