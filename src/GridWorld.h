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
	virtual void reset() override;

// Variables
protected:
	double m_gridSize{ 0.35 };
	double m_cameraMoveMultiplier{ 6.0 };

	vector< string > m_worldData;

	Vec2 m_playerStart;
};

//================================================================================