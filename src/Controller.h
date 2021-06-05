//================================================================================

#pragma once

//================================================================================

#include "Object.h"

//================================================================================

namespace Input {

//--------------------------------------------------------------------------------

class Controller : public Object {
public:
	Controller() = default;

public:
	void onEvent( sf::Event e ) override;

public:
	void loadController( sf::Event::JoystickConnectEvent joystick );

	bool getButtonState( ControllerButton button );
	float getAxisState( ControllerAxis axis );

	ControllerButton getButton( uint32_t buttonid );
	ControllerAxis getAxis( sf::Joystick::Axis axis );

	inline uint32_t getID() const { return m_id; }

protected:
	string m_name;
	uint32_t m_id;
	uint32_t m_vendorid;
	uint32_t m_productid;
	unordered_map< ControllerButton, bool > m_buttonStates;
};

//--------------------------------------------------------------------------------

}

//================================================================================