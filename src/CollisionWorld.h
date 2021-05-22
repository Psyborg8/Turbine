//================================================================================

#pragma once

//================================================================================

#include "World.h"
#include "Rect.h"
#include "Line.h"

//================================================================================

class CollisionWorld : public World
{
public:
	CollisionWorld() : World() {}

public:
	void onSpawnChildren() override;
	void onUpdate( double deltaTime ) override;
	void onRender() override;

private:
	Gfx::Rect m_player;
	vector< Gfx::Rect > m_walls;
	vector< Gfx::Line > m_lines;
};

//================================================================================