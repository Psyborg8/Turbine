//================================================================================

#include "Player.h"

//--------------------------------------------------------------------------------

#include "MathTypes.h"
#include "System.h"
#include "World.h"
#include "Timer.h"
#include "Line.h"

//================================================================================

Player::Player() : Player( Vec2() )
{
	//
}

//--------------------------------------------------------------------------------

Player::Player( Vec2 pos ) : RigidRect( pos, Vec2( 0.15, 0.2 ), Colors::BLUE )
{
	setName( "Player" );
}

//--------------------------------------------------------------------------------

void Player::onUpdate( double deltaTime )
{
	Vec2 gravity = System::getWorld()->getGravity();
	bool didMove = false;

	// Ignore physics while dashing
	if( !m_isDashing )
	{
		// Movement
		if( abs( velocity.x ) <= m_maxMoveSpeed )
		{
			// Input
			double move = 0.0;
			if( System::getKeyState( KeyCode::d ) )
				move += m_moveAcceleration * deltaTime;
			if( System::getKeyState( KeyCode::a ) )
				move -= m_moveAcceleration * deltaTime;

			// Air control
			if( !m_canJump )
				move *= m_airMoveMultiplier;

			// Apply movement
			if( abs( velocity.x ) + move > m_maxMoveSpeed )
				velocity.x = m_maxMoveSpeed * ( move / abs( move ) );
			else
				velocity.x += move;

			// Disable friction
			if( move )
				didMove = true;
		}

		// Friction
		if( velocity.x && !didMove )
		{
			double friction = m_frictionMultiplier * abs( velocity.x ) * deltaTime;
			friction = std::clamp( friction, m_minFriction * deltaTime, m_maxFriction * deltaTime );
			if( abs( velocity.x ) < friction )
				velocity.x = 0.0;
			else
				velocity.x -= friction * ( velocity.x / abs( velocity.x ) );
		}

		// Gravity
		velocity += gravity * deltaTime;
		velocity.y = velocity.y < -m_terminalVelocity ? -m_terminalVelocity : velocity.y;

		// Jump Release
		if( !System::getKeyState( KeyCode::Space ) )
			velocity.y = std::min( velocity.y, m_jumpReleaseThreshold );

		// Wall Cling
		if( m_isWallCling )
		{
			double friction = m_wallFrictionMultiplier * abs( velocity.y ) * deltaTime;
			friction = std::clamp( friction, m_minWallFriction * deltaTime, m_maxWallFriction * deltaTime );
			if( abs( velocity.y ) < friction )
				velocity.y = 0.0;
			else
				velocity.y -= friction * ( velocity.y / abs( velocity.y ) );
		}
	}

	m_isWallCling = false;
	m_canJump = false;

	// Walls and Traps
	{
		vector< shared_ptr< Object > > targets;
		const vector< shared_ptr< Object > > walls = getObjectsByName( "Wall" );
		const vector< shared_ptr< Object > > traps = getObjectsByName( "Trap" );
		const vector< shared_ptr< Object > > platforms = getObjectsByName( "Platform" );

		targets.insert( targets.end(), walls.begin(), walls.end() );
		targets.insert( targets.end(), traps.begin(), traps.end() );

		bool collision = false;
		for( shared_ptr< Object > platform : platforms )
		{
			Collision::CollisionResult result = isColliding( platform );
			if( result.success )
			{
				collision = true;
				break;
			}
		}

		m_jumpingDown &= collision;

		if( !m_jumpingDown )
			if( velocity.y < 0.0 )
				targets.insert( targets.end(), platforms.begin(), platforms.end() );

		resolveCollisions( targets, true );
	}

	position += velocity * deltaTime;
}

//--------------------------------------------------------------------------------

void Player::onRender()
{
	RigidRect::render();
}

//--------------------------------------------------------------------------------

void Player::onDestroy()
{
	Observer::removeObserver( m_keyPressObserver );
	Observer::removeObserver( m_keyReleaseObserver );
}

//--------------------------------------------------------------------------------

void Player::onCreateObservers()
{
	using namespace std::placeholders;

	m_keyPressObserver = Observer::addObserver( ObserverType::KeyPress, KeyCallback( std::bind( &Player::onKeyboardPress, this, _1 ) ) );
	m_keyReleaseObserver = Observer::addObserver( ObserverType::KeyRelease, KeyCallback( std::bind( &Player::onKeyboardRelease, this, _1 ) ) );
}

//--------------------------------------------------------------------------------

void Player::onKeyboardPress( int key )
{
	const KeyCode keyCode = static_cast< KeyCode >( key );
	if( keyCode == KeyCode::Space )
		jump();
	else if( keyCode == KeyCode::e )
		dash();
}

//--------------------------------------------------------------------------------

void Player::onKeyboardRelease( int key )
{
	const KeyCode keyCode = static_cast< KeyCode >( key );
	if( keyCode == KeyCode::Space )
		m_waitJump = false;
	else if( keyCode == KeyCode::e )
		m_waitDash = false;
}

//--------------------------------------------------------------------------------

void Player::onCollision( Collision::CollisionResult result, shared_ptr< Object > target )
{
	// Check for Trap or Wall
	if( target->getName() == "Trap" )
		return kill();
	if( target->getName() == "Platform" )
		m_canJumpDown = true;

	if( result.normal.y == 1.0 )
	{
		m_canJump = true;
		m_canDoubleJump = true;
	}

	if( result.normal.x )
	{
		m_isWallCling = true;
		m_wallClingNormal = result.normal.x;
	}
}

//--------------------------------------------------------------------------------

void Player::jump()
{
	if( m_waitJump )
		return;

	if( m_canJump )
	{
		if( m_canJumpDown && System::getKeyState( KeyCode::s ) )
			m_jumpingDown = true;
		else
		{
			velocity.y = m_jumpPower;
			m_canJump = false;
		}
	}
	else if( m_isWallCling )
	{
		Vec2 normal = m_wallJumpNormal;
		normal.x *= m_wallClingNormal;
		velocity = normal * m_wallJumpPower;

		m_isWallCling = false;
	}
	else if( m_canDoubleJump )
	{
		velocity.y = m_doubleJumpPower;
		m_canDoubleJump = false;
	}

	m_waitJump = true;
}

//--------------------------------------------------------------------------------

void Player::dash()
{
	if( m_waitDash )
		return;

	m_waitDash = true;

	if( !m_canDash )
		return;

	Vec2 direction;
	if( System::getKeyState( KeyCode::d ) )
		direction.x += 1.0;
	if( System::getKeyState( KeyCode::a ) )
		direction.x -= 1.0;
	if( System::getKeyState( KeyCode::w ) )
		direction.y += 1.0;
	if( System::getKeyState( KeyCode::s ) )
		direction.y -= 1.0;

	if( direction == Vec2() )
	{
		m_waitDash = true;
		return;
	}

	direction = direction.normalize();
	direction *= m_dashPower;

	velocity = direction;

	m_canDash = false;
	m_isDashing = true;
	m_waitDash = true;

	Timer::addTimer( m_dashCooldown, nullptr, [this] { m_canDash = true; }, false );
	Timer::addTimer( m_dashReleaseTime, nullptr,
					 [this] { 
						 m_isDashing = false; 
						 velocity.y = velocity.y > m_jumpReleaseThreshold ? m_jumpReleaseThreshold : velocity.y;
					 }, false );
}

//--------------------------------------------------------------------------------

void Player::kill()
{
	System::getWorld()->reset();
}

//================================================================================
