#pragma once

//================================================================================

#include "Object.h"
#include "MathTypes.h"
#include "Shader.h"

//================================================================================

class Box : public Object
{
public:
	Box();
	Box( c2AABB box, Color color );

// Events
public:
	void onRender() override;

// Get-Set
public:
	inline c2AABB getBox() const { return m_box; }
	inline void setBox( c2AABB box ) { m_box = box; }

	Vec2 getMidpoint();
	Vec2 getSize();

// Transform
public:
	void moveBox( Vec2 direction );
	void scaleBox( Vec2 scale );

// Rendering
protected:
	array< Vec2, 4 > calculateVertices();

// Variables
protected:
	c2AABB m_box;
	Color m_color;
};

//================================================================================
