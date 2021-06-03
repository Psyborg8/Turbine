//==================================================================================================

#pragma once

//==================================================================================================

#include "Controller.h"

//==================================================================================================

namespace Input {

//--------------------------------------------------------------------------------------------------

class PlayerController : public Object {
protected:
	struct KeyBind {
		string name;
		ControllerButton button;
		sf::Keyboard::Key key;
		function< void( bool ) > callback = nullptr;
	};

	struct AxisBind {
		string name;
		ControllerAxis axis;
		sf::Keyboard::Key positiveKey;
		sf::Keyboard::Key negativeKey;
		function< void( float ) > callback = nullptr;
	};

public:
	PlayerController() = default;

public:
	void onEvent( sf::Event e );

public:
	void bindButton( string name, ControllerButton button, sf::Keyboard::Key key, function< void( bool ) > callback = nullptr );
	void bindAxis( string name, ControllerAxis axis, sf::Keyboard::Key negative, sf::Keyboard::Key positive, function< void( float ) > callback = nullptr );

	bool getButtonState( string name );
	float getAxisState( string name );

protected:
	vector< shared_ptr< Controller > >::iterator getController( uint32_t joystickid );

	KeyBind* getKeyBind( string name );
	KeyBind* getKeyBind( ControllerButton button );
	KeyBind* getKeyBind( sf::Keyboard::Key key );
	AxisBind* getAxisBind( string name );
	AxisBind* getAxisBind( ControllerAxis axis );
	AxisBind* getAxisBind( sf::Keyboard::Key key );

protected:
	int m_activeController{ -1 };
	vector< shared_ptr< Controller > > m_controllers;
	vector< KeyBind > m_keyBindings;
	vector< AxisBind > m_axisBindings;
};

//--------------------------------------------------------------------------------------------------

}

//==================================================================================================
