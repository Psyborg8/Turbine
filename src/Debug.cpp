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

vector< Timer > timers;

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

	for( auto& timer : timers )
		s += timer.name + ": " + std::to_string( getAverageTime( timer.name ) * 1000.0f ) + "ms\n";

	// Render string
	m_text.setString( sf::String( s ) );
	window->draw( m_text );
	stopTimer( "Debug::Render" );
}

//--------------------------------------------------------------------------------

}

//================================================================================