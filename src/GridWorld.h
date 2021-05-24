//================================================================================

#pragma once

//================================================================================

#include "World.h"

//================================================================================

namespace Worlds {

//================================================================================

enum class BlockType {
	None,
	Wall,
	Player,
};

//--------------------------------------------------------------------------------

struct WorldData {
	string name;
	vector< string > blockData;
	double gridSize;
	double cameraDistance;
};

//================================================================================

class GridWorld : public World {
public:
	GridWorld();

	// Events
public:
	virtual void onSpawnChildren() override;
	virtual void onUpdate( double deltaTime ) override;
	virtual void onStart() override;
	virtual void reset() override;

	void loadWorld( string name ) override;
	void unloadWorld();

	void setCheckpoint( size_t checkpoint ) override;

	// Variables
protected:
	double m_cameraMoveMultiplier{ 6.0 };

	Math::Vec2 m_playerStart{ 0.0, 0.0 };
	vector< WorldData > m_worldData;
};

//================================================================================

} // Worlds

//================================================================================