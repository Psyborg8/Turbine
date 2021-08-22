//================================================================================

#include "World.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

namespace Worlds {

//================================================================================

World::World() : Object() {
	m_visibility = true;
}

//--------------------------------------------------------------------------------

void World::onSpawnChildren() {
	m_camera = Gfx::Camera();
}

//--------------------------------------------------------------------------------

void World::update( sf::Time deltaTime ) {
	if( isMarkedForRemoval() )
		return;

	sf::Time dt = sf::seconds( deltaTime.asSeconds() * m_speed );

	onUpdate( dt );

	for( shared_ptr< Object > child : m_children )
		child->update( dt );
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

//================================================================================

} // Worlds

//================================================================================
