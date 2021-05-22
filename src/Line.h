//================================================================================

#pragma once

//================================================================================

#include "Collision.h"
#include "MathTypes.h"

//================================================================================

namespace Gfx
{

//================================================================================

struct Line : public Collision::Ray
{
	Color color = Colors::WHITE;
	float weight = 3.0f;

	void render();
};

//================================================================================

} // namespace Gfx

//================================================================================