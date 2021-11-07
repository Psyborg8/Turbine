//================================================================================

#include "player.h"

//--------------------------------------------------------------------------------

#include "mathtypes.h"
#include "system.h"
#include "timer.h"
#include "debug.h"

#include "particle.h"
#include "world.h"
#include "camera.h"

//================================================================================

namespace Game {

//================================================================================

Player::Player() : Player( Math::Vec2() ) {
	//
}

//--------------------------------------------------------------------------------

Player::Player( Math::Vec2 pos ) : RigidRect() {
	setName( "Player" );
	setPosition( pos );
	setSize( spriteData.size );
	setColor( spriteData.color );
	setCollisionType( CollisionType::Dynamic );
	setPriority( static_cast< int >( RenderPriority::Player ) );
	m_rect.setOutlineColor( sf::Color( 180u, 180u, 180u, 255u ) );
	m_rect.setOutlineThickness( 0.5f );
}

//--------------------------------------------------------------------------------

void Player::onSpawnChildren() {
	m_bottomCollider = make_shared< RigidRect >();
	m_bottomCollider->setCollisionType( CollisionType::Static );
	m_bottomCollider->setSize( Math::Vec2( getSize().x, jumpData.leniency ) );
	m_bottomCollider->setVelocity( Math::Vec2( 0.0f, 1.0f ) );

	m_leftCollider = make_shared< RigidRect >();
	m_leftCollider->setCollisionType( CollisionType::Static );
	m_leftCollider->setSize( Math::Vec2( wallClingData.leniency, getSize().y ) );
	m_leftCollider->setVelocity( Math::Vec2( -1.0f, 0.0f ) );

	m_rightCollider = make_shared< RigidRect >();
	m_rightCollider->setCollisionType( CollisionType::Static );
	m_rightCollider->setSize( Math::Vec2( wallClingData.leniency, getSize().y ) );
	m_rightCollider->setVelocity( Math::Vec2( 1.0f, 0.0 ) );

	m_attackCollider = make_shared< RigidRect >();
	m_attackCollider->setCollisionType( CollisionType::Static );
	m_attackCollider->setColor( Colors::CYAN );

	{
		m_wallAttackCollider = make_shared< RigidRect >();
		Math::Vec2 size;
		size.x = attackData.size.x * 2.0f + getSize().x;
		size.y = attackData.size.y;
		m_wallAttackCollider->setSize( size );
		m_wallAttackCollider->setVelocity( Math::Vec2( 0.1f, 0.0f ) );
		m_wallAttackCollider->setCollisionType( CollisionType::Static );
		m_wallAttackCollider->setColor( Math::Color( 1.0f, 1.0f, 1.0f, 0.5f ) );
	}

	using namespace std::placeholders;
	// Movement
	Input::bindButton( "Jump", bindings.controller.jump, bindings.keyboard.jump, bind( &Player::jump, this, _1 ) );
	Input::bindButton( "Attack", bindings.controller.attack, bindings.keyboard.attack, bind( &Player::attack, this, _1 ) );
	Input::bindAxis( "Move X", bindings.controller.moveX, bindings.keyboard.moveX, nullptr );
	Input::bindAxis( "Move Y", bindings.controller.moveY, bindings.keyboard.moveY, nullptr );

	// System
	Input::bindButton( "Reset", bindings.controller.reset, bindings.keyboard.reset, bind( &Player::kill, this, _1, false ) );
	Input::bindButton( "Restart", bindings.controller.restart, bindings.keyboard.restart, bind( &Player::kill, this, _1, true ) );

	// Debug
	Input::bindButton( "Debug", bindings.controller.debug, bindings.keyboard.debug, bind( &Player::debug, this, _1 ) );

	// Sounds
	soundData.buffers.jump.loadFromFile( Folders::Sound + "Jump.wav" );
	soundData.players.jump.setBuffer( soundData.buffers.jump );

	// Console Commands
	Debug::addCommand( "restart", std::bind( &Player::kill, this, true, true ) );
	Debug::addCommand( "respawn", std::bind( &Player::kill, this, true, false ) );

	Debug::addSetCommand( "player_movement_enabled", movementData.enabled );
	Debug::addSetCommand( "player_movement_canMove", movementData.canMove );
	Debug::addSetCommand( "player_movement_direction", movementData.direction );
	Debug::addSetCommand( "player_movement_acceleration", movementData.acceleration );
	Debug::addSetCommand( "player_movement_maxSpeed", movementData.maxSpeed );
	Debug::addSetCommand( "player_movement_airMultiplier", movementData.airMultiplier );

	Debug::addSetCommand( "player_gravity_enabled", gravityData.enabled );
	Debug::addSetCommand( "player_gravity_power", gravityData.power );
	Debug::addSetCommand( "player_gravity_max", gravityData.max );
	Debug::addSetCommand( "player_gravity_min", gravityData.min );

	Debug::addSetCommand( "player_friction_enabled", frictionData.enabled );
	Debug::addSetCommand( "player_friction_power", frictionData.power );
	Debug::addSetCommand( "player_friction_min", frictionData.min );
	Debug::addSetCommand( "player_friction_max", frictionData.max );
	Debug::addSetCommand( "player_friction_airMultiplier", frictionData.airMultiplier );

	Debug::addSetCommand( "player_jump_enabled", jumpData.enabled );
	Debug::addSetCommand( "player_jump_isJumping", jumpData.isJumping );
	Debug::addSetCommand( "player_jump_canJump", jumpData.canJump );
	Debug::addSetCommand( "player_jump_canJumpDown", jumpData.canJumpDown );
	Debug::addSetCommand( "player_jump_isJumpingDown", jumpData.isJumpingDown );
	Debug::addSetCommand( "player_jump_leniency", jumpData.leniency );
	Debug::addSetCommand( "player_jump_power", jumpData.power );
	Debug::addSetCommand( "player_jump_release", jumpData.release );

	Debug::addSetCommand( "player_doubleJump_enabled", doubleJumpData.enabled );
	Debug::addSetCommand( "player_doubleJump_canDoubleJump", doubleJumpData.canDoubleJump );
	Debug::addSetCommand( "player_doubleJump_power", doubleJumpData.power );

	Debug::addSetCommand( "player_attack_enabled", attackData.enabled );
	Debug::addSetCommand( "player_attack_canAttack", attackData.canAttack );
	Debug::addSetCommand( "player_attack_isAttacking", attackData.isAttacking );
	Debug::addSetCommand( "player_attack_power", attackData.power );
	Debug::addSetCommand( "player_attack_fallTransferMultiplier", attackData.fallTransferMultiplier );
	Debug::addSetCommand( "player_attack_direction", attackData.direction );
	Debug::addSetCommand( "player_attack_size", attackData.size );
	Debug::addSetCommand( "player_attack_min", attackData.min );
	Debug::addSetCommand( "player_attack_duration", attackData.duration );

	Debug::addSetCommand( "player_dash_enabled", dashData.enabled );
	Debug::addSetCommand( "player_dash_canDashBounce", dashData.canDash );
	Debug::addSetCommand( "player_dash_power", dashData.power );
	Debug::addSetCommand( "player_dash_release", dashData.release );
	Debug::addSetCommand( "player_dash_cooldown", dashData.cooldown );
	Debug::addSetCommand( "player_dash_duration", dashData.duration );
	Debug::addSetCommand( "player_dash_animationStep", dashData.animationStep );

	Debug::addSetCommand( "player_wallCling_enabled", wallClingData.enabled );
	Debug::addSetCommand( "player_wallCling_isClinging", wallClingData.isClinging );
	Debug::addSetCommand( "player_wallCling_power", wallClingData.power );
	Debug::addSetCommand( "player_wallCling_leniency", wallClingData.leniency );
	Debug::addSetCommand( "player_wallCling_min", wallClingData.min );
	Debug::addSetCommand( "player_wallCling_max", wallClingData.max );
}

//--------------------------------------------------------------------------------

void Player::onUpdate( sf::Time deltaTime ) {
	// Ignore physics while dashing
	if( !dashData.isDashing ) {
		// Horizontal
		{
			float move = 0.0f;
			float axis = Input::getAxisState( "Move X" );
			if( axis ) {
				axis = pow( axis / 100.0f, 2.0f ) * 100.0f * ( axis / abs( axis ) );
				move = axis * movementData.acceleration* deltaTime.asSeconds();
			}

			if( move )
				movementData.direction = move / abs( move );

			// Movement
			if( movementData.enabled && movementData.canMove && move &&
				!( m_velocity.abs().x > movementData.maxSpeed * ( abs( axis ) / 100.0f ) && m_velocity.x / abs( m_velocity.x ) == move / abs( move ) ) ) {

				// Air control
				if( !jumpData.canJump )
					move *= movementData.airMultiplier;

				// Apply movement
				m_velocity.x += move;
			}

			// Friction
			else if( m_velocity.x && frictionData.enabled ) {
				float friction = frictionData.power * abs( m_velocity.x ) * deltaTime.asSeconds();

				// Air resistance
				if( !jumpData.canJump )
					friction *= frictionData.airMultiplier;

					friction = std::clamp( friction, frictionData.min * deltaTime.asSeconds(), frictionData.max * deltaTime.asSeconds() );
					if( abs( m_velocity.x ) < friction )
						m_velocity.x = 0.0;
					else
						m_velocity.x -= friction * ( m_velocity.x / abs( m_velocity.x ) );
			}
		}

		// Vertical
		{
			// Gravity
			if( gravityData.enabled ) {
				float gravity = m_velocity.y < 0.0f ? m_velocity.y : 0.0f;
				gravity = gravity > gravityData.min ? gravity : gravityData.min;
				gravity = gravity * gravityData.power * deltaTime.asSeconds();
				m_velocity.y += gravity;
				m_velocity.y = m_velocity.y > gravityData.max ? gravityData.max : m_velocity.y;
			}

			// Jump Release
			if( jumpData.isJumping )
				if( !Input::getButtonState( "Jump" ) ) {
					m_velocity.y = std::max( m_velocity.y, -jumpData.release );
					jumpData.isJumping = false;
				}

			// Wall Cling
			if( wallClingData.isClinging && wallClingData.enabled ) {
				float friction = wallClingData.power * abs( m_velocity.y ) * deltaTime.asSeconds();
				friction = std::clamp( friction, wallClingData.min * deltaTime.asSeconds(), wallClingData.max * deltaTime.asSeconds() );
				if( abs( m_velocity.y ) < friction )
					m_velocity.y = 0.0;
				else
					m_velocity.y -= friction * ( m_velocity.y / abs( m_velocity.y ) );
			}
		}
	}
}

//--------------------------------------------------------------------------------

void Player::onProcessCollisions()
{
	// Find relevant colliders
	World* world = getWorld();
	if( world == nullptr )
		return;

	string mapName = world->getCurrentMap();
	const vector< shared_ptr< Object > > walls;
	const vector< shared_ptr< Object > > traps;
	const vector< shared_ptr< Object > > platforms;
	const vector< shared_ptr< Object > > checkpoints;

	vector< shared_ptr< Object > > targets;

	// Check if we're supposed to collide with platforms
	bool collision = false;
	for( shared_ptr< Object > platform : platforms ) {
		Collision::CollisionResult result = isColliding( platform );
		if( result.success ) {
			collision = true;
			break;
		}
	}

	jumpData.isJumpingDown &= collision;
	if( !jumpData.isJumpingDown )
		if( m_velocity.y < 0.0 )
			targets.insert( targets.end(), platforms.begin(), platforms.end() );
		else if( collision )
			jumpData.isJumpingDown = true;

	// Physics resolution
	targets.insert( targets.end(), walls.begin(), walls.end() );
	targets = sortObjectsByDistance( targets, getPosition(), 64.0f );
	resolveCollisions( targets, true );

	// Extended hitbox detection
	extendedHitbox( targets );

	// Checkpoint detection
	targets.clear();
	targets.insert( targets.end(), checkpoints.begin(), checkpoints.end() );
	targets.insert( targets.end(), traps.begin(), traps.end() );
	targets = sortObjectsByDistance( targets, getPosition(), 64.0f );
	processCollisions( targets );

	// Attack detection
	if( attackData.isAttacking ) {
		targets.clear();
		targets.insert( targets.end(), walls.begin(), walls.end() );
		targets = sortObjectsByDistance( targets, getPosition(), 64.0f );
		attackHitbox( targets );
	}
}

//--------------------------------------------------------------------------------

void Player::onDestroy() {

}

//--------------------------------------------------------------------------------

void Player::onEvent( sf::Event e ) {

}

//--------------------------------------------------------------------------------

void Player::onRender( sf::RenderTarget* target ) {
	m_attackCollider->render( target );
	m_wallAttackCollider->render( target );

	target->draw( m_rect );
	Debug::incDrawCall();
}

//--------------------------------------------------------------------------------

void Player::onCollision( Collision::CollisionResult result, shared_ptr< Object > target ) {
	// Checkpoints
	if( target->getName() == "Checkpoint" ) {
		shared_ptr< RigidRect > checkpoint = std::dynamic_pointer_cast< RigidRect >( target );
		Math::Vec2 position = checkpoint->getPosition();
		position.x += checkpoint->getSize().x / 2.0f;
		m_spawn = position;
		checkpoint->setCollisionType( CollisionType::None );
		System::getWorld()->onMessage( "Checkpoint" );
		return;
	}

	if( target->getName() == "Level End" ) {
		target->setCollisionType( CollisionType::None );
		System::getWorld()->onMessage( "Level End" );
		return;
	}

	// Traps
	if( target->getName() == "Trap" )
		return kill( true, false );

	// Platforms
	if( target->getName() == "Platform" )
		jumpData.canJumpDown = true;

	jumpData.isJumping = false;
}

//--------------------------------------------------------------------------------

void Player::jump( bool pressed )
{
	if( !pressed )
		return;

	// Dash Bounce
	if( dashBounceData.enabled && dashBounceData.canDashBounce ) {
		dashBounce();
		return;
	}

	// Jump
	if( jumpData.canJump && jumpData.enabled ) {
		if( jumpData.canJumpDown && sf::Keyboard::isKeyPressed( sf::Keyboard::S ) )
			jumpData.isJumpingDown = true;
		else {
			m_velocity.y = -jumpData.power;
			jumpData.canJump = false;
			jumpData.isJumping = true;
		}
		return;
	}

	if( dashData.isDashing )
		return;

	// Wall Jump
	if( wallClingData.isClinging && wallJumpData.enabled ) {
		m_velocity.x = wallJumpData.direction.x * wallJumpData.power * wallJumpData.normal;
		m_velocity.y = wallJumpData.direction.y * -wallJumpData.power;

		float acceleration = movementData.airMultiplier;
		float friction = frictionData.airMultiplier;

		Timers::addTimer( wallJumpData.duration,
						  [this, acceleration, friction]( float alpha ) {
							  movementData.airMultiplier = alpha * acceleration;
							  frictionData.airMultiplier = alpha * friction;
						  },
						  nullptr, false );
		return;
	}

	// Dash
	if( dashData.enabled && dashData.canDash ) {
		dash( true );
		return;
	}

	// Double Jump
	if( doubleJumpData.canDoubleJump && doubleJumpData.enabled ) {
		m_velocity.y = -doubleJumpData.power;
		doubleJumpData.canDoubleJump = false;
		return;
	}
}

//--------------------------------------------------------------------------------

void Player::dash( bool pressed ) {
	if( !pressed )
		return;
	if( dashBounceData.canDashBounce && dashBounceData.enabled ) {
		dashBounce();
		return;
	}
	if( !dashData.canDash || !dashData.enabled )
		return;

	Math::Vec2 direction;
	direction.x = Input::getAxisState( "Move X" );
	direction.y = Input::getAxisState( "Move Y" );

	if( direction == Math::Vec2() )
		return;

	System::getWorld()->message( "Dash" );

	direction = direction.normalize();
	direction *= dashData.power;

	setVelocity( direction );

	dashData.canDash = false;
	dashData.isDashing = true;
	doubleJumpData.canDoubleJump = true;

	m_dashCooldownTimer = Timers::addTimer( dashData.cooldown, nullptr, [this] { dashData.canDash = true; }, false );
	m_dashTimer = Timers::addTimer( dashData.duration,
									nullptr,
									[this, direction] {
										dashData.isDashing = false;
										m_velocity = dashData.release * direction.normalize();
										Timers::removeTimer( m_dashAnimationTimer );
									}, false );



	m_dashAnimationTimer = Timers::addTimer( dashData.animationStep,
											 nullptr,
											 [this] {
												 shared_ptr< RigidRect > rect = makeObject< RigidRect >( this );
												 rect->setRect( m_rect );
												 sf::RectangleShape& r = rect->getRect();
												 {
													 sf::Color color = r.getFillColor();
													 color.a = sf::Uint8( color.a * 0.9f );
													 r.setFillColor( color );
												 }
												 {
													 sf::Color color = r.getOutlineColor();
													 color.a =  sf::Uint8( color.a * 0.75f );
													 r.setOutlineColor( color );
												 }

												 rect->setVisibility( true );

												 spriteData.dashShadows.push_back( rect );

												 Timers::addTimer( 100, 
																   [this, rect]( float alpha ) {
																	   if( this == nullptr )
																		   return;
																	   if( spriteData.dashShadows.empty() )
																		   return;
																	   if( spriteData.dashShadows.size() > 255u )
																		   return;

																	   const auto it = std::find( spriteData.dashShadows.begin(),
																								  spriteData.dashShadows.end(),
																								  rect );
																	   if( it == spriteData.dashShadows.end() )
																		   return;

																		sf::Color color = ( *it )->getRect().getFillColor();
																		sf::Color outline = ( *it )->getRect().getOutlineColor();
																		color.a = sf::Uint8( ( 0.6f - alpha * 0.6 )  * 255.0f );
																		outline.a = sf::Uint8( ( 0.6f - alpha * 0.6 ) * 255.0f );
																		( *it )->setColor( color );
																		( *it )->getRect().setOutlineColor( outline );
																   },
																   [this, rect] {
																	   if( this == nullptr )
																		   return;
																	   if( spriteData.dashShadows.empty() )
																		   return;
																	   if( spriteData.dashShadows.size() > 255u )
																		   return;

																	   const auto it = std::find( spriteData.dashShadows.begin(),
																								  spriteData.dashShadows.end(),
																								  rect );
																	   if( it != spriteData.dashShadows.end() ) {
																		   ( *it )->destroy();
																		   spriteData.dashShadows.erase( it );
																	   }
																  }, false );
											}, true );

}

//--------------------------------------------------------------------------------

void Player::dashBounce() {
	if( dashBounceData.direction.length() == 0.0f )
		return;

	if( abs( dashBounceData.direction.normalize().y ) < abs( wallJumpData.direction.y ) ) {
		Math::Vec2 v = wallJumpData.direction * dashBounceData.direction.length();
		v.x *= dashBounceData.direction.x / ( abs( dashBounceData.direction.x ) );
		v.y *= -1.0f;
		m_velocity = v;
	}
	else {
		m_velocity = dashBounceData.direction;
	}

	m_velocity *= dashBounceData.power;

	dashBounceData.canDashBounce = false;
	dashBounceData.direction = Math::Vec2();

	dashData.isDashing = false;
	Timers::removeTimer( m_dashTimer );
	Timers::removeTimer( m_dashAnimationTimer ); 

	float acceleration = movementData.airMultiplier;
	float friction = frictionData.airMultiplier;
	Timers::addTimer( wallJumpData.duration,
					  [this, acceleration, friction]( float alpha ) {
						  movementData.airMultiplier = alpha * acceleration;
						  frictionData.airMultiplier = alpha * friction;
					  },
					  nullptr, false );
}

//--------------------------------------------------------------------------------

void Player::attack( bool pressed ) {
	if( !pressed )
		return;
	if( !attackData.enabled || !attackData.canAttack )
		return;

	// Get direction
	attackData.direction.x = Input::getAxisState( "Move X" );
	attackData.direction.y = Input::getAxisState( "Move Y" );

	// Find direction
	if( attackData.direction.x && attackData.direction.y ) {
		if( attackData.direction.x >= attackData.direction.y )
			attackData.direction.y = 0.0f;
		else
			attackData.direction.x = 0.0f;
	}

	// Neutral attack
	if( !attackData.direction.length() )
		// While on the ground we use the direction the player is facing
		if( jumpData.canJump )
			attackData.direction = Math::Vec2( movementData.direction, 0.0f );

	// While in the air, we need to look for walls to hit in attackHitbox

	attackData.isAttacking = true;
	m_attackCollider->setVisibility( true );

	Timers::removeTimer( m_attackRenderTimer );
	m_attackRenderTimer = Timers::addTimer( 34, nullptr, [this] { m_attackCollider->setVisibility( false ); }, false );
}

//--------------------------------------------------------------------------------

void Player::kill( bool pressed, bool restart ) {
	if( !pressed )
		return;
	if( restart )
		System::getWorld()->onMessage( "Player Restart" );

	Timers::removeTimer( m_dashAnimationTimer );
	Timers::removeTimer( m_dashCooldownTimer );
	Timers::removeTimer( m_dashTimer );
	Timers::removeTimer( m_attackTimer );
	Timers::removeTimer( m_attackRenderTimer );

	spriteData.dashShadows.clear();

	System::getWorld()->reset();
}

//--------------------------------------------------------------------------------

void Player::debug( bool pressed ) {
	if( !pressed )
		return;

	System::getWorld()->onMessage( "Flip Debug Page" );
}

//--------------------------------------------------------------------------------

void Player::extendedHitbox( vector< shared_ptr< Object > > targets ) {

	m_bottomCollider->setPosition( Math::Vec2( getPosition().x, getPosition().y + getSize().y ) );
	m_leftCollider->setPosition( Math::Vec2( getPosition().x - wallClingData.leniency, getPosition().y ) );
	m_rightCollider->setPosition( Math::Vec2( getPosition().x + getSize().x, getPosition().y ) );

	jumpData.canJump = false;
	wallClingData.isClinging = false;

	// Bottom
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = m_bottomCollider->isColliding( target );

		if( result.success ) {
			jumpData.canJump = true;
			doubleJumpData.canDoubleJump = true;
			return;
		}
	}

	// Left
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = m_leftCollider->isColliding( target );

		if( result.success ) {
			wallClingData.isClinging = true;
			wallJumpData.normal = 1.0f;
			return;
		}
	}

	// Right
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = m_rightCollider->isColliding( target );

		if( result.success ) {
			wallClingData.isClinging = true;
			wallJumpData.normal = -1.0f;
			return;
		}
	}
}

//--------------------------------------------------------------------------------

void Player::attackHitbox( vector< shared_ptr< Object > > targets, bool firstPass ) {
	attackData.isAttacking = false;

	// Look for walls to hit if the attack was neutral and we're in the air
	if( !attackData.direction.length() ) {
		Math::Vec2 position;
		position.x = getPosition().x - attackData.size.x;
		position.y = getPosition().y + ( getSize().y / 2.0f ) - ( m_wallAttackCollider->getSize().y / 2.0f );
		m_wallAttackCollider->setPosition( position );

		for( shared_ptr< Object > target : targets ) {
			const Collision::CollisionResult result = m_wallAttackCollider->isColliding( target );

			if( result.success ) {
				attackData.direction.x = result.point.x - getPosition().x;
				attackData.direction.x /= abs( attackData.direction.x );
				break;
			}
		}
	}

	// A neutral attack in the air, if there's no valid targets, 
	// should attack in the direction the character is facing
	if( !attackData.direction.length() )
		attackData.direction.x = movementData.direction;

	// Calculate size from direction
	Math::Vec2 size;
	size.x = attackData.direction.x ? attackData.size.x : attackData.size.y;
	size.y = attackData.direction.y ? attackData.size.x : attackData.size.y;

	// Calculate position from direction and size
	Math::Vec2 position;
	if( attackData.direction.x ) {
		if( attackData.direction.x > 0.0f )
			position.x = getPosition().x + getSize().x;
		else
			position.x = getPosition().x - size.x;

		position.y = getPosition().y + ( getSize().y / 2.0f ) - ( size.y / 2.0f );
	}
	if( attackData.direction.y ) {
		if( attackData.direction.y > 0.0f )
			position.y = getPosition().y + getSize().y;
		else
			position.y = getPosition().y - size.y;

		position.x = getPosition().x + ( getSize().x / 2.0f ) - ( size.x / 2.0f );
	}

	// Update collider
	m_attackCollider->setPosition( position );
	m_attackCollider->setSize( size );
	m_attackCollider->setCollisionType( CollisionType::Static );
	m_attackCollider->setVelocity( attackData.direction.normalize() * ( getVelocity().length() + 1.0f ) );

	bool collision = false;
	for( shared_ptr< Object > target : targets ) {
		const Collision::CollisionResult result = m_attackCollider->isColliding( target );

		if( result.success ) {
			collision = true;
			break;
		}
	}

	// If we're attempting a vertical attack and there's nothing to hit, check the sides instead
	if( !collision ) {
		if( firstPass ) {
			attackData.direction = Math::Vec2();
			attackHitbox( targets, false );
		}
		
		return;
	}

	// Horizontal Attack
	if( attackData.direction.x ) {
		// Carry over existing X momentum
		m_velocity.x *= -1.0f; 

		// Raise to the minimum if necessary
		if( abs( m_velocity.x ) < attackData.min.x )
			m_velocity.x = attackData.min.x * -attackData.direction.normalize().x;

		// If the player is falling, use that momentum too
		if( m_velocity.y > 0.0f )
			m_velocity.x += m_velocity.y * attackData.fallTransferMultiplier * -attackData.direction.normalize().x;

		// Add some height too
		m_velocity.y -= attackData.power;
		if( m_velocity.y > -( attackData.min.y / 1.75f ) )
			m_velocity.y = -( attackData.min.y / 1.75f );

		// LERP air movement from 0 back up to the original. So the player can't halt their momentum immediately
		Timers::triggerTimer( m_attackTimer );
		float acceleration = movementData.airMultiplier;
		float friction = frictionData.airMultiplier;
		m_attackTimer = Timers::addTimer( attackData.duration,
						  [this, acceleration, friction]( float alpha ) {
							  movementData.airMultiplier = alpha * acceleration;
							  frictionData.airMultiplier = alpha * friction;
						  },
						  nullptr, false );
	}
	// Vertical attack
	else {
		// Carry over existing Y momentum
		m_velocity.y *= -1.0f;

		// Raise to the minimum if necessary
		if( abs( m_velocity.y ) < attackData.min.y )
			m_velocity.y = attackData.min.y * -attackData.direction.normalize().y;

		// If we already have a minimum of X momentum, add some more
		if( abs( m_velocity.x ) > attackData.min.x / 10.0f ) {
			m_velocity.x += ( attackData.power / 4.0f ) * ( m_velocity.x / abs( m_velocity.x ) );
			if( abs( m_velocity.x ) < attackData.min.y / 3.0f )
				m_velocity.x = attackData.min.x / 3.0f * ( m_velocity.x / abs( m_velocity.x ) );
		}
	}

	Gfx::CameraShake shake;
	shake.distance = 8.f;
	shake.intensity = 5.f;
	shake.duration = 250ms;

	getWorld()->getCamera().shake( shake );

	jumpData.isJumping = false;
	dashData.isDashing = false;
	dashData.canDash = true;
	Timers::removeTimer( m_dashTimer );
	Timers::removeTimer( m_dashCooldownTimer );
	Timers::removeTimer( m_dashAnimationTimer );
}

//================================================================================

} // Game

//================================================================================
