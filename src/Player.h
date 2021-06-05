//================================================================================

#pragma once

//================================================================================

#include "RigidRect.h"
#include "Timer.h"
#include "PlayerController.h"

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
	void kill( bool pressed, bool restart );
	void debug( bool pressed );

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

	shared_ptr< Input::PlayerController > m_controller;

	// Timers
private:
	Timers::TimerID m_dashCooldownTimer;
	Timers::TimerID m_dashAnimationTimer;
	Timers::TimerID m_dashTimer;

	// Attributes
public:
	struct {
		struct {
			ControllerButton jump{ ControllerButton::Bottom };
			ControllerButton dash{ ControllerButton::Left };
			ControllerButton reset{ ControllerButton::Start };
			ControllerButton restart{ ControllerButton::Select };
			ControllerButton debug{ ControllerButton::Logo };
			ControllerAxis moveX{ ControllerAxis::LeftStickX };
			ControllerAxis moveY{ ControllerAxis::LeftStickY };
		} controller;

		struct {
			sf::Keyboard::Key jump{ sf::Keyboard::Space };
			sf::Keyboard::Key dash{ sf::Keyboard::E };
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
		float acceleration{ 9.0f };
		float maxSpeed{ 85.0f };
		float airMultiplier{ 0.85f };
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

		float power{ 245.0f };
		Math::Vec2 direction{ 0.55f, 0.45f };
		milliseconds duration{ 550 };
	} wallJumpData;
};

//================================================================================

} // Game

//================================================================================