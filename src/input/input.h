//==================================================================================================

#pragma once

//==================================================================================================

#include "global.h"

#include "input-controller.h"

//==================================================================================================

namespace Input {

void start( Object* parent );

void bindButton( string name, sf::Mouse::Button key, function< void( bool ) > callback = nullptr );
void bindButton( string name, ControllerButton button, sf::Keyboard::Key key, function< void( bool ) > callback = nullptr );
void bindAxis( string name, ControllerAxis axis, KeyPair keys, function< void( float ) > callback = nullptr );

bool getButtonState( string name );
float getAxisState( string name );

}

//==================================================================================================
