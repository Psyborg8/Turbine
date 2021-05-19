//================================================================================

#include "GridWorld.h"

//--------------------------------------------------------------------------------

#include "Box.h"
#include "Player.h"

//================================================================================

GridWorld::GridWorld() : World()
{
	m_worldData = {
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1 },
		{ 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0 },
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0 },
		{ 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
		{ 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1 },
		{ 1, 2, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};

	m_gravity = Vec2( 0.0, -6.0 );
}

//--------------------------------------------------------------------------------

void GridWorld::onSpawnChildren()
{
	World::onSpawnChildren();

	for( int y = 0; y < m_worldData.size(); ++y )
	{
		for( int x = 0; x < m_worldData.at( y ).size(); ++x )
		{
			const Vec2 pos = Vec2( x, -y ) * m_gridSize;
			const BlockType block = static_cast< BlockType >( m_worldData.at( y ).at( x ) );

			if( block == BlockType::Wall )
			{
				Box box = Box( pos, Vec2( m_gridSize, m_gridSize ), Colors::WHITE );
				box.setCollisionType( CollisionType::StaticBlocking );
				Object::makeObject< Box >( box, this );
			}
			else if( block == BlockType::Player )
			{
				Player player = Player( pos );
				Object::makeObject< Player >( player, this );
			}
		}
	}
}

void GridWorld::onStart()
{
	const shared_ptr< Player > player = Object::getObjectsByType< Player >().at( 0 );
	m_camera->setDistance( 2.0 );
	m_camera->setPosition( player->getPos() );
}

//--------------------------------------------------------------------------------

void GridWorld::onUpdate( double deltaTime )
{
	const shared_ptr< Player > player = Object::getObjectsByType< Player >().at( 0 );
	const Vec2 pos = player->getPos();
	const Vec2 direction = pos - m_camera->getPosition();
	if( direction.length() )
	{
		const Vec2 diff = direction * m_cameraMoveMultiplier * deltaTime;

		Vec2 newPos = m_camera->getPosition() + diff;
		if( direction.length() < diff.length() )
			newPos = pos;

		m_camera->setPosition( newPos );
	}
}