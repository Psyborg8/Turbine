//================================================================================

#pragma once

//================================================================================

#include "Observers.h"
#include "RigidRect.h"

//================================================================================

class Player : public RigidRect {
public:
	Player();
	Player( Vec2 pos );

// Events
public:
	void onUpdate( double deltaTime ) override;
	void onProcessCollisions() override;
	void onRender() override;
	void onDestroy() override;
	void onCreateObservers() override;

	void onKeyboardPress( int key );
	void onKeyboardRelease( int key );

	void onCollision( Collision::CollisionResult collision, ObjectPtr target ) override;

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

// Attributes
private:
	struct {
		const double acceleration{ 20.0 };
		const double maxSpeed{ 2.0 };
		const double airMultiplier{ 0.9 };
	} movementData;

	struct {
		const double power{ 6.0 };
		const double max{ 4.0 };
	} gravityData;

	struct {
		const double power{ 20.0 };
		const double min{ 2.0 };
		const double max{ 50.0 };
		const double airMultiplier{ 0.4 };
	} frictionData;

	struct {
		bool canJump{ false };
		bool canJumpDown{ false };
		bool isJumpingDown{ false };
		bool wait{ false };

		const double power{ 3.5 };
		const double release{ 1.5 };
	} jumpData;

	struct {
		bool canDoubleJump{ true };
		const double power{ 2.75 };
	} doubleJumpData;

	struct {
		bool canDash{ true };
		bool isDashing{ false };
		bool wait{ false };

		const double power{ 7.5 };
		const milliseconds cooldown{ 1500 };
		const milliseconds duration{ 150 };
	} dashData;

	struct {
		bool isClinging{ false };

		const double multiplier{ 0.2 };
		const double min{ 1.0 };
		const double max{ 5.0 };
	} wallClingData;

	struct {
		double normal{ 0.0 };

		const double power{ 8.0 };
		const Vec2 direction{ 0.7, 0.3 };
		const milliseconds duration{ 300 };
	} wallJumpData;
};

//================================================================================