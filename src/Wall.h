//================================================================================

#pragma once

//================================================================================

#include "Quad.h"

//================================================================================

class Wall : public Quad
{
public:
	Wall();
	Wall( float width, float height, b2Vec2 center, float rotation );

// Events
public:
	void onRender() override;

// Get-Set
public:
	inline const b2PolygonShape& getCollider() { return m_collider; }
	
protected:
	void calculateCollider();

// Variables
protected:
	float m_length;
	float m_width;
	b2Vec2 m_middle;
	float m_rotation;

	b2PolygonShape m_collider;
};

//================================================================================