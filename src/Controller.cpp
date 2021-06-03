//================================================================================

#include "Controller.h"

//================================================================================

unordered_map< string, unordered_map< ControllerButton, uint32_t > > buttonMap = {
	{ "Wireless Controller", {
		{ ControllerButton::Bottom, 1u },
		{ ControllerButton::Top, 3u },
		{ ControllerButton::Left, 0u },
		{ ControllerButton::Right, 2u },
		{ ControllerButton::L, 4u },
		{ ControllerButton::R, 5u },
		{ ControllerButton::LTrigger, 6u },
		{ ControllerButton::RTrigger, 7u },
		{ ControllerButton::LeftStick, 10u },
		{ ControllerButton::RightStick, 11u },
		{ ControllerButton::Select, 8u },
		{ ControllerButton::Start, 9u },
		{ ControllerButton::Special, 13u },
		{ ControllerButton::Logo, 12u },
	} } };

unordered_map< string, unordered_map< ControllerAxis, sf::Joystick::Axis > > axisMap = {
	{ "Wireless Controller", {
		{ ControllerAxis::LeftStickX, sf::Joystick::X },
		{ ControllerAxis::LeftStickY, sf::Joystick::Y },
		{ ControllerAxis::RightStickX, sf::Joystick::Z },
		{ ControllerAxis::RightStickY, sf::Joystick::R },
		{ ControllerAxis::LTrigger, sf::Joystick::U },
		{ ControllerAxis::RTrigger, sf::Joystick::V },
		{ ControllerAxis::DPadX, sf::Joystick::PovX },
		{ ControllerAxis::DPadY, sf::Joystick::PovY },
} } };

//================================================================================

namespace Input {

//--------------------------------------------------------------------------------

void Controller::onEvent( sf::Event e ) {
	if( !( e.type == sf::Event::JoystickButtonPressed || e.type == sf::Event::JoystickButtonReleased ) )
		return;
		
	if( e.joystickButton.joystickId != m_id )
		return;

	ControllerButton key = getButton( e.joystickButton.button );

	if( key == ControllerButton::None )
		return;

	if( e.type == sf::Event::JoystickButtonPressed ) {
		m_buttonStates[ key ] = true;
		return;
	}

	if( e.type == sf::Event::JoystickButtonReleased ) {
		m_buttonStates[ key ] = false;
		return;
	}

	if( e.type == sf::Event::JoystickDisconnected )
		if( e.joystickConnect.joystickId == m_id )
			destroy();
}

//--------------------------------------------------------------------------------

void Controller::loadController( sf::Event::JoystickConnectEvent joystick ) {
	const sf::Joystick::Identification id = sf::Joystick::getIdentification( joystick.joystickId );
	m_name = id.name;
	m_vendorid = id.vendorId;
	m_productid = id.productId;

	m_buttonStates[ ControllerButton::Bottom ] = false;
	m_buttonStates[ ControllerButton::Left ] = false;
	m_buttonStates[ ControllerButton::Top ] = false;
	m_buttonStates[ ControllerButton::Right ] = false;
	m_buttonStates[ ControllerButton::R ] = false;
	m_buttonStates[ ControllerButton::L ] = false;
	m_buttonStates[ ControllerButton::RTrigger ] = false;
	m_buttonStates[ ControllerButton::LTrigger ] = false;
	m_buttonStates[ ControllerButton::LeftStick ] = false;
	m_buttonStates[ ControllerButton::RightStick ] = false;
	m_buttonStates[ ControllerButton::Select ] = false;
	m_buttonStates[ ControllerButton::Start ] = false;
	m_buttonStates[ ControllerButton::Special ] = false;
	m_buttonStates[ ControllerButton::Logo ] = false;
}

//--------------------------------------------------------------------------------

bool Controller::getButtonState( ControllerButton button ) {
	if( this == nullptr )
		return false;

	return m_buttonStates[ button ];
}

//--------------------------------------------------------------------------------

float Controller::getAxisState( ControllerAxis axis ) {
	if( this == nullptr )
		return 0.0f;

	return sf::Joystick::getAxisPosition( m_id, axisMap.at( sf::Joystick::getIdentification( m_id ).name ).at( axis ) );
}

//--------------------------------------------------------------------------------

ControllerButton Controller::getButton( uint32_t buttonid ) {
	if( this == nullptr )
		return ControllerButton::None;

	const unordered_map< ControllerButton, uint32_t >& bmap = buttonMap.at( sf::Joystick::getIdentification( m_id ).name );
	for( pair< ControllerButton, uint32_t > pair : bmap )
		if( pair.second == buttonid )
			return pair.first;

	return ControllerButton::None;
}

//--------------------------------------------------------------------------------

ControllerAxis Controller::getAxis( sf::Joystick::Axis axis ) {
	if( this == nullptr )
		return ControllerAxis::None;

	const unordered_map< ControllerAxis, sf::Joystick::Axis >& bmap = axisMap.at( sf::Joystick::getIdentification( m_id ).name );
	for( pair< ControllerAxis, sf::Joystick::Axis > pair : bmap )
		if( pair.second == axis )
			return pair.first;

	return ControllerAxis::None;
}

//--------------------------------------------------------------------------------

}

//================================================================================