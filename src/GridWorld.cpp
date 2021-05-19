//================================================================================

#include "GridWorld.h"

//--------------------------------------------------------------------------------

#include "Box.h"
#include "Player.h"

//================================================================================

GridWorld::GridWorld() : World()
{
	m_worldData = {
	{ 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 1 },
	{ 1, 2, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1 }
	};
}

//--------------------------------------------------------------------------------

void GridWorld::onSpawnChildren()
{
	for( int y = 0; y < m_worldData.size(); ++y )
	{
		for( int x = 0; x < m_worldData.size(); ++y )
		{
			const Vec2 pos = Vec2( x, y ) * m_gridSize;
			const BlockType block = static_cast< BlockType >( m_worldData.at( y ).at( x ) );

			if( block == BlockType::Wall )
			{
				Box box = Box( pos, Vec2( m_gridSize, m_gridSize ), Colors::WHITE );
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