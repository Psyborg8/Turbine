//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "Object.h"

//================================================================================

namespace Gfx {

class Camera {
public:
	Camera();

	// Get-Set
public:
	inline Math::Vec2 getPosition() { return m_position; }
	inline void setPosition( Math::Vec2 position ) { m_position = position; calculate(); }

	inline double getDistance() { return m_distance.y; }
	inline void setDistance( float distance ) { m_distance.y = distance; calculate(); }

	// Conversions
public:
	Math::Vec2 screenToWorld( Math::Vec2 screenPosition );
	Math::Vec2 worldToScreen( Math::Vec2 worldPosition );

	// Utility
public:
	void calculate();

	// Variables
protected:
	Math::Vec2 m_position;
	Math::Vec2 m_distance;
};

//================================================================================

} // Gfx

//================================================================================