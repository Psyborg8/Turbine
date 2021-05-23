//================================================================================

#pragma once

//================================================================================

#include "Observers.h"
#include "RigidRect.h"

//================================================================================

class Player : public RigidRect
{
public:
	Player();
	Player( Vec2 pos );

// Events
public:
	void onUpdate( double deltaTime ) override;
	void onProcessCollisions( double deltaTime ) override;
	void onRender() override;
	void onDestroy() override;
	void onCreateObservers() override;

	void onKeyboardPress( int key );
	void onKeyboardRelease( int key );

	void onCollision( Collision::CollisionResult collision, shared_ptr< Object > target ) override;

// Methods
public:
	void jump();
	void dash();
	void kill();

// Observers
private:
	ObserverID m_keyPressObserver;
	ObserverID m_keyReleaseObserver;

	vector< function< void() > > m_renders;

// Variables
private:
	bool m_canJump{ false };
	bool m_canDoubleJump{ true };
	bool m_waitJump{ false };

	bool m_canJumpDown{ false };
	bool m_jumpingDown{ false };

	bool m_canDash{ true };
	bool m_waitDash{ false };
	bool m_isDashing{ false };

	bool m_isWallCling{ false };
	double m_wallClingNormal{ 0.0 };

// Constants
private:
	const double m_moveAcceleration{ 20.0 };
	const double m_maxMoveSpeed{ 2.0 };
	const double m_airMoveMultiplier{ 0.8 };

	const double m_terminalVelocity{ 4.0 };

	const double m_frictionMultiplier{ 20.0 };
	const double m_minFriction{ 2.0 };
	const double m_maxFriction{ 50.0 };

	const double m_jumpPower{ 4.0 };
	const double m_doubleJumpPower{ 2.25 };
	const double m_jumpReleaseThreshold{ 1.5 };

	const double m_dashPower{ 7.5 };
	const milliseconds m_dashCooldown{ 1500 };
	const milliseconds m_dashReleaseTime{ 150 };

	const double m_wallFrictionMultiplier{ 5.0 };
	const double m_minWallFriction{ 1.0 };
	const double m_maxWallFriction{ 5.0 };

	const double m_wallJumpPower{ 8.5 };
	const Vec2 m_wallJumpNormal{ 0.7, 0.3 };
	const milliseconds m_wallJumpReleaseTime{ 300 };
};

//================================================================================