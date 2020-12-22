//================================================================================

#pragma once

//================================================================================

#include "Quad.h"
#include "Line.h"
#include "Observers.h"
#include "MathTypes.h"

//================================================================================

class Player : public Object
{
public:
	Player();

public:

private:
	virtual void onSpawnChildren() override;
	virtual void onStart() override;
	virtual void onUpdate( float deltaTime ) override;
	
	virtual void onMouseMove( float x, float y ) override;

private:

public:
	inline Color getColor() { return m_color; }
	void setColor( Color color );

	inline Vec2 getPosition() { return m_position; }
	inline void setPosition( Vec2 position ) { m_position = position; }

private:
	const float m_acceleration{ 0.02f };
	const float m_gravity{ 0.0f };
	const float m_maxVelocity{ 0.2f };

	b2PolygonShape m_collider;

	Vec2 m_velocity;
	Vec2 m_position;
	Vec2 m_target;

	Color m_color;

	shared_ptr< Quad > m_face;
	shared_ptr< Line > m_line;
};