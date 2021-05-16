//================================================================================

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
	Box( Vec2 pos, Vec2 size, Color color );

// Events
public:
	virtual void onRender() override;
	virtual void onCollision( shared_ptr< Object > target, Vec2 normal ) override;

// Get-Set
public:
	inline Vec2 getSize() const { return m_size; }
	inline void setSize( Vec2 size ) { m_size = size; }

	inline Vec2 getPos() const { return m_pos; }
	inline void setPos( Vec2 pos ) { m_pos = pos; }

	inline Color getColor() const { return m_color; }
	inline void setColor( Color color ) { m_color = color; }

	Vec2 getMidpoint() const;

// Collision
	virtual bool isColliding( shared_ptr< Object > target, Vec2& normal ) override;

	bool isColliding( shared_ptr< Box > target, Vec2& normal );
	bool isColliding( Vec2 target, Vec2& normal );

// Rendering
protected:
	array< Vec2, 4 > calculateVertices();

// Variables
protected:
	Vec2 m_size;
	Vec2 m_pos;
	Color m_color;
};