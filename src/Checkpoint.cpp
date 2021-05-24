//================================================================================

#include "Checkpoint.h"

//--------------------------------------------------------------------------------

#include "Player.h"
#include "System.h"

//================================================================================

namespace Game {

//================================================================================

void Checkpoint::onSpawnChildren() {
	lineColor = Colors::BLACK;

	m_collider = makeObject< Game::RigidRect >( this );
	m_flag = makeObject< Game::RigidRect >( this );

}

//--------------------------------------------------------------------------------

void Checkpoint::onStart() {
	m_collider->size = Math::Vec2( 1.0, 1.0 );
	m_collider->setMidpoint( midpoint() );
	m_collider->fill = false;

	m_flag->size = Math::Vec2( size.x * 5, size.y / 4.0 );
	m_flag->position = Math::Vec2( position.x + size.x, position.y + size.y - m_flag->size.y );
	m_flag->fillColor = Math::Color( 0.7f, 0.2f, 0.2f, 1.0f );
	m_flag->fill = false;
	m_flag->lineColor = Colors::BLACK;
	m_flag->lineWeight = 1.0f;

	line = true;
	lineWeight = 1.0f;
	lineColor = Colors::BLACK;
}

//--------------------------------------------------------------------------------

void Checkpoint::onProcessCollisions() {
	if( m_flag->fill )
		return;

	vector< shared_ptr< Player > > players = getObjects< Player >( System::getWorld() );
	if( players.empty() )
		return;
	shared_ptr< Player > player = players.at( 0 );
	
	Collision::CollisionResult result = isColliding( player );
	if( result.success )
	{
		m_flag->fill = true;
		System::getWorld()->setCheckpoint( m_number );
	}
}

//================================================================================

} // Game

//================================================================================