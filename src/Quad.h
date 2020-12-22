#pragma once

//================================================================================

#include "Object.h"
#include "MathTypes.h"
#include "Shader.h"

//================================================================================

class Quad : public Object, public b2PolygonShape
{
public:
	Quad();
	Quad( float width, float height, b2Vec2 center, float rotation );

	void onRender() override;

public:
	inline float getWidth() { return m_width; }
	inline void setWidth( float width ) { m_width = width; calculate(); }

	inline float getHeight() { return m_height; }
	inline void setHeight( float height ) { m_height = height; calculate(); }

	inline b2Vec2 getPosition() { return m_position; }
	inline void setPosition( b2Vec2 position ) { m_position = position; calculate(); }

	inline float getRotation() { return m_rotation; }
	inline void setRotation( float rotation ) { m_rotation = rotation; calculate(); }

	inline Color getColor() { return m_color; }
	inline void setColor( Color color ) { m_color = color; }

protected:
	void calculate();

protected:
	float m_width;
	float m_height;
	b2Vec2 m_position;
	float m_rotation;
	Color m_color;
};

//================================================================================
