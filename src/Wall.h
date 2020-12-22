//================================================================================

#pragma once

//================================================================================

#include "Quad.h"

//================================================================================

class Wall : public Quad
{
public:
	Wall();
	Wall( float length, float width, Vec2 middle, float rotation, Color color );

// Events
public:
	void onRender() override;

// Get-Set
public:
	inline float getLength() { return m_length; }
	inline void setLength( float length ) { m_length = length; }

	inline float getWidth() { return m_width; }
	inline void setWidth( float width ) { m_width = width; }

	inline Vec2 getMiddle() { return m_middle; }
	inline void setMiddle( Vec2 middle ) { m_middle = middle; }

	inline float getRotation() { return m_rotation; }
	inline void setMiddle( float rotation ) { m_rotation = rotation; }

	inline Color getColor() { return m_color; }
	inline void setColor( Color color ) { Quad::m_color = color; }

	inline const b2PolygonShape& getCollider() { return m_collider; }
	
protected:
	void calculateCollider();
	void calculateQuad();

// Variables
protected:
	float m_length;
	float m_width;
	Vec2 m_middle;
	float m_rotation;

	b2PolygonShape m_collider;
};

//================================================================================