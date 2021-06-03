//==================================================================================================

#include "PlayerController.h"

//==================================================================================================

namespace Input {

//--------------------------------------------------------------------------------------------------

void PlayerController::onEvent( sf::Event e ) {
	if( e.type == sf::Event::JoystickConnected ) {
		shared_ptr< Controller > controller = makeObject< Controller >( m_parent );
		controller->loadController( e.joystickConnect );
		m_controllers.push_back( controller );
	}
	if( e.type == sf::Event::JoystickDisconnected ) {
		const auto it = getController( e.joystickConnect.joystickId );
		if( it != m_controllers.end() )
			m_controllers.erase( it );
		return;
	}

	if( e.type == sf::Event::JoystickButtonPressed ) {
		if( e.joystickButton.joystickId != m_activeController ) {
			m_activeController = e.joystickButton.joystickId;
			return;
		}

		// Notify on button press
		const auto it = getController( e.joystickButton.joystickId );
		if( it == m_controllers.end() )
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
		if( e.joystickButton.joystickId != m_activeController )
			return;

		// Notify on button release
		const auto it = getController( e.joystickButton.joystickId );
		if( it == m_controllers.end() )
			return;
		KeyBind* bind = getKeyBind( ( *it )->getButton( e.joystickButton.button ) );
		if( bind == nullptr )
			return;

		bind->callback( false );
		return;
	}

	if( e.type == sf::Event::JoystickMoved ) {
		if( e.joystickMove.joystickId != m_activeController ) {
			m_activeController = e.joystickMove.joystickId;
			return;
		}

		// Notify on axis moved
		const auto it = getController( e.joystickMove.joystickId );
		if( it == m_controllers.end() )
			return;
		AxisBind* bind = getAxisBind( ( *it )->getAxis( e.joystickMove.axis ) );
		if( bind == nullptr )
			return;
		if( bind->callback == nullptr )
			return;

		bind->callback( e.joystickMove.position );
		return;
	}

	if( e.type == sf::Event::KeyPressed ) {
		if( e.key.code == sf::Keyboard::Unbound )
			return;

		if( m_activeController >= 0 ) {
			m_activeController = -1;
			return;
		}

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

		if( m_activeController >= 0 )
			return;

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
		if( m_activeController >= 0 ) {
			m_activeController = -1;
			return;
		}

		// Notify on mouse press

		return;
	}

	if( e.type == sf::Event::MouseMoved ) {
		if( m_activeController >= 0 ) {
			m_activeController = -1;
			return;
		}

		// Notify on mouse move

		return;
	}
}

//--------------------------------------------------------------------------------------------------

void PlayerController::bindButton( string name, ControllerButton button, sf::Keyboard::Key key, function< void( bool ) > callback ) {
	{
		KeyBind* bind = getKeyBind( name );
		if( bind != nullptr ) {
			bind->button = button;
			bind->key = key;
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
			if( bind->negativeKey == key )
				bind->negativeKey = sf::Keyboard::Unbound;
			else if( bind->positiveKey == key )
				bind->positiveKey = sf::Keyboard::Unbound;
	}

	KeyBind bind;
	bind.name = name;
	bind.button = button;
	bind.key = key;
	bind.callback = callback;

	m_keyBindings.push_back( bind );
}

//--------------------------------------------------------------------------------------------------

void PlayerController::bindAxis( string name, ControllerAxis axis, sf::Keyboard::Key negative, sf::Keyboard::Key positive, function< void( float ) > callback ) {
	{
		AxisBind* bind = getAxisBind( name );
		if( bind != nullptr ) {
			bind->axis = axis;
			bind->negativeKey = negative;
			bind->positiveKey = positive;
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
		AxisBind* bind = getAxisBind( negative );
		if( bind != nullptr )
			if( bind->negativeKey == negative )
				bind->negativeKey = sf::Keyboard::Unbound;
			else if( bind->positiveKey == negative )
				bind->positiveKey = sf::Keyboard::Unbound;
	}
	{
		AxisBind* bind = getAxisBind( positive );
		if( bind != nullptr )
			if( bind->negativeKey == positive )
				bind->negativeKey = sf::Keyboard::Unbound;
			else if( bind->positiveKey == positive )
				bind->positiveKey = sf::Keyboard::Unbound;
	}
	{
		KeyBind* bind = getKeyBind( negative );
		if( bind != nullptr )
			bind->key = sf::Keyboard::Unbound;
	}
	{
		KeyBind* bind = getKeyBind( positive );
		if( bind != nullptr )
			bind->key = sf::Keyboard::Unbound;
	}

	AxisBind bind;
	bind.name = name;
	bind.axis = axis;
	bind.positiveKey = positive;
	bind.negativeKey = negative;
	bind.callback = callback;

	m_axisBindings.push_back( bind );
}

//--------------------------------------------------------------------------------------------------

bool PlayerController::getButtonState( string name ) {
	const KeyBind* bind = getKeyBind( name );
	if( bind == nullptr )
		return false;

	const auto it = getController( uint32_t( m_activeController ) );
	if( it == m_controllers.end() )
		return sf::Keyboard::isKeyPressed( bind->key );

	return ( *it )->getButtonState( bind->button );
}

//--------------------------------------------------------------------------------------------------

float PlayerController::getAxisState( string name ) {
	const AxisBind* bind = getAxisBind( name );
	if( bind == nullptr )
		return 0.0f;

	const auto it = getController( uint32_t( m_activeController ) );
	if( it == m_controllers.end() ) {
		float out = 0.0f;
		if( sf::Keyboard::isKeyPressed( bind->positiveKey ) )
			out += 1.0f;
		if( sf::Keyboard::isKeyPressed( bind->negativeKey ) )
			out -= 1.0f;

		return out;
	}

	return ( *it )->getAxisState( bind->axis );
}

//--------------------------------------------------------------------------------------------------

vector< shared_ptr< Controller > >::iterator PlayerController::getController( uint32_t joystickid ) {
	return std::find_if( m_controllers.begin(), m_controllers.end(),
						 [joystickid]( const shared_ptr< Controller >& controller ) {
							 return controller->getID() == joystickid;
						 } );
}

//--------------------------------------------------------------------------------------------------

PlayerController::KeyBind* PlayerController::getKeyBind( string name ) {
	const auto it = std::find_if( m_keyBindings.begin(), m_keyBindings.end(),
								  [name]( const KeyBind& bind ) {
									  return bind.name == name;
								  } );
	if( it != m_keyBindings.end() )
		return &*it;

	return nullptr;
}

//--------------------------------------------------------------------------------------------------

PlayerController::KeyBind* PlayerController::getKeyBind( ControllerButton button ) {
	const auto it = std::find_if( m_keyBindings.begin(), m_keyBindings.end(),
								  [button]( const KeyBind& bind ) {
									  return bind.button == button;
								  } );
	if( it != m_keyBindings.end() )
		return &*it;

	return nullptr;
}

//--------------------------------------------------------------------------------------------------

PlayerController::KeyBind* PlayerController::getKeyBind( sf::Keyboard::Key key ) {
	const auto it = std::find_if( m_keyBindings.begin(), m_keyBindings.end(),
								  [key]( const KeyBind& bind ) {
									  return bind.key == key;
								  } );
	if( it != m_keyBindings.end() )
		return &*it;

	return nullptr;
}

//--------------------------------------------------------------------------------------------------

PlayerController::AxisBind* PlayerController::getAxisBind( string name ) {
	const auto it = std::find_if( m_axisBindings.begin(), m_axisBindings.end(),
								  [name]( const AxisBind& bind ) {
									  return bind.name == name;
								  } );
	if( it != m_axisBindings.end() )
		return &*it;

	return nullptr;
}

//--------------------------------------------------------------------------------------------------

PlayerController::AxisBind* PlayerController::getAxisBind( ControllerAxis axis ) {
	const auto it = std::find_if( m_axisBindings.begin(), m_axisBindings.end(),
								  [axis]( const AxisBind& bind ) {
									  return bind.axis == axis;
								  } );
	if( it != m_axisBindings.end() )
		return &*it;

	return nullptr;
}

//--------------------------------------------------------------------------------------------------

PlayerController::AxisBind* PlayerController::getAxisBind( sf::Keyboard::Key key ) {
	const auto it = std::find_if( m_axisBindings.begin(), m_axisBindings.end(),
								  [key]( const AxisBind& bind ) {
									  return bind.positiveKey == key || bind.negativeKey == key;
								  } );
	if( it != m_axisBindings.end() )
		return &*it;

	return nullptr;
}

//--------------------------------------------------------------------------------------------------

}

//==================================================================================================