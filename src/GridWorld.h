//================================================================================

#pragma once

//================================================================================

#include "World.h"

//================================================================================

enum class BlockType
{
	None,
	Wall,
	Player,
};

//================================================================================

class GridWorld : public World
{
public:
	GridWorld();

// Events
public:
	virtual void onSpawnChildren() override;

// Variables
protected:
	double m_gridSize{ 0.5 };

	vector< vector< int > > m_worldData;
};

//================================================================================