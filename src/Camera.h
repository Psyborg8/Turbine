//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "Object.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

class Camera {
public:
	Camera();

	// Get-Set
public:
	inline Math::Vec2 getPosition() { return m_position; }
	inline void setPosition( Math::Vec2 position ) { m_position = position; calculate(); }

	inline float getDistance() { return m_distance.y; }
	inline void setDistance( float distance ) { m_distance.y = distance; calculate(); }

	inline float getRotation() { return m_rotation; }
	inline void setRotation( float rotation ) { m_rotation = rotation; calculate(); }

	// Utility
public:
	void calculate();

	// Variables
protected:
	Math::Vec2 m_position;
	Math::Vec2 m_distance;
	float m_rotation;
};

//--------------------------------------------------------------------------------

} // Gfx

//================================================================================