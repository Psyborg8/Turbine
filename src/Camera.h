//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "Object.h"

//================================================================================

class Camera : public Object
{
public:
	Camera( b2Vec2 position, float distance );

// Get-Set
public:
	inline b2Vec2 getPosition() { return m_position; }
	inline void setPosition( b2Vec2 position ) { m_position = position; calculate(); }

	inline float getDistance() { return m_distance.y; }
	inline void setDistance( float distance ) { m_distance.y = distance; calculate(); }

// Conversions
public:
	b2Vec2 screenToWorld( b2Vec2 screenPosition );
	b2Vec2 worldToScreen( b2Vec2 worldPosition );

// Utility
public:
	void calculate();

// Variables
protected:
	b2Vec2 m_position;
	b2Vec2 m_distance;
};

//================================================================================