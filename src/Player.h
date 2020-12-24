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

private:
	virtual void onSpawnChildren() override;
	virtual void onStart() override;
	virtual void onUpdate( float deltaTime ) override;
	
	virtual void onMouseMove( float x, float y ) override;

public:
	inline Color getColor() { return m_color; }
	void setColor( Color color );

	inline Vec2 getPosition() { return m_position; }
	inline void setPosition( Vec2 position ) { m_position = position; }

private:
	const float m_acceleration{ 0.5f };
	const float m_maxVelocity{ 1.0f };

	Vec2 m_velocity;
	Vec2 m_position;
	Vec2 m_target;

	Color m_color;

	shared_ptr< Box > m_face;
	shared_ptr< Line > m_line;
};