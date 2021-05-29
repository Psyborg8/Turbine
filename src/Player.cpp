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

void Player::onUpdate( sf::Time deltaTime ) {
	Debug::startTimer( "Player::Update" );

	// Ignore physics while dashing
	if( !dashData.isDashing ) {
		// Horizontal
		{
			float move = 0.0;
			// Movement
			if( abs( m_velocity.x ) <= movementData.maxSpeed ) {
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
			if( !move && m_velocity.x ) {
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
			m_velocity.y += gravityData.power * deltaTime.asSeconds();
			m_velocity.y = m_velocity.y > gravityData.max ? gravityData.max : m_velocity.y;

			// Jump Release
			if( !sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) )
				m_velocity.y = std::max( m_velocity.y, -jumpData.release );

			// Wall Cling
			if( wallClingData.isClinging ) {
				float friction = wallClingData.multiplier * abs( m_velocity.y ) * deltaTime.asSeconds();
				friction = std::clamp( friction, wallClingData.min * deltaTime.asSeconds(), wallClingData.max * deltaTime.asSeconds() );
				if( abs( m_velocity.y ) < friction )
					m_velocity.y = 0.0;
				else
					m_velocity.y -= friction * ( m_velocity.y / abs( m_velocity.y ) );
			}
		}
	}

	// Reset flags
	wallClingData.isClinging = false;
	jumpData.canJump = false;

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

	Debug::stopTimer( "Player::Update" );
}

//--------------------------------------------------------------------------------

void Player::onProcessCollisions()
{
	Debug::startTimer( "Player::Find Colliders" );
	vector< shared_ptr< Object > > targets;
	const vector< shared_ptr< Object > > walls = getObjects( System::getWorld(), "Wall" );
	const vector< shared_ptr< Object > > traps = getObjects( System::getWorld(), "Trap" );
	const vector< shared_ptr< Object > > platforms = getObjects( System::getWorld(), "Platform" );
	Debug::stopTimer( "Player::Find Colliders" );

	targets.insert( targets.end(), walls.begin(), walls.end() );
	targets.insert( targets.end(), traps.begin(), traps.end() );

	// Check if we're supposed to collide with platforms

	Debug::startTimer( "Player::Platform Collision" );
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
	Debug::stopTimer( "Player::Platform Collision" );

	Debug::startTimer( "Player::Resolve Collisions" );
	resolveCollisions( targets, true );
	Debug::stopTimer( "Player::Resolve Collisions" );
}

//--------------------------------------------------------------------------------

void Player::onDestroy() {

}

//--------------------------------------------------------------------------------

void Player::onEvent( sf::Event e ) {
	if( e.type == sf::Event::KeyPressed )
		switch( e.key.code ) {
		case sf::Keyboard::Space:
			jump();
			break;
		case sf::Keyboard::E:
			dash();
			break;
		default:
			break;
	}
	else if( e.type == sf::Event::KeyReleased )
		switch( e.key.code ) {
		case sf::Keyboard::Space:
			jumpData.wait = false;
			break;
		case sf::Keyboard::E:
			dashData.wait = false;
			break;
		default:
			break;
		}
}

//--------------------------------------------------------------------------------

void Player::onRender() {
	Debug::startTimer( "Player::Render" );

	for( shared_ptr< RigidRect > shadow : spriteData.dashShadows )
		System::getWindow()->draw( shadow->getRect() );

	System::getWindow()->draw( m_rect );

	Debug::stopTimer( "Player::Render" );
}

//--------------------------------------------------------------------------------

void Player::onCollision( Collision::CollisionResult result, shared_ptr< Object > target ) {
	// Check for Trap or Wall
	if( target->getName() == "Trap" )
		return kill();
	if( target->getName() == "Platform" )
		jumpData.canJumpDown = true;

	if( result.normal.y == -1.0 ) {
		jumpData.canJump = true;
		doubleJumpData.canDoubleJump = true;
	}

	if( result.normal.x ) {
		wallClingData.isClinging = true;
		wallJumpData.normal = result.normal.x;
	}

	Timers::triggerTimer( m_dashTimer );
}

//--------------------------------------------------------------------------------

void Player::jump()
{
	if( jumpData.wait )
		return;

	if( jumpData.canJump ) {
		if( jumpData.canJumpDown && sf::Keyboard::isKeyPressed( sf::Keyboard::S ) )
			jumpData.isJumpingDown = true;
		else {
			m_velocity.y = -jumpData.power;
			jumpData.canJump = false;
		}
	}
	else if( doubleJumpData.canDoubleJump ) {
		if( wallClingData.isClinging ) {
			m_velocity.x = wallJumpData.direction.x * wallJumpData.power * wallJumpData.normal;
			m_velocity.y = wallJumpData.direction.y * -wallJumpData.power;
		}
		else
			m_velocity.y = -doubleJumpData.power;

		doubleJumpData.canDoubleJump = false;
	}

	jumpData.wait = true;
}

//--------------------------------------------------------------------------------

void Player::dash() {
	if( dashData.wait )
		return;

	dashData.wait = true;

	if( !dashData.canDash )
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
	dashData.wait = true;
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

	System::getWorld()->reset();
}

//================================================================================

} // Game

//================================================================================
