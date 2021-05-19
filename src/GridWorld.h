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
	virtual void onUpdate( double deltaTime ) override;
	virtual void onStart() override;

// Variables
protected:
	double m_gridSize{ 0.5 };
	double m_cameraMoveMultiplier{ 6.0 };

	vector< vector< int > > m_worldData;
};

//================================================================================