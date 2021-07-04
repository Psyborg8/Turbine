//================================================================================

#include "GameTimer.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "Debug.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

GameTimer::GameTimer() : Object() {

}

//--------------------------------------------------------------------------------

void GameTimer::reset() {
	m_splits.clear();
	m_clock.restart();
	m_active = true;

	m_splitText.setString( "" );
}

//--------------------------------------------------------------------------------

void GameTimer::split() {
	sf::Time time = m_clock.getElapsedTime();
	m_splits.push_back( time );

	char buffer[ 32 ];
	sprintf_s( buffer, "%.2f", time.asSeconds() );

	string text = m_splitText.getString();
	text += "Checkpoint " + std::to_string( m_splits.size() ) + ": " + buffer + "\n";
	m_splitText.setString( text );
}

//--------------------------------------------------------------------------------

void GameTimer::stop() {
	m_active = false;

	sf::Time time = m_clock.getElapsedTime();

	char buffer[ 32 ];
	sprintf_s( buffer, "%.2f", time.asSeconds() );

	string text = m_splitText.getString();
	text += "Level End: " + string( buffer );
	m_splitText.setString( text );
}

//--------------------------------------------------------------------------------

void GameTimer::onStart() {
	m_font.loadFromFile( Folders::Fonts + "MonospaceTypewriter.ttf" );

	m_timerText.setFont( m_font );
	m_timerText.setCharacterSize( 40u );
	m_timerText.setScale( sf::Vector2f( 0.25f, 0.25f ) / System::getWorld()->getCamera().getDistance() * 256.0f );
	m_timerText.setOutlineThickness( 3.0f );
	m_timerText.setOutlineColor( sf::Color::Black );

	m_splitText.setFont( m_font );
	m_splitText.setCharacterSize( 40u );
	m_splitText.setScale( sf::Vector2f( 0.1f, 0.1f ) / System::getWorld()->getCamera().getDistance() * 256.0f );
	m_splitText.setOutlineThickness( 3.0f );
	m_splitText.setOutlineColor( sf::Color::Black );

	m_clock.restart();
}

//--------------------------------------------------------------------------------

void GameTimer::onUpdate( sf::Time deltaTime ) {
	Debug::startTimer( "GameTimer::Update" );
	if( m_active ) {
		char buffer[ 40 ];
		sprintf_s( buffer, "%.2f", m_clock.getElapsedTime().asSeconds() );
		m_timerText.setString( buffer );
	}

	sf::RenderWindow* window = System::getWindow();
	m_timerText.setPosition( window->mapPixelToCoords( sf::Vector2i() ) );

	m_splitText.setPosition( window->mapPixelToCoords( sf::Vector2i() ) + sf::Vector2f( 0, 16.0f ) );
	Debug::stopTimer( "GameTimer::Update" );
}

//--------------------------------------------------------------------------------

void GameTimer::onRender() {
	if( !m_visibility )
		return;

	Debug::startTimer( "GameTimer::Render" );
	sf::RenderWindow* window = System::getWindow();

	window->draw( m_timerText );
	window->draw( m_splitText );
	Debug::stopTimer( "GameTimer::Render" );
}

//--------------------------------------------------------------------------------

} // Gfx

//================================================================================