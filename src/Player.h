//================================================================================

#pragma once

//================================================================================

#include "RigidRect.h"
#include "Timer.h"
#include "Input.h"
#include "Particle.h"

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
	void attackHitbox( vector< shared_ptr< Object > > targets, bool firstPass = true );

	// Variables
private:
	Math::Vec2 m_spawn;
	shared_ptr< RigidRect > m_bottomCollider;
	shared_ptr< RigidRect > m_leftCollider;
	shared_ptr< RigidRect > m_rightCollider;
	shared_ptr< RigidRect > m_extendedCollider;

	shared_ptr< RigidRect > m_attackCollider;
	shared_ptr< RigidRect > m_wallAttackCollider;

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
	// Bindings
	struct {
		// Controller
		struct {
			ControllerButton jump{ ControllerButton::Bottom };
			ControllerButton attack{ ControllerButton::RTrigger };
			ControllerButton reset{ ControllerButton::Start };
			ControllerButton restart{ ControllerButton::Select };
			ControllerButton debug{ ControllerButton::Logo };
			ControllerAxis moveX{ ControllerAxis::LeftStickX };
			ControllerAxis moveY{ ControllerAxis::LeftStickY };
		} controller;

		// Keyboard
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

	// Sprite
	struct {
		const Math::Vec2 size{ 8.0f, 12.0f };
		const Math::Color color{ 0.2f, 0.2f, 0.2f, 1.0f };

		vector< shared_ptr< RigidRect > > dashShadows;

		// Particles
		struct {
			Gfx::Particle::Pattern jump;
			Gfx::Particle::Pattern wallSlash;
		} particlePatterns;
	} spriteData;

	// Sound
	struct {
		// Buffers
		struct {
			sf::SoundBuffer jump;
		} buffers;

		// Players
		struct {
			sf::Sound jump;
		} players;
	} soundData;

	// Movement
	struct {
		bool enabled{ true };
		bool canMove{ true };

		float direction{ 1.0f };
		float acceleration{ 22.0f };
		float maxSpeed{ 100.0f };
		float airMultiplier{ 0.7f };
	} movementData;

	// Gravity
	struct {
		bool enabled{ true };

		float power{ 25.0f };
		float max{ 300.0f };
		float min{ 20.0f };
	} gravityData;

	// Friction
	struct {
		bool enabled{ true };

		float power{ 17.5f };
		float min{ 8.0f };
		float max{ 800.0f };
		float airMultiplier{ 0.05f };
	} frictionData;

	// Jump
	struct {
		bool enabled{ true };
		bool isJumping{ false };
		bool canJump{ false };
		bool canJumpDown{ false };
		bool isJumpingDown{ false };

		float leniency{ 1.5f };
		float power{ 200.0f };
		float release{ 70.0f };
	} jumpData;

	// Double Jump
	struct {
		bool enabled{ false };
		bool canDoubleJump{ true };

		float power{ 96.0f };
	} doubleJumpData;

	// Attack
	struct {
		bool enabled{ true };
		bool canAttack{ true };
		bool isAttacking{ false };

		float power{ 25.0f };
		float fallTransferMultiplier{ 0.3f };
		Math::Vec2 direction{ 0.0f, 0.0f };
		Math::Vec2 size{ 16.0f, 6.0f };
		Math::Vec2 min{ 150.0f, 180.0f };
		milliseconds duration{ 600 };
	} attackData;

	// Dash
	struct {
		bool enabled{ true };
		bool canDash{ true };
		bool isDashing{ false };

		float power{ 250.0f };
		Math::Vec2 release{ 32.0f, 32.0f };
		milliseconds cooldown{ 1000 };
		milliseconds duration{ 200 };
		
		milliseconds animationStep{ 25 };
	} dashData;

	// Dash Bounce
	struct {
		bool enabled{ true };
		bool canDashBounce{ false };

		float power{ 0.6f };
		Math::Vec2 direction{ 0.0f, 0.0f };
		float durationMultiplier{ 0.5f };
		milliseconds leniency{ 200 };
	} dashBounceData;

	// Wall Cling
	struct {
		bool enabled{ true };
		bool isClinging{ false };
		
		float power{ 2.0f };
		float leniency{ 1.5f };
		float min{ 32.0f };
		float max{ 128.0f };
	} wallClingData;

	// Wall Jump
	struct {
		bool enabled{ false };
		float normal{ 0.0f };

		float power{ 300.0f };
		Math::Vec2 direction{ 0.45f, 0.55f };
		milliseconds duration{ 500 };
	} wallJumpData;
};

//================================================================================

} // Game

//================================================================================