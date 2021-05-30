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
	void onEvent( sf::Event e ) override;
	void onRender() override;

	void onCollision( Collision::CollisionResult collision, shared_ptr< Object > target ) override;

	// Methods
public:
	void jump();
	void dash();
	void kill();

	Math::Vec2 getSpawn() const { return m_spawn; }
	void setSpawn( Math::Vec2 pos ) { m_spawn = pos; }

	// Variables
private:
	Math::Vec2 m_spawn;

	// Timers
private:
	Timers::TimerID m_dashCooldownTimer;
	Timers::TimerID m_dashAnimationTimer;
	Timers::TimerID m_dashTimer;

	// Attributes
private:
	struct {
		const Math::Vec2 size{ 8.0f, 12.0f };
		const Math::Color color{ 0.2f, 0.2f, 0.2f, 1.0f };

		vector< shared_ptr< RigidRect > > dashShadows;
	} spriteData;

	struct {
		bool enabled{ true };
		const float acceleration{ 860.0f };
		const float maxSpeed{ 80.0f };
		const float airMultiplier{ 0.75f };
	} movementData;

	struct {
		bool enabled{ true };
		const float power{ 288.0f };
		const float max{ 196.0f };
	} gravityData;

	struct {
		bool enabled{ true };
		const float power{ 80.0f };
		const float min{ 128.0f };
		const float max{ 800.0f };
		const float airMultiplier{ 0.4f };
	} frictionData;

	struct {
		bool enabled{ true };
		bool canJump{ false };
		bool canJumpDown{ false };
		bool isJumpingDown{ false };
		bool wait{ false };

		const float power{ 152.0f };
		const float release{ 72.0f };
	} jumpData;

	struct {
		bool enabled{ false };
		bool canDoubleJump{ true };
		const float power{ 0.0f }; // 96.0f
	} doubleJumpData;

	struct {
		bool enabled{ true };
		bool canDash{ true };
		bool isDashing{ false };
		bool wait{ false };

		const float power{ 280.0f };
		const Math::Vec2 release{ 32.0f, 32.0f };
		const milliseconds cooldown{ 750 };
		const milliseconds duration{ 150 };
		
		const milliseconds animationStep{ 15 };
	} dashData;

	struct {
		bool enabled{ true };
		bool isClinging{ false };
		
		const float multiplier{ 0.2f };
		const float min{ 32.0f };
		const float max{ 128.0f };
	} wallClingData;

	struct {
		bool enabled{ true };
		float normal{ 0.0f };

		const float power{ 356.0f };
		const Math::Vec2 direction{ 0.65f, 0.35f };
		const milliseconds duration{ 300 };
	} wallJumpData;
};

//================================================================================

} // Game

//================================================================================