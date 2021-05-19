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

// Methods
public:
	void jump();
	void dash();

// Variables
private:
	Vec2 m_velocity;

	bool m_canJump{ false };
	bool m_canDoubleJump{ false };
	bool m_waitJump{ false };

	bool m_canDash{ true };
	bool m_waitDash{ false };
	bool m_isDashing{ false };

	bool m_isWallCling{ false };
	double m_wallClingNormal{ 0.0 };

// Constants
private:
	const double m_moveAcceleration{ 50.0 };
	const double m_maxMoveSpeed{ 2.0 };

	const double m_terminalVelocity{ 4.0 };

	const double m_frictionMultiplier{ 20.0 };
	const double m_minFriction{ 2.0 };
	const double m_maxFriction{ 50.0 };

	const double m_jumpPower{ 4.0 };
	const double m_doubleJumpPower{ 2.25 };
	const double m_jumpReleaseThreshold{ 1.5 };

	const double m_dashPower{ 7.5 };
	const milliseconds m_dashCooldown{ 1500 };
	const milliseconds m_dashReleaseTimer{ 150 };

	const double m_wallFrictionMultiplier{ 5.0 };
	const double m_minWallFriction{ 1.0 };
	const double m_maxWallFriction{ 5.0 };

	const double m_wallJumpPower{ 10.0 };
	const Vec2 m_wallJumpNormal{ 0.65, 0.35 };
};