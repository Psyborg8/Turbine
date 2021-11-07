//==================================================================================================

#include "input.h"

//--------------------------------------------------------------------------------------------------

#include "debug.h"

//==================================================================================================

namespace Input {

//--------------------------------------------------------------------------------------------------

struct MouseBind {
	string name;
	sf::Mouse::Button key;
	function< void( bool ) > callback = nullptr;
};

struct KeyBind {
	string name;
	ControllerButton button;
	sf::Keyboard::Key key;
	function< void( bool ) > callback = nullptr;
};

struct AxisBind {
	string name;
	ControllerAxis axis;
	KeyPair keys;
	function< void( float ) > callback = nullptr;
};

//==================================================================================================

int activeController{ -1 };
vector< shared_ptr< Controller > > controllers;

vector< KeyBind > keyBindings;
vector< AxisBind > axisBindings;
vector< MouseBind > mouseBindings;

float innerDeadzone = 10.0f;
float outerDeadzone = 10.0f;

//--------------------------------------------------------------------------------------------------

void processEvent( sf::Event e );

vector< shared_ptr< Controller > >::iterator getController( unsigned int id );

KeyBind* getKeyBind( string name );
KeyBind* getKeyBind( sf::Keyboard::Key key );
KeyBind* getKeyBind( ControllerButton button );
AxisBind* getAxisBind( string name );
AxisBind* getAxisBind( sf::Keyboard::Key key );
AxisBind* getAxisBind( ControllerAxis axis );
MouseBind* getMouseBind( string name );
MouseBind* getMouseBind( sf::Mouse::Button button );

//--------------------------------------------------------------------------------------------------

class InputManager : public Object {
public:
	void onSpawnChildren() override {
		for( uint32_t i = 0u; i < sf::Joystick::Count; ++i ) {
			if( sf::Joystick::isConnected( i ) ) {
				shared_ptr< Controller > controller = makeObject< Controller >( m_parent );
				sf::Event::JoystickConnectEvent e;
				e.joystickId = i;
				controller->loadController( e );
				controllers.push_back( controller );
			}
		}
	}

	void onEvent( sf::Event e ) override {
		processEvent( e );
	}
};

//--------------------------------------------------------------------------------------------------

void start( Object* parent ) {
	Object::makeObject< InputManager >( parent );
}

//--------------------------------------------------------------------------------------------------

void bindButton( string name, sf::Mouse::Button key, function< void( bool ) > callback ) {
	{
		MouseBind* bind = getMouseBind( name );
		if( bind != nullptr ) {
			if( key != sf::Mouse::Button::Unbound )
				bind->key = key;
			if( callback != nullptr )
				bind->callback = callback;
			return;
		}
	}
	{
		MouseBind* bind = getMouseBind( key );
		if( bind != nullptr )
			bind->key = sf::Mouse::Unbound;
	}

	MouseBind bind;
	bind.name = name;
	bind.key = key;
	bind.callback = callback;

	mouseBindings.push_back( bind );
}

//--------------------------------------------------------------------------------------------------

void bindButton( string name, ControllerButton button, sf::Keyboard::Key key, function< void( bool ) > callback ) {
	{
		KeyBind* bind = getKeyBind( name );
		if( bind != nullptr ) {
			if( button != ControllerButton::None )
				bind->button = button;
			if( key != sf::Keyboard::Unbound )
				bind->key = key;
			if( callback != nullptr )
				bind->callback = callback;
			return;
		}
	}
	{
		KeyBind* bind = getKeyBind( button );
		if( bind != nullptr )
			bind->button = ControllerButton::None;
	}
	{
		KeyBind* bind = getKeyBind( key );
		if( bind != nullptr )
			bind->key = sf::Keyboard::Unbound;
	}
	{
		AxisBind* bind = getAxisBind( key );
		if( bind != nullptr )
			if( bind->keys.first == key )
				bind->keys.first = sf::Keyboard::Unbound;
			else if( bind->keys.second == key )
				bind->keys.second = sf::Keyboard::Unbound;
	}

	KeyBind bind;
	bind.name = name;
	bind.button = button;
	bind.key = key;
	bind.callback = callback;

	keyBindings.push_back( bind );
}

//--------------------------------------------------------------------------------------------------

void bindAxis( string name, ControllerAxis axis, KeyPair keys, function< void( float ) > callback ) {
	{
		AxisBind* bind = getAxisBind( name );
		if( bind != nullptr ) {
			if( axis != ControllerAxis::None )
				bind->axis = axis;
			if( keys.first != sf::Keyboard::Unbound && keys.second != sf::Keyboard::Unbound )
				bind->keys = keys;
			if( callback != nullptr )
				bind->callback = callback;
			return;
		}
	}
	{
		AxisBind* bind = getAxisBind( axis );
		if( bind != nullptr )
			bind->axis = ControllerAxis::None;
	}
	{
		AxisBind* bind = getAxisBind( keys.first );
		if( bind != nullptr )
			if( bind->keys.first == keys.first )
				bind->keys.first = sf::Keyboard::Unbound;
			else if( bind->keys.second == keys.first )
				bind->keys.second = sf::Keyboard::Unbound;
	}
	{
		AxisBind* bind = getAxisBind( keys.second );
		if( bind != nullptr )
			if( bind->keys.first == keys.second )
				bind->keys.first = sf::Keyboard::Unbound;
			else if( bind->keys.second == keys.second )
				bind->keys.second = sf::Keyboard::Unbound;
	}
	{
		KeyBind* bind = getKeyBind( keys.first );
		if( bind != nullptr )
			bind->key = sf::Keyboard::Unbound;
	}
	{
		KeyBind* bind = getKeyBind( keys.second );
		if( bind != nullptr )
			bind->key = sf::Keyboard::Unbound;
	}

	AxisBind bind;
	bind.name = name;
	bind.axis = axis;
	bind.keys = keys;
	bind.callback = callback;

	axisBindings.push_back( bind );
}

//--------------------------------------------------------------------------------------------------

bool getButtonState( string name ) {
	const KeyBind* bind = getKeyBind( name );
	if( bind == nullptr )
		return false;

	const auto it = getController( uint32_t( activeController ) );
	if( it == controllers.end() )
		return sf::Keyboard::isKeyPressed( bind->key );

	return ( *it )->getButtonState( bind->button );
}

//--------------------------------------------------------------------------------------------------

float getAxisState( string name ) {
	const AxisBind* bind = getAxisBind( name );
	if( bind == nullptr )
		return 0.0f;

	const auto it = getController( uint32_t( activeController ) );

	float controllerAxis = 0.0f;
	if( it != controllers.end() )
		controllerAxis = ( *it )->getAxisState( bind->axis );

	if( abs( controllerAxis ) <= innerDeadzone )
		controllerAxis = 0.0f;

	if( it == controllers.end() || !controllerAxis ) {
		float out = 0.0f;
		if( sf::Keyboard::isKeyPressed( bind->keys.second ) )
			out += 100.0f;
		if( sf::Keyboard::isKeyPressed( bind->keys.first ) )
			out -= 100.0f;

		return out;
	}
	else {
		float out = std::clamp( abs( controllerAxis ), innerDeadzone, 100.0f - outerDeadzone );
		out -= innerDeadzone;
		out /= 100.0f - innerDeadzone - outerDeadzone;
		out *= 100.0f;

		out *= controllerAxis / abs( controllerAxis );
		return out;
	}

	return 0.0f;
}

//--------------------------------------------------------------------------------------------------

vector< shared_ptr< Controller > >::iterator getController( unsigned int joystickid ) {
	return std::find_if( controllers.begin(), controllers.end(),
						 [joystickid]( const shared_ptr< Controller >& controller ) {
							 return controller->getID() == joystickid;
						 } );
}

//--------------------------------------------------------------------------------------------------

KeyBind* getKeyBind( string name ) {
	const auto it = std::find_if( keyBindings.begin(), keyBindings.end(),
								  [name]( const KeyBind& bind ) {
									  return bind.name == name;
								  } );
	if( it == keyBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

KeyBind* getKeyBind( ControllerButton button ) {
	const auto it = std::find_if( keyBindings.begin(), keyBindings.end(),
								  [button]( const KeyBind& bind ) {
									  return bind.button == button;
								  } );
	if( it == keyBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

KeyBind* getKeyBind( sf::Keyboard::Key key ) {
	const auto it = std::find_if( keyBindings.begin(), keyBindings.end(),
								  [key]( const KeyBind& bind ) {
									  return bind.key == key;
								  } );

	if( it == keyBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

AxisBind* getAxisBind( string name ) {
	const auto it = std::find_if( axisBindings.begin(), axisBindings.end(),
								  [name]( const AxisBind& bind ) {
									  return bind.name == name;
								  } );

	if( it == axisBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

AxisBind* getAxisBind( ControllerAxis axis ) {
	const auto it = std::find_if( axisBindings.begin(), axisBindings.end(),
								  [axis]( const AxisBind& bind ) {
									  return bind.axis == axis;
								  } );

	if( it == axisBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

AxisBind* getAxisBind( sf::Keyboard::Key key ) {
	const auto it = std::find_if( axisBindings.begin(), axisBindings.end(),
								  [key]( const AxisBind& bind ) {
									  return bind.keys.first == key || bind.keys.second == key;
								  } );

	if( it == axisBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

MouseBind* getMouseBind( string name ) {
	const auto it = std::find_if( mouseBindings.begin(), mouseBindings.end(),
								  [name]( const MouseBind& bind ) {
									  return bind.name == name;
								  } );

	if( it == mouseBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

MouseBind* getMouseBind( sf::Mouse::Button button ) {
	const auto it = std::find_if( mouseBindings.begin(), mouseBindings.end(),
								  [button]( const MouseBind& bind ) {
									  return bind.key == button;
								  } );

	if( it == mouseBindings.end() )
		return nullptr;

	return it._Ptr;
}

//--------------------------------------------------------------------------------------------------

void processEvent( sf::Event e ) {
	if( e.type == sf::Event::JoystickConnected ) {
		shared_ptr< Controller > controller = Object::makeObject< Controller >( nullptr );
		controller->loadController( e.joystickConnect );
		controllers.push_back( controller );

		return;
	}

	if( e.type == sf::Event::JoystickDisconnected ) {
		const auto it = getController( e.joystickConnect.joystickId );
		if( it != controllers.end() ) {
			( *it )->destroy();
			controllers.erase( it );
		}

		return;
	}

	if( e.type == sf::Event::JoystickButtonPressed ) {
		if( e.joystickButton.joystickId != activeController )
			activeController = e.joystickButton.joystickId;

		// Notify on button press
		const auto it = getController( e.joystickButton.joystickId );
		if( it == controllers.end() )
			return;
		KeyBind* bind = getKeyBind( ( *it )->getButton( e.joystickButton.button ) );
		if( bind == nullptr )
			return;
		if( bind->callback == nullptr )
			return;

		bind->callback( true );
		return;
	}

	if( e.type == sf::Event::JoystickButtonReleased ) {
		if( e.joystickButton.joystickId != activeController )
			return;

		// Notify on button release
		const auto it = getController( e.joystickButton.joystickId );
		if( it == controllers.end() )
			return;
		KeyBind* bind = getKeyBind( ( *it )->getButton( e.joystickButton.button ) );
		if( bind == nullptr )
			return;

		bind->callback( false );
		return;
	}

	if( e.type == sf::Event::KeyPressed ) {
		if( e.key.code == sf::Keyboard::Unbound )
			return;

		// Look for key and axis bindings for the key
		{
			KeyBind* bind = getKeyBind( e.key.code );
			if( bind == nullptr )
				return;
			if( bind->callback == nullptr )
				return;

			bind->callback( true );
			return;
		}

		return;
	}

	if( e.type == sf::Event::KeyReleased ) {
		if( e.key.code == sf::Keyboard::Unbound )
			return;

		if( activeController >= 0 )
			activeController = -1;

		// Notify on key release
		{
			KeyBind* bind = getKeyBind( e.key.code );
			if( bind == nullptr )
				return;
			if( bind->callback == nullptr )
				return;

			bind->callback( false );
			return;
		}
		return;
	}

	if( e.type == sf::Event::MouseButtonPressed ) {
		if( activeController >= 0 ) {
			activeController = -1;
		}

		// Notify on mouse press

		return;
	}
}

//--------------------------------------------------------------------------------------------------

}

//==================================================================================================