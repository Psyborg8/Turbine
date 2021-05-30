//================================================================================

#include "Debug.h"

//--------------------------------------------------------------------------------

#include "System.h"

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
}

//--------------------------------------------------------------------------------

float getAverageTime( string name ) {
	startTimer( "Debug::Average Time" );
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

	stopTimer( "Debug::Average Time" );

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
		color = sf::Color( 200, 150, 200, 255 );
	else if( out.type == DebugType::Warning )
		color = sf::Color( 200, 150, 50, 255 );
	else if( out.type == DebugType::Error )
		color = sf::Color( 200, 50, 50, 255 );

	out.text.setFillColor( color );

	messages.push_back( out );
}

//================================================================================

void DebugWindow::onStart() {
	font.loadFromFile( Folders::Fonts + "MonospaceTypewriter.ttf" );
	m_text = sf::Text();
	m_text.setCharacterSize( 40u );
	m_text.setScale( sf::Vector2f( 0.1f, 0.1f ) );
	m_text.setFont( font );
	m_text.setOutlineColor( sf::Color::Black );
	m_text.setOutlineThickness( 3.0f );
}

//--------------------------------------------------------------------------------

void DebugWindow::onUpdate( sf::Time deltaTime ) {
	startTimer( "Debug::Update" );
	if( m_first ) {
		std::fill( m_deltaTimes.begin(), m_deltaTimes.end(), deltaTime );
		m_iterator = m_deltaTimes.begin();
		m_first = false;
		return;
	}

	sf::Time& time = *m_iterator;
	time = deltaTime;

	if( ++m_iterator == m_deltaTimes.end() )
		m_iterator = m_deltaTimes.begin();

	float pos = 0.0;
	for( DebugMessage& message : messages ) {
		message.alpha -= 2.0f * deltaTime.asSeconds();
		if( message.alpha < 0.0f ) {
			const auto it = std::find_if( messages.begin(), messages.end(),
										  [message]( const DebugMessage& rh ) {
											  return rh.text.getString() == message.text.getString() &&
												  rh.alpha == message.alpha;
										  } );
			if( it != messages.end() )
				messages.erase( it );
			continue;
		}

		sf::Color color = message.text.getFillColor();
		color.a = sf::Uint8( message.alpha * 255.0f );
		message.text.setFillColor( color );

		Math::Vec2 position = Math::Vec2( System::getWindow()->mapPixelToCoords( sf::Vector2i( System::getSystemInfo().width, 0 ) ) );
		position.y += pos;
		position.x -= message.text.getLocalBounds().width;

		message.text.setPosition( position.sf() );

		pos += 10.0f;
	}

	stopTimer( "Debug::Update" );
}

//--------------------------------------------------------------------------------

void DebugWindow::onRender() {

	startTimer( "Debug::Render" );
	// FPS
	float total = 0.0f;
	for( const sf::Time& time : m_deltaTimes )
		total += time.asSeconds();

	const float average = total / m_deltaTimes.size();
	const int fps = int( 1.0f / average );

	// Get screen position
	sf::RenderWindow* window = System::getWindow();
	sf::Vector2f pos = window->mapPixelToCoords( sf::Vector2i() );

	m_text.setPosition( pos + sf::Vector2f( 0.0f, 0.0f ) );

	// Construct string
	string s = std::to_string( fps ) + "fps\n";
	int i = 2;

	for( auto& timer : timers ) {
		char buffer[ 40 ];
		s += timer.name + ": ";
		sprintf_s( buffer, "%.3f", getAverageTime( timer.name ) * 1000.0f );
		s += buffer;
		s += "ms\n";
	}

	// Render string
	m_text.setString( sf::String( s ) );
	window->draw( m_text );

	// Messages
	for( const DebugMessage& message : messages ) {
		window->draw( message.text );
	}

	stopTimer( "Debug::Render" );
}

//--------------------------------------------------------------------------------

}

//================================================================================