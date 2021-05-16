//================================================================================

#pragma once

//================================================================================

#include "Box.h"

//================================================================================

class Player : public Box
{
public:
	Player();
	Player( Vec2 pos );

// Events
public:
	void onUpdate( double deltaTime ) override;
	void onRender() override;

	void onKeyboardPress( int key ) override;
	void onKeyboardRelease( int key ) override;

	void onCollision( shared_ptr< Object > target, Vec2 normal ) override;

// Variables
private:
	Vec2 m_velocity;
	Vec2 m_acceleration;

// Constants
private:
	const double m_moveSpeed{ 1.0 };
	const double m_terminalVelocity{ 1.0 };
	const Vec2 m_size{ 0.1, 0.15 };
};