//================================================================================

#pragma once

//================================================================================

#include "World.h"

//================================================================================

class DevWorld : public World
{
public:
	DevWorld();

// Events
public:
	virtual void onSpawnChildren() override;
};
