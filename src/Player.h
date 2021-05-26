//================================================================================

#pragma once

//================================================================================

#include "RigidRect.h"
#include "Timer.h"

//================================================================================

namespace Game {

//================================================================================

class Player : public RigidRect {
public:
	Player();
	Player( Math::Vec2 pos );

	// Events
public:
	void onUpdate( sf::Time deltaTime ) override;
	void onProcessCollisions() override;
	void onDestroy() override;
	void onEvent( sf::Event e );

	void onCollision( Collision::CollisionResult collision, shared_ptr< Object > target ) override;

	// Methods
public:
	void jump();
	void dash();
	void kill();

	// Attributes
private:
	struct {
		const float acceleration{ 20.0f };
		const float maxSpeed{ 2.0f };
		const float airMultiplier{ 0.9f };
	} movementData;

	struct {
		const float power{ 6.0f };
		const float max{ 4.0f };
	} gravityData;

	struct {
		const float power{ 20.0f };
		const float min{ 2.0f };
		const float max{ 50.0f };
		const float airMultiplier{ 0.4f };
	} frictionData;

	struct {
		bool canJump{ false };
		bool canJumpDown{ false };
		bool isJumpingDown{ false };
		bool wait{ false };

		const float power{ 3.5f };
		const float release{ 1.5f };
	} jumpData;

	struct {
		bool canDoubleJump{ true };
		const float power{ 2.75f };
	} doubleJumpData;

	struct {
		bool canDash{ true };
		bool isDashing{ false };
		bool wait{ false };

		const float power{ 7.5f };
		const milliseconds cooldown{ 1500 };
		const milliseconds duration{ 150 };

		const milliseconds animationStep{ 15 };
	} dashData;

	struct {
		bool isClinging{ false };

		const float multiplier{ 0.2f };
		const float min{ 1.0f };
		const float max{ 5.0f };
	} wallClingData;

	struct {
		float normal{ 0.0f };

		const float power{ 8.0f };
		const Math::Vec2 direction{ 0.7f, 0.3f };
		const milliseconds duration{ 300 };
	} wallJumpData;

// Timers
private:
	Timers::TimerID m_dashCooldownTimer;
	Timers::TimerID m_dashAnimationTimer;
	Timers::TimerID m_dashTimer;
};

//================================================================================

} // Game

//================================================================================