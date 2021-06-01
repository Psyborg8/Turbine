//================================================================================

#include "Player.h"

//--------------------------------------------------------------------------------

#include "MathTypes.h"
#include "System.h"
#include "World.h"
#include "Timer.h"
#include "Debug.h"

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
	m_rect.setOutlineColor( sf::Color( 180u, 180u, 180u, 255u ) );
	m_rect.setOutlineThickness( 0.5f );
}

//--------------------------------------------------------------------------------

void Player::onSpawnChildren() {
	m_bottomCollider= make_shared< RigidRect >();
	m_bottomCollider->setCollisionType( CollisionType::Static );
	m_bottomCollider->setSize( Math::Vec2( getSize().x, wallClingData.leniency ) );
	m_bottomCollider->setColor( sf::Color::Red );
	m_bottomCollider->setVelocity( Math::Vec2( 0.0f, 1.0f ) );

	m_leftCollider = make_shared< RigidRect >();
	m_leftCollider->setCollisionType( CollisionType::Static );
	m_leftCollider->setSize( Math::Vec2( wallClingData.leniency, getSize().y ) );
	m_leftCollider->setColor( sf::Color::Red );
	m_leftCollider->setVelocity( Math::Vec2( -1.0f, 0.0f ) );

	m_rightCollider = make_shared< RigidRect >();
	m_rightCollider->setCollisionType( CollisionType::Static );
	m_rightCollider->setSize( Math::Vec2( wallClingData.leniency, getSize().y ) );
	m_rightCollider->setColor( sf::Color::Red );
	m_rightCollider->setVelocity( Math::Vec2( 1.0f, 0.0 ) );
}

//--------------------------------------------------------------------------------

void Player::onUpdate( sf::Time deltaTime ) {

	Debug::startTimer( "Player::Update Physics" );
	// Ignore physics while dashing
	if( !dashData.isDashing ) {
		// Horizontal
		{
			float move = 0.0;
			// Movement
			if( abs( m_velocity.x ) <= movementData.maxSpeed && movementData.enabled ) {
				// Input
				if( sf::Keyboard::isKeyPressed( sf::Keyboard::D ) )
					move += movementData.acceleration * deltaTime.asSeconds();
				if( sf::Keyboard::isKeyPressed( sf::Keyboard::A ) )
					move -= movementData.acceleration * deltaTime.asSeconds();

				// Air control
				if( !jumpData.canJump )
					move *= movementData.airMultiplier;

				// Apply movement
				if( abs( m_velocity.x + move ) > movementData.maxSpeed &&
					m_velocity.x / abs( m_velocity.x ) == move / abs( move ) )
					m_velocity.x = movementData.maxSpeed * ( move / abs( move ) );
				else
					m_velocity.x += move;
			}

			// Friction
			if( !move && m_velocity.x && frictionData.enabled ) {
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
				m_velocity.y += gravityData.power * deltaTime.asSeconds();
				m_velocity.y = m_velocity.y > gravityData.max ? gravityData.max : m_velocity.y;
			}

			// Jump Release
			if( !sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) )
				m_velocity.y = std::max( m_velocity.y, -jumpData.release );

			// Wall Cling
			if( wallClingData.isClinging && wallClingData.enabled ) {
				float friction = wallClingData.multiplier * abs( m_velocity.y ) * deltaTime.asSeconds();
				friction = std::clamp( friction, wallClingData.min * deltaTime.asSeconds(), wallClingData.max * deltaTime.asSeconds() );
				if( abs( m_velocity.y ) < friction )
					m_velocity.y = 0.0;
				else
					m_velocity.y -= friction * ( m_velocity.y / abs( m_velocity.y ) );
			}
		}
	}
	Debug::stopTimer( "Player::Update Physics" );

	Debug::startTimer( "Player::Update Animations" );
	// Animations
	for( shared_ptr< RigidRect > shadow : spriteData.dashShadows ) {
		sf::RectangleShape& rect = shadow->getRect();
		{
			sf::Color color = rect.getFillColor();
			color.a = sf::Uint8( color.a * 0.8f );
			rect.setFillColor( color );
		}
		{
			sf::Color color = rect.getOutlineColor();
			color.a = sf::Uint8( color.a * 0.8f );
			rect.setOutlineColor( color );
		}
		
	}
	Debug::stopTimer( "Player::Update Animations" );
}

//--------------------------------------------------------------------------------

void Player::onProcessCollisions()
{
	Debug::startTimer( "Player::Find Colliders" );
	const vector< shared_ptr< Object > > walls = getObjects( System::getWorld(), "Wall" );
	const vector< shared_ptr< Object > > traps = getObjects( System::getWorld(), "Trap" );
	const vector< shared_ptr< Object > > platforms = getObjects( System::getWorld(), "Platform" );
	const vector< shared_ptr< Object > > checkpoints = getObjects( System::getWorld(), "Checkpoint" );
	const vector< shared_ptr< Object > > levelEnds = getObjects( System::getWorld(), "Level End" );
	Debug::stopTimer( "Player::Find Colliders" );

	vector< shared_ptr< Object > > targets;

	// Check if we're supposed to collide with platforms
	Debug::startTimer( "Player::Platform Detection" );
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
	Debug::stopTimer( "Player::Platform Detection" );

	Debug::startTimer( "Player::Resolve Collisions" );
	targets.insert( targets.end(), walls.begin(), walls.end() );
	resolveCollisions( targets, true );
	Debug::stopTimer( "Player::Resolve Collisions" );

	// Extended hitbox detection
	Debug::startTimer( "Player::Extended Hitbox Detection" );
	extendedHitbox( targets );
	Debug::stopTimer( "Player::Extended Hitbox Detection" );

	Debug::startTimer( "Player::Checkpoint Detection" );
	targets.clear();
	targets.insert( targets.end(), checkpoints.begin(), checkpoints.end() );
	targets.insert( targets.end(), levelEnds.begin(), levelEnds.end() );
	targets.insert( targets.end(), traps.begin(), traps.end() );
	processCollisions( targets );
	Debug::stopTimer( "Player::Checkpoint Detection" );
}

//--------------------------------------------------------------------------------

void Player::onDestroy() {

}

//--------------------------------------------------------------------------------

void Player::onEvent( sf::Event e ) {
	if( e.type == sf::Event::KeyPressed )
		if( e.key.code == sf::Keyboard::Space )
				jump();
		else if( e.key.code == sf::Keyboard::E )
				dash();
}

//--------------------------------------------------------------------------------

void Player::onRender() {
	Debug::startTimer( "Player::Render" );

	for( shared_ptr< RigidRect > shadow : spriteData.dashShadows )
		System::getWindow()->draw( shadow->getRect() );

	System::getWindow()->draw( m_bottomCollider->getRect() );
	System::getWindow()->draw( m_leftCollider->getRect() );
	System::getWindow()->draw( m_rightCollider->getRect() );
	System::getWindow()->draw( m_rect );

	Debug::stopTimer( "Player::Render" );
}

//--------------------------------------------------------------------------------

void Player::onCollision( Collision::CollisionResult result, shared_ptr< Object > target ) {
	// Checkpoints
	if( target->getName() == "Checkpoint" ) {
		shared_ptr< RigidRect > checkpoint = std::dynamic_pointer_cast< RigidRect >( target );
		Math::Vec2 position = checkpoint->getPosition();
		position.x += checkpoint->getSize().x / 2.0f;
		m_spawn = position;
		checkpoint->setParent( nullptr );
		System::getWorld()->onMessage( "Checkpoint" );
		return;
	}

	if( target->getName() == "Level End" ) {
		target->setParent( nullptr );
		System::getWorld()->onMessage( "Level End" );
		return;
	}

	// Traps
	if( target->getName() == "Trap" )
		return kill();

	// Platforms
	if( target->getName() == "Platform" )
		jumpData.canJumpDown = true;

	Timers::triggerTimer( m_dashTimer );
}

//--------------------------------------------------------------------------------

void Player::jump()
{
	// Jump
	if( jumpData.canJump && jumpData.enabled ) {
		if( jumpData.canJumpDown && sf::Keyboard::isKeyPressed( sf::Keyboard::S ) )
			jumpData.isJumpingDown = true;
		else {
			m_velocity.y = -jumpData.power;
			jumpData.canJump = false;
		}
	}

	// Wall Jump
	else if( wallClingData.isClinging && wallJumpData.enabled ) {
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
	}
	// Dash
	else if( dashData.enabled && dashData.canDash )
		dash();
	// Double Jump
	else if( doubleJumpData.canDoubleJump && doubleJumpData.enabled ) {
		m_velocity.y = -doubleJumpData.power;
		doubleJumpData.canDoubleJump = false;
	}
}

//--------------------------------------------------------------------------------

void Player::dash() {
	if( !dashData.canDash || !dashData.enabled )
		return;

	Math::Vec2 direction;
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::D ) )
		direction.x += 1.0;
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::A ) )
		direction.x -= 1.0;
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::W ) )
		direction.y -= 1.0;
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::S ) )
		direction.y += 1.0;

	if( direction == Math::Vec2() )
		return;

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

												 spriteData.dashShadows.push_back( rect );

												 Timers::addTimer( 500, nullptr,
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
																	   if( it != spriteData.dashShadows.end() )
																		   spriteData.dashShadows.erase( it );
																  }, false );
											}, true );

}

//--------------------------------------------------------------------------------

void Player::kill() {
	Timers::removeTimer( m_dashAnimationTimer );
	Timers::removeTimer( m_dashCooldownTimer );
	Timers::removeTimer( m_dashTimer );

	spriteData.dashShadows.clear();

	System::getWorld()->reset();
}

//--------------------------------------------------------------------------------

void Player::extendedHitbox( vector< shared_ptr< Object > > targets ) {

	m_bottomCollider->setPosition( Math::Vec2( getPosition().x, getPosition().y + getSize().y ) );
	m_leftCollider->setPosition( Math::Vec2( getPosition().x - wallClingData.leniency, getPosition().y ) );
	m_rightCollider->setPosition( Math::Vec2( getPosition().x + getSize().x, getPosition().y ) );

	jumpData.canJump = false;
	wallClingData.isClinging = false;

	m_bottomCollider->setColor( sf::Color::Red );
	m_leftCollider->setColor( sf::Color::Red );
	m_rightCollider->setColor( sf::Color::Red );

	// Bottom
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = m_bottomCollider->isColliding( target );

		if( result.success ) {
			jumpData.canJump = true;
			doubleJumpData.canDoubleJump = true;
			m_bottomCollider->setColor( sf::Color::Green );
			return;
		}
	}

	// Left
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = m_leftCollider->isColliding( target );

		if( result.success ) {
			wallClingData.isClinging = true;
			wallJumpData.normal = 1.0f;
			m_leftCollider->setColor( sf::Color::Green );
			return;
		}
	}

	// Right
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = m_rightCollider->isColliding( target );

		if( result.success ) {
			wallClingData.isClinging = true;
			wallJumpData.normal = -1.0f;
			m_rightCollider->setColor( sf::Color::Green );
			return;
		}
	}
}

//================================================================================

} // Game

//================================================================================
