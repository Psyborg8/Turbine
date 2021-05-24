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

	void loadWorld( WorldData data );
	void unloadWorld();

	// Variables
protected:
	double m_cameraMoveMultiplier{ 6.0 };

	Math::Vec2 m_playerStart{ 0.0, 0.0 };
	size_t m_checkpoints{ 0u };
};

//================================================================================

} // Worlds

//================================================================================