//================================================================================

#pragma once

//================================================================================

#include "World.h"
#include "Player.h"
#include "Wall.h"

//================================================================================

class DevWorld : public World
{
public:
	DevWorld();

	void onSpawnChildren() override;
	void onStart() override;
	void onUpdate( float deltaTime ) override;
	void onRender() override;

private:
	shared_ptr< Player > m_player;
	vector< shared_ptr< Wall > > m_walls;
};

//================================================================================
