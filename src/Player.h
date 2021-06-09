//================================================================================

#pragma once

//================================================================================

#include "RigidRect.h"
#include "Timer.h"
#include "Input.h"

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

	// Keybinds
public:
	void jump( bool pressed );
	void dash( bool pressed );
	void dashBounce();
	void attack( bool pressed );

	void kill( bool pressed, bool restart );
	void debug( bool pressed );

	Math::Vec2 getSpawn() const { return m_spawn; }
	void setSpawn( Math::Vec2 pos ) { m_spawn = pos; }

private:
	void extendedHitbox( vector< shared_ptr< Object > > targets );
	void attackHitbox( vector< shared_ptr< Object > > targets );

	// Variables
private:
	Math::Vec2 m_spawn;
	shared_ptr< RigidRect > m_bottomCollider;
	shared_ptr< RigidRect > m_leftCollider;
	shared_ptr< RigidRect > m_rightCollider;
	shared_ptr< RigidRect > m_attackCollider;

	// Timers
private:
	Timers::TimerID m_dashCooldownTimer;
	Timers::TimerID m_dashAnimationTimer;
	Timers::TimerID m_dashTimer;
	Timers::TimerID m_dashBounceTimer;
	Timers::TimerID m_attackTimer;
	Timers::TimerID m_attackRenderTimer;

	// Attributes
public:
	struct {
		struct {
			ControllerButton jump{ ControllerButton::Bottom };
			ControllerButton attack{ ControllerButton::RTrigger };
			ControllerButton reset{ ControllerButton::Start };
			ControllerButton restart{ ControllerButton::Select };
			ControllerButton debug{ ControllerButton::Logo };
			ControllerAxis moveX{ ControllerAxis::LeftStickX };
			ControllerAxis moveY{ ControllerAxis::LeftStickY };
		} controller;

		struct {
			sf::Keyboard::Key jump{ sf::Keyboard::Space };
			sf::Keyboard::Key attack{ sf::Keyboard::E };
			sf::Keyboard::Key reset{ sf::Keyboard::R };
			sf::Keyboard::Key restart{ sf::Keyboard::Backspace };
			sf::Keyboard::Key debug{ sf::Keyboard::Z };
			KeyPair moveX{ sf::Keyboard::A, sf::Keyboard::D };
			KeyPair moveY{ sf::Keyboard::W, sf::Keyboard::S };
		} keyboard;
	} bindings;

	struct {
		const Math::Vec2 size{ 8.0f, 12.0f };
		const Math::Color color{ 0.2f, 0.2f, 0.2f, 1.0f };

		vector< shared_ptr< RigidRect > > dashShadows;
	} spriteData;

	struct {
		bool enabled{ true };
		bool canMove{ true };

		float acceleration{ 22.0f };
		float maxSpeed{ 100.0f };
		float airMultiplier{ 0.7f };
	} movementData;

	struct {
		bool enabled{ true };

		float power{ 15.0f };
		float max{ 300.0f };
		float min{ 20.0f };
	} gravityData;

	struct {
		bool enabled{ true };

		float power{ 17.5f };
		float min{ 8.0f };
		float max{ 800.0f };
		float airMultiplier{ 0.05f };
	} frictionData;

	struct {
		bool enabled{ true };
		bool isJumping{ false };
		bool canJump{ false };
		bool canJumpDown{ false };
		bool isJumpingDown{ false };

		float power{ 160.0f };
		float release{ 70.0f };
	} jumpData;

	struct {
		bool enabled{ false };
		bool canDoubleJump{ true };

		float power{ 96.0f };
	} doubleJumpData;

	struct {
		bool enabled{ true };
		bool canAttack{ true };
		bool isAttacking{ false };
		bool isVisible{ false };

		float power{ 25.0f };
		float fallTransferMultiplier{ 0.5f };
		Math::Vec2 direction{ 0.0f, 0.0f };
		Math::Vec2 size{ 12.0f, 6.0f };
		Math::Vec2 min{ 150.0f, 180.0f };
		milliseconds duration{ 600 };
	} attackData;

	struct {
		bool enabled{ true };
		bool canDash{ true };
		bool isDashing{ false };

		float power{ 300.0f };
		Math::Vec2 release{ 32.0f, 32.0f };
		milliseconds cooldown{ 1000 };
		milliseconds duration{ 175 };
		
		milliseconds animationStep{ 25 };
	} dashData;

	struct {
		bool enabled{ true };
		bool canDashBounce{ false };

		float power{ 0.6f };
		Math::Vec2 direction{ 0.0f, 0.0f };
		float durationMultiplier{ 0.5f };
		milliseconds leniency{ 200 };
	} dashBounceData;

	struct {
		bool enabled{ true };
		bool isClinging{ false };
		
		float power{ 2.0f };
		float leniency{ 2.5f };
		float min{ 32.0f };
		float max{ 256.0f };
	} wallClingData;

	struct {
		bool enabled{ true };
		float normal{ 0.0f };

		float power{ 300.0f };
		Math::Vec2 direction{ 0.45f, 0.55f };
		milliseconds duration{ 500 };
	} wallJumpData;
};

//================================================================================

} // Game

//================================================================================