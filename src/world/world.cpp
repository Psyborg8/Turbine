//================================================================================

#include "world.h"

//--------------------------------------------------------------------------------

#include "system.h"
#include "player.h"
#include "camera.h"

//================================================================================

World::World() : Object() {
	m_visibility = true;
}

//--------------------------------------------------------------------------------

void World::onSpawnChildren() {

}

//--------------------------------------------------------------------------------

void World::update( sf::Time deltaTime ) {
	if( isMarkedForRemoval() )
		return;

	sf::Time dt = sf::seconds( deltaTime.asSeconds() * m_speed );

	Timers::update( dt );

	onUpdate( dt );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->update( dt );
}

//--------------------------------------------------------------------------------

void World::postUpdate( sf::Time deltaTime ) {
	if( isMarkedForRemoval() )
		return;

	sf::Time dt = sf::seconds( deltaTime.asSeconds() * m_speed );

	onPostUpdate( dt );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->postUpdate( dt );
}

//--------------------------------------------------------------------------------

void World::render( sf::RenderTarget* target ) {
	if( isMarkedForRemoval() )
		return;

	m_camera.calculate( target );

	onRender( target );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->render( target );
}

//================================================================================
