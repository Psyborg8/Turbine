// ======================================================================

#include "app.h"

// ======================================================================

#include "system.h"

// ======================================================================

App::App() : Object() {
	m_visibility = true;
}

// ----------------------------------------------------------------------

void App::update( sf::Time _delta ) {
	if( isMarkedForRemoval() )
		return;

	sf::Time dt = sf::seconds( _delta.asSeconds() * m_speed );

	Timers::update( dt );

	onUpdate( dt );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->update( dt );
}

// ----------------------------------------------------------------------

void App::postUpdate( sf::Time _delta ) {
	if( isMarkedForRemoval() )
		return;

	sf::Time dt = sf::seconds( _delta.asSeconds() * m_speed );

	onPostUpdate( dt );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->postUpdate( dt );
}

// ----------------------------------------------------------------------

void App::render( sf::RenderTarget* _target ) {
	if( isMarkedForRemoval() )
		return;

	m_camera.calculate( _target );

	onRender( _target );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->render( _target );
}

// ======================================================================
