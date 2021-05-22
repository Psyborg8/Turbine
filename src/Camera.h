//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "Object.h"

//================================================================================

class Camera
{
public:
	Camera();

// Get-Set
public:
	inline Vec2 getPosition() { return m_position; }
	inline void setPosition( Vec2 position ) { m_position = position; calculate(); }

	inline double getDistance() { return m_distance.y; }
	inline void setDistance( float distance ) { m_distance.y = distance; calculate(); }

// Conversions
public:
	Vec2 screenToWorld( Vec2 screenPosition );
	Vec2 worldToScreen( Vec2 worldPosition );

// Utility
public:
	void calculate();

// Variables
protected:
	Vec2 m_position;
	Vec2 m_distance;
};

//================================================================================