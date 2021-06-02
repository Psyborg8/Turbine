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
	void onSpawnChildren() override;
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

private:
	void extendedHitbox( vector< shared_ptr< Object > > targets );

	// Variables
private:
	Math::Vec2 m_spawn;
	shared_ptr< RigidRect > m_bottomCollider;
	shared_ptr< RigidRect > m_leftCollider;
	shared_ptr< RigidRect > m_rightCollider;

	// Timers
private:
	Timers::TimerID m_dashCooldownTimer;
	Timers::TimerID m_dashAnimationTimer;
	Timers::TimerID m_dashTimer;

	// Attributes
public:
	struct {
		const Math::Vec2 size{ 8.0f, 12.0f };
		const Math::Color color{ 0.2f, 0.2f, 0.2f, 1.0f };

		vector< shared_ptr< RigidRect > > dashShadows;
	} spriteData;

	struct {
		bool enabled{ true };
		float acceleration{ 850.0f };
		float maxSpeed{ 85.0f };
		float airMultiplier{ 0.9f };
	} movementData;

	struct {
		bool enabled{ true };
		float power{ 288.0f };
		float max{ 196.0f };
	} gravityData;

	struct {
		bool enabled{ true };
		float power{ 10.0f };
		float min{ 128.0f };
		float max{ 800.0f };
		float airMultiplier{ 0.2f };
	} frictionData;

	struct {
		bool enabled{ true };
		bool canJump{ false };
		bool canJumpDown{ false };
		bool isJumpingDown{ false };

		float power{ 160.0f };
		float release{ 72.0f };
	} jumpData;

	struct {
		bool enabled{ false };
		bool canDoubleJump{ true };
		float power{ 96.0f };
	} doubleJumpData;

	struct {
		bool enabled{ true };
		bool canDash{ true };
		bool isDashing{ false };

		float power{ 280.0f };
		Math::Vec2 release{ 32.0f, 32.0f };
		milliseconds cooldown{ 1000 };
		milliseconds duration{ 150 };
		
		milliseconds animationStep{ 15 };
	} dashData;

	struct {
		bool enabled{ true };
		bool isClinging{ false };
		
		float leniency{ 2.5f };
		float multiplier{ 0.2f };
		float min{ 32.0f };
		float max{ 128.0f };
	} wallClingData;

	struct {
		bool enabled{ true };
		float normal{ 0.0f };

		float power{ 250.0f };
		Math::Vec2 direction{ 0.55f, 0.45f };
		milliseconds duration{ 200 };
	} wallJumpData;
};

//================================================================================

} // Game

//================================================================================