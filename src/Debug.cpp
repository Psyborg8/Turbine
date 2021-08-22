//================================================================================

#include "Debug.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "Player.h"

//================================================================================

namespace Debug {

//--------------------------------------------------------------------------------

struct Timer {
	string name;
	sf::Clock clock;
	array< sf::Time, 100u > times;
	size_t nextPosition{ 0u };
	bool init = false;
};

//--------------------------------------------------------------------------------

struct DebugMessage {
	string text;
	DebugType type;
	float alpha;
};

//--------------------------------------------------------------------------------

vector< Timer > timers;
vector< DebugMessage > messages;

//--------------------------------------------------------------------------------

sf::Font font;

//================================================================================

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
	out.text = message;
	out.alpha = 1.0;
	out.type = type;

	messages.push_back( out );
}

//================================================================================

void MessageWindow::onRender() {
	ImGui::Begin( "Debug Messages", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground );
	ImGui::SetWindowSize( ImVec2( 750.0f, float( System::getSystemInfo().height ) ) );
	ImGui::SetWindowPos( ImVec2( float( System::getSystemInfo().width - 750.0f ), 0.0f ) );

	for( size_t i = messages.size(); i > 0; --i ) {
		ImColor color;
		DebugMessage message = messages.at( i - 1u );

		if( message.type == DebugType::None )
			color = ImColor( 150, 150, 150, int( message.alpha * 255.0f ) );
		else if( message.type == DebugType::Info )
			color = ImColor( 100, 200, 200, int( message.alpha * 255.0f ) );
		else if( message.type == DebugType::Warning )
			color = ImColor( 200, 150, 50, int( message.alpha * 255.0f ) );
		else if( message.type == DebugType::Error )
			color = ImColor( 200, 50, 50, int( message.alpha * 255.0f ) );
		else if( message.type == DebugType::Input )
			color = ImColor( 150, 100, 200, int( message.alpha * 255.0f ) );
		else if( message.type == DebugType::Performance )
			color = ImColor( 225, 180, 50, int( message.alpha * 255.0f ) );

		ImGui::TextColored( color, message.text.c_str() );
	}

	ImGui::End();
}

//--------------------------------------------------------------------------------

void MessageWindow::onUpdate( sf::Time deltaTime ) {
	// Messages
	float y = 0.0;
	for( int i = int( messages.size() ) - 1; i >= 0; --i ) {
		DebugMessage& message = messages.at( i );
		// Fade out message
		message.alpha -= 0.1f * deltaTime.asSeconds();
		// Delete invisible messages
		if( message.alpha < 0.0f ) {
			messages.erase( messages.begin() + i );
			continue;
		}
	}
}

//================================================================================

void JoystickWindow::onStart() {
	font.loadFromFile( Folders::Fonts + "MonospaceTypewriter.ttf" );
	m_text = sf::Text();
	m_text.setCharacterSize( 40u );
	m_text.setFont( font );
	m_text.setOutlineColor( sf::Color::Black );
	m_text.setOutlineThickness( 3.0f );

	m_priority = static_cast< int >( RenderPriority::UI );
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
	sf::Vector2f pos = window->mapPixelToCoords( sf::Vector2i( 3, 0 ) );
	m_text.setPosition( pos );
}

//--------------------------------------------------------------------------------

void JoystickWindow::onRender() {
	if( !m_visibility )
		return;

	m_text.setScale( getWorld()->getCamera().scale( Math::Vec2( 0.1f, 0.1f ) ).sf() );

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
	m_text.setFont( font );
	m_text.setLineSpacing( 0.9f );
	m_text.setOutlineColor( sf::Color::Black );
	m_text.setOutlineThickness( 3.0f );

	m_priority = static_cast< int >( RenderPriority::UI );
}

//--------------------------------------------------------------------------------

void PhysicsWindow::onUpdate( sf::Time deltaTime ) {
	shared_ptr< Game::Player > player = System::getWorld()->getPlayer();
	if( player == nullptr )
		return;

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
			   wallClingData.enabled ? "True" : "False", wallClingData.isClinging ? "True" : "False", wallClingData.leniency, wallClingData.power, wallClingData.min, wallClingData.max,
			   wallJumpData.enabled ? "True" : "False", wallJumpData.normal, wallJumpData.power, wallJumpData.direction.x, wallJumpData.direction.y, wallJumpData.duration.count() );

	m_text.setString( sf::String( buffer ) );

	m_text.setScale( getWorld()->getCamera().scale( Math::Vec2( 0.125f, 0.125f ) ).sf() );

	sf::RenderWindow* window = System::getWindow();
	sf::Vector2f pos = window->mapPixelToCoords( sf::Vector2i( 3, 0 ) );
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