//================================================================================

#include "Debug.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "Player.h"

//================================================================================

struct Timer {
	string name;
	sf::Clock clock;
	array< sf::Time, 100u > times;
	size_t nextPosition{ 0u };
	bool init = false;
};

//--------------------------------------------------------------------------------

struct DebugMessage {
	sf::Text text;
	DebugType type;
	float alpha;
};

//--------------------------------------------------------------------------------

vector< Timer > timers;
vector< DebugMessage > messages;

//--------------------------------------------------------------------------------

sf::Font font;

//================================================================================

namespace Debug {

//--------------------------------------------------------------------------------

void startTimer( string name ) {
	const auto it = std::find_if( timers.begin(), timers.end(),
									[name]( const Timer& timer ) {
										return timer.name == name;
									} );

	if( it != timers.end() ) {
		it->clock.restart();
		return;
	}

	Timer timer;
	timer.name = name;
	timer.clock.restart();
	timers.push_back( timer );

	std::sort( timers.begin(), timers.end(),
			   []( const Timer& a, const Timer& b ) {
				   return a.name < b.name;
			   } );
}

//--------------------------------------------------------------------------------

void stopTimer( string name ) {
	const auto it = std::find_if( timers.begin(), timers.end(),
								  [name]( const Timer& timer ) {
									  return timer.name == name;
								  } );

	if( it == timers.end() )
		return;

	if( !it->init ) {
		it->init = true;
		std::fill( it->times.begin(), it->times.end(), it->clock.restart() );
		return;
	}

	sf::Time& time = it->times.at( it->nextPosition++ );

	if( it->nextPosition == it->times.size() )
		it->nextPosition = 0u;
	time = it->clock.restart();

	const float t = time.asSeconds() * 1000.0f;
	const float average = getAverageTime( name ) * 1000.0f;
	const float threshold = average * 15.0f;
	if( t > threshold && average > 1.0f ) {
		char buffer[ 128 ];
		sprintf_s( buffer, "%s spiked at %.3fms(%.3fms average)", name.c_str(), t, average );
		addMessage( buffer, DebugType::Performance );
	}
}

//--------------------------------------------------------------------------------

float getAverageTime( string name ) {
	const auto it = std::find_if( timers.begin(), timers.end(),
								  [name]( const Timer& timer ) {
									  return timer.name == name;
								  } );

	if( it == timers.end() )
		return 0.0f;
	if( !it->init )
		return 0.0f;

	const std::array< sf::Time, 100 >& vt = it->times;

	float total = 0.0f;
	for( const sf::Time& time : vt )
		total += time.asSeconds();

	return total / vt.size();
}

//--------------------------------------------------------------------------------

void addMessage( string message, DebugType type  ) {
	DebugMessage out;
	out.text.setFont( font );
	out.text.setCharacterSize( 40u );
	out.text.setScale( sf::Vector2f( 0.1f, 0.1f ) );
	out.text.setOutlineColor( sf::Color::Black );
	out.text.setOutlineThickness( 3.0f );
	out.text.setString( message );
	out.alpha = 1.0;
	out.type = type;
	
	sf::Color color;
	if( out.type == DebugType::None )
		color = sf::Color::White;
	else if( out.type == DebugType::Info )
		color = sf::Color( 100, 200, 200, 255 );
	else if( out.type == DebugType::Warning )
		color = sf::Color( 200, 150, 50, 255 );
	else if( out.type == DebugType::Error )
		color = sf::Color( 200, 50, 50, 255 );
	else if( out.type == DebugType::Input )
		color = sf::Color( 150, 100, 200, 255 );
	else if( out.type == DebugType::Performance )
		color = sf::Color( 225, 180, 50, 255 );

	out.text.setFillColor( color );

	messages.push_back( out );
}

//================================================================================

void PerformanceWindow::onStart() {
	font.loadFromFile( Folders::Fonts + "MonospaceTypewriter.ttf" );
	m_text = sf::Text();
	m_text.setCharacterSize( 40u );
	m_text.setScale( sf::Vector2f( 0.1f, 0.1f ) );
	m_text.setFont( font );
	m_text.setOutlineColor( sf::Color::Black );
	m_text.setOutlineThickness( 3.0f );
}

//--------------------------------------------------------------------------------

void PerformanceWindow::onUpdate( sf::Time deltaTime ) {
	// Update FPS
	if( m_first ) {
		std::fill( m_deltaTimes.begin(), m_deltaTimes.end(), deltaTime );
		m_iterator = m_deltaTimes.begin();
		m_first = false;
	}
	else {
		sf::Time& time = *m_iterator;
		time = deltaTime;

		if( ++m_iterator == m_deltaTimes.end() )
			m_iterator = m_deltaTimes.begin();
	}

	// Get FPS average
	float total = 0.0f;
	for( const sf::Time& time : m_deltaTimes )
		total += time.asSeconds();

	const float average = total / m_deltaTimes.size();
	const int fps = int( 1.0f / average );

	// Get screen position
	sf::RenderWindow* window = System::getWindow();
	sf::Vector2f pos = window->mapPixelToCoords( sf::Vector2i() );
	pos.x += 3.0f;
	m_text.setPosition( pos );

	// Construct strings
	// FPS
	string s = std::to_string( fps ) + "fps\n";
	for( auto& timer : timers ) {
		char buffer[ 40 ];
		s += timer.name + ": ";
		sprintf_s( buffer, "%.3f", getAverageTime( timer.name ) * 1000.0f );
		s += buffer;
		s += "ms\n";
	}
	m_text.setString( sf::String( s ) );

	// Messages
	float y = 0.0;
	for( int i = int( messages.size() ) - 1; i > 0; --i ) {
		DebugMessage& message = messages.at( i );
		// Fade out message
		message.alpha -= 0.1f * deltaTime.asSeconds();
		// Delete invisible messages
		if( message.alpha < 0.0f ) {
			messages.erase( messages.begin() + i );
			continue;
		}
		// Set alpha
		{
			sf::Color color = message.text.getFillColor();
			sf::Color outline = message.text.getOutlineColor();
			color.a = outline.a = sf::Uint8( message.alpha * 255.0f );
			message.text.setFillColor( color );
			message.text.setOutlineColor( outline );
		}
		// Set position
		Math::Vec2 position = Math::Vec2( window->mapPixelToCoords( sf::Vector2i( System::getSystemInfo().width, 0 ) ) );
		position.y += y;
		const float width = message.text.getGlobalBounds().width;
		position.x -= width + 3.0f;
		message.text.setPosition( position.sf() );

		// Move next line down
		y += 5.0f;
	}
}

//--------------------------------------------------------------------------------

void PerformanceWindow::onRender() {
	if( !m_visibility )
		return;

	sf::RenderWindow* window = System::getWindow();

	// Times
	window->draw( m_text );

	// Messages
	for( const DebugMessage& message : messages ) {
		window->draw( message.text );
	}
}

//--------------------------------------------------------------------------------

void PerformanceWindow::onEvent( sf::Event e ) {
	if( e.type == sf::Event::JoystickConnected ) {
		const unsigned int id = e.joystickConnect.joystickId;
		const string name = sf::Joystick::getIdentification( id ).name;

		char buffer[ 128 ];
		sprintf_s( buffer, "Joystick %i \"%s\" connected", id, name.c_str() );
		addMessage( buffer, DebugType::Input );
		return;
	}

	if( e.type == sf::Event::JoystickDisconnected ) {
		const unsigned int id = e.joystickConnect.joystickId;
		const string name = sf::Joystick::getIdentification( id ).name;

		char buffer[ 128 ];
		sprintf_s( buffer, "Joystick %i \"%s\" disconnected", id, name.c_str() );
		addMessage( buffer, DebugType::Input );
		return;
	}
}

//================================================================================

void JoystickWindow::onStart() {
	font.loadFromFile( Folders::Fonts + "MonospaceTypewriter.ttf" );
	m_text = sf::Text();
	m_text.setCharacterSize( 40u );
	m_text.setScale( sf::Vector2f( 0.1f, 0.1f ) );
	m_text.setFont( font );
	m_text.setOutlineColor( sf::Color::Black );
	m_text.setOutlineThickness( 3.0f );
}

//--------------------------------------------------------------------------------

void JoystickWindow::onUpdate( sf::Time deltaTime ) {

	float x = sf::Joystick::getAxisPosition( 0, sf::Joystick::X );
	float y = sf::Joystick::getAxisPosition( 0, sf::Joystick::Y );
	float z = sf::Joystick::getAxisPosition( 0, sf::Joystick::Z );
	float r = sf::Joystick::getAxisPosition( 0, sf::Joystick::R );
	float u = sf::Joystick::getAxisPosition( 0, sf::Joystick::U ) + 100.0f;
	float v = sf::Joystick::getAxisPosition( 0, sf::Joystick::V ) + 100.0f;
	float povx = sf::Joystick::getAxisPosition( 0, sf::Joystick::PovX );
	float povy = sf::Joystick::getAxisPosition( 0, sf::Joystick::PovY );

	if( abs( x ) < 10.0f )
		x = 0.0f;
	if( abs( y ) < 10.0f )
		y = 0.0f;
	if( abs( z ) < 10.0f )
		z = 0.0f;
	if( abs( r ) < 10.0f )
		r = 0.0f;
	if( abs( u ) < 10.0f )
		u = 0.0f;
	if( abs( v ) < 10.0f )
		v = 0.0f;
	if( abs( povx ) < 10.0f )
		povx = 0.0f;
	if( abs( povy ) < 10.0f )
		povy = 0.0f;

	char format[] =
		"Controller 0: \"%s\"\n"
		"Product ID: %i\n"
		"Vendor ID: %i\n\n"
		"  Axis:\n"
		"    X: %.2f\n"
		"    Y: %.2f\n"
		"    Z: %.2f\n"
		"    R: %.2f\n"
		"    U: %.2f\n"
		"    V: %.2f\n"
		" PovX: %.2f\n"
		" PovY: %.2f\n\n"
		"  Button:\n";

	char buffer[ 1024 ];
	sprintf_s( buffer, format,
			   sf::Joystick::getIdentification( 0 ).name.toAnsiString().c_str(),
			   sf::Joystick::getIdentification( 0 ).productId,
			   sf::Joystick::getIdentification( 0 ).vendorId,
			   x, y, z, r, u, v, povx, povy );

	sf::String text = sf::String( buffer );
	for( int button : buttons ) {
		text += std::to_string( button );
		text += " ";
	}

	m_text.setString( text );

	sf::RenderWindow* window = System::getWindow();
	sf::Vector2f pos = window->mapPixelToCoords( sf::Vector2i() );
	pos.x += 3.0f;
	m_text.setPosition( pos );
}

//--------------------------------------------------------------------------------

void JoystickWindow::onRender() {
	if( !m_visibility )
		return;

	sf::RenderWindow* window = System::getWindow();
	window->draw( m_text );
}

//--------------------------------------------------------------------------------

void JoystickWindow::onEvent( sf::Event e ) {
	if( e.type == sf::Event::JoystickButtonPressed )
		buttons.insert( e.joystickButton.button );
	else if( e.type == sf::Event::JoystickButtonReleased )
		buttons.erase( e.joystickButton.button );
}

//================================================================================

void PhysicsWindow::onStart() {
	font.loadFromFile( Folders::Fonts + "MonospaceTypewriter.ttf" );
	m_text = sf::Text();
	m_text.setCharacterSize( 20u );
	m_text.setScale( sf::Vector2f( 0.17f, 0.17f ) );
	m_text.setFont( font );
	m_text.setLineSpacing( 0.9f );
	m_text.setOutlineColor( sf::Color::Black );
	m_text.setOutlineThickness( 3.0f );
}

//--------------------------------------------------------------------------------

void PhysicsWindow::onUpdate( sf::Time deltaTime ) {
	const vector< shared_ptr< Game::Player > > players = Object::getObjects< Game::Player >();
	if( players.empty() )
		return;
	const shared_ptr< Game::Player > player = Object::getObjects< Game::Player >().at( 0 );

	const Math::Vec2 position = player->getPosition();
	const Math::Vec2 velocity = player->getVelocity();
	const auto& spriteData = player->spriteData;
	const auto& movementData = player->movementData;
	const auto& gravityData = player->gravityData;
	const auto& frictionData = player->frictionData;
	const auto& jumpData = player->jumpData;
	const auto& doubleJumpData = player->doubleJumpData;
	const auto& dashData = player->dashData;
	const auto& wallClingData = player->wallClingData;
	const auto& wallJumpData = player->wallJumpData;

	char format[] = "Player\n"
		"  Physics Data\n"
		"    Position: { %.2f, %.2f }\n"
		"    Velocity: { %.2f, %.2f }\n"
		"  Sprite Data\n"
		"    Size: { %.0f, %.0f }\n"
		"    Dash Shadows: %i\n"
		"  Movement Data\n"
		"    Enabled: %s\n"
		"    Acceleration: %.0f\n"
		"    Max Speed: %.0f\n"
		"    Air Multiplier: %.2f\n"
		"  Gravity Data\n"
		"    Enabled: %s\n"
		"    Power: %.0f\n"
		"    Max: %.0f\n"
		"  Friction Data\n"
		"    Enabled: %s\n"
		"    Power: %.0f\n"
		"    Min: %.0f\n"
		"    Max: %.0f\n"
		"    Air Multiplier: %.2f\n"
		"  Jump Data\n"
		"    Enabled: %s\n"
		"    Can Jump: %s\n"
		"    Can Jump Down: %s\n"
		"    Is Jumping Down: %s\n"
		"  Double Jump Data\n"
		"    Enabled: %s\n"
		"    Can Double Jump: %s\n"
		"    Power: %.0f\n"
		"  Dash Data\n"
		"    Enabled: %s\n"
		"    Can Dash: %s\n"
		"    Is Dashing: %s\n"
		"    Power: %.0f\n"
		"    Release: { %.0f, %.0f }\n"
		"    Cooldown: %ims\n"
		"    Duration: %ims\n"
		"    Animation Step: %ims\n"
		"  Wall Cling Data\n"
		"    Enabled: %s\n"
		"    Is Clinging: %s\n"
		"    Leniency: %.2f\n"
		"    Multiplier: %.2f\n"
		"    Min: %.0f\n"
		"    Max: %.0f\n"
		"  Wall Jump Data\n"
		"    Enabled: %s\n"
		"    Normal: %.2f\n"
		"    Power: %.0f\n"
		"    Direction: { %.2f, %.2f }\n"
		"    Duration: %ims";

	char buffer[ 2048 ];
	sprintf_s( buffer, format,
			   position.x, position.y,
			   velocity.x, velocity.y,
			   spriteData.size.x, spriteData.size.y, spriteData.dashShadows.size(),
			   movementData.enabled ? "True" : "False", movementData.acceleration, movementData.maxSpeed, movementData.airMultiplier,
			   gravityData.enabled ? "True" : "False", gravityData.power, gravityData.max,
			   frictionData.enabled ? "True" : "False", frictionData.power, frictionData.min, frictionData.max, frictionData.airMultiplier,
			   jumpData.enabled ? "True" : "False", jumpData.canJump ? "True" : "False", jumpData.canJumpDown ? "True" : "False", jumpData.isJumpingDown ? "True" : "False",
			   doubleJumpData.enabled ? "True" : "False", doubleJumpData.canDoubleJump ? "True" : "False", doubleJumpData.power,
			   dashData.enabled ? "True" : "False", dashData.canDash ? "True" : "False", dashData.isDashing ? "True" : "False", dashData.power, dashData.release.x, dashData.release.y, dashData.cooldown.count(), dashData.duration.count(), dashData.animationStep.count(),
			   wallClingData.enabled ? "True" : "False", wallClingData.isClinging ? "True" : "False", wallClingData.leniency, wallClingData.multiplier, wallClingData.min, wallClingData.max,
			   wallJumpData.enabled ? "True" : "False", wallJumpData.normal, wallJumpData.power, wallJumpData.direction.x, wallJumpData.direction.y, wallJumpData.duration.count() );

	m_text.setString( sf::String( buffer ) );

	sf::RenderWindow* window = System::getWindow();
	sf::Vector2f pos = window->mapPixelToCoords( sf::Vector2i() );
	pos.x += 3.0f;
	m_text.setPosition( pos );
}

//--------------------------------------------------------------------------------

void PhysicsWindow::onRender() {
	if( !m_visibility )
		return;

	sf::RenderWindow* window = System::getWindow();
	window->draw( m_text );
}

//--------------------------------------------------------------------------------

}

//================================================================================