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
		const float acceleration{ 860.0f };
		const float maxSpeed{ 80.0f };
		const float airMultiplier{ 0.9f };
	} movementData;

	struct {
		const float power{ 288.0f };
		const float max{ 196.0f };
	} gravityData;

	struct {
		const float power{ 80.0f };
		const float min{ 128.0f };
		const float max{ 800.0f };
		const float airMultiplier{ 0.4f };
	} frictionData;

	struct {
		bool canJump{ false };
		bool canJumpDown{ false };
		bool isJumpingDown{ false };
		bool wait{ false };

		const float power{ 168.0f };
		const float release{ 72.0f };
	} jumpData;

	struct {
		bool canDoubleJump{ true };
		const float power{ 132.0f };
	} doubleJumpData;

	struct {
		bool canDash{ true };
		bool isDashing{ false };
		bool wait{ false };

		const float power{ 280.0f };
		const Math::Vec2 release{ 32.0f, 32.0f };
		const milliseconds cooldown{ 1000 };
		const milliseconds duration{ 150 };
		
		const milliseconds animationStep{ 15 };
	} dashData;

	struct {
		bool isClinging{ false };

		const float multiplier{ 0.2f };
		const float min{ 64.0f };
		const float max{ 256.0f };
	} wallClingData;

	struct {
		float normal{ 0.0f };

		const float power{ 256.0f };
		const Math::Vec2 direction{ 0.7f, 0.3f };
		const milliseconds duration{ 300 };
	} wallJumpData;
};

//================================================================================

} // Game

//================================================================================