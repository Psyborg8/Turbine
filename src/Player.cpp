//================================================================================

#include "Player.h"

//--------------------------------------------------------------------------------

#include "MathTypes.h"
#include "System.h"
#include "World.h"
#include "Timer.h"

//================================================================================

Player::Player() : Box( Vec2(), Vec2( 0.15, 0.2 ), Colors::BLUE ), m_velocity( Vec2() )
{
	m_collisionType = CollisionType::DynamicBlocking;
}

//--------------------------------------------------------------------------------

Player::Player( Vec2 pos ) : Box( pos, Vec2( 0.15, 0.2 ), Colors::BLUE ), m_velocity( Vec2() )
{
	m_collisionType = CollisionType::DynamicBlocking;
}

//--------------------------------------------------------------------------------

void Player::onUpdate( double deltaTime )
{
	Box::onUpdate( deltaTime );

	Vec2 gravity = System::getWorld()->getGravity();
	bool didMove = false;

	if( m_isDashing )
		goto afterMovement;

	// Movement
	if( abs( m_velocity.x ) <= m_maxMoveSpeed )
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
		if( abs( m_velocity.x ) + move > m_maxMoveSpeed )
			m_velocity.x = m_maxMoveSpeed * ( move / abs( move ) );
		else
			m_velocity.x += move;	

		// Disable friction
		if( move )
			didMove = true;
	}

	// Friction
	if( m_velocity.x && !didMove )
	{
		double friction = m_frictionMultiplier * abs( m_velocity.x ) * deltaTime;
		friction = std::clamp( friction, m_minFriction * deltaTime, m_maxFriction * deltaTime );
		if( abs( m_velocity.x ) < friction )
			m_velocity.x = 0.0;
		else
			m_velocity.x -= friction * ( m_velocity.x / abs( m_velocity.x ) );
	}

	// Gravity
	m_velocity += gravity * deltaTime;
	m_velocity.y = m_velocity.y < -m_terminalVelocity ? -m_terminalVelocity : m_velocity.y;

	// Jump Release
	if( !System::getKeyState( KeyCode::Space ) )
		m_velocity.y = min( m_velocity.y, m_jumpReleaseThreshold );

	// Wall Cling
	if( m_isWallCling )
	{
		double friction = m_wallFrictionMultiplier * abs( m_velocity.y ) * deltaTime;
		friction = std::clamp( friction, m_minWallFriction * deltaTime, m_maxWallFriction * deltaTime );
		if( abs( m_velocity.y ) < friction )
			m_velocity.y = 0.0;
		else
			m_velocity.y -= friction * ( m_velocity.y / abs( m_velocity.y ) );
	}

afterMovement:
	m_pos += m_velocity * deltaTime;

	m_canJump = false;
	m_isWallCling = false;
}

//--------------------------------------------------------------------------------

void Player::onRender()
{
	Box::onRender();
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

void Player::onCollision( shared_ptr<Object> target, Vec2 normal )
{
	Box::onCollision( target, normal );

	if( target->getCollisionType() == CollisionType::StaticBlocking )
	{
		if( normal.x == 1.0 )
			m_velocity.x = min( m_velocity.x, 0.0 );
		else if( normal.x == -1.0 )
			m_velocity.x = max( m_velocity.x, 0.0 );
		else if( normal.y == 1.0 )
			m_velocity.y = min( m_velocity.y, 0.0 );
		else if( normal.y == -1.0 )
			m_velocity.y = max( m_velocity.y, 0.0 );

		if( normal.y == -1.0 )
		{
			m_canJump = true;
			m_canDoubleJump = true;
		}

		if( normal.x )
		{
			m_isWallCling = true;
			m_wallClingNormal = -normal.x;
		}
	}
}

//--------------------------------------------------------------------------------

void Player::jump()
{
	if( m_waitJump )
		return;

	if( m_isWallCling )
	{
		Vec2 normal = m_wallJumpNormal;
		normal.x *= m_wallClingNormal;
		m_velocity = normal * m_wallJumpPower;

		m_isWallCling = false;
	}
	else if( m_canJump )
	{
		m_velocity.y = m_jumpPower;
		m_canJump = false;
	}
	else if( m_canDoubleJump )
	{
		m_velocity.y = m_doubleJumpPower;
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

	m_velocity = direction;

	m_canDash = false;
	m_isDashing = true;
	m_waitDash = true;

	Timer::addTimer( m_dashCooldown, nullptr, [this] { m_canDash = true; }, false );
	Timer::addTimer( m_dashReleaseTime, nullptr,
					 [this] { 
						 m_isDashing = false; 
						 m_velocity.y = m_velocity.y > m_jumpReleaseThreshold ? m_jumpReleaseThreshold : m_velocity.y;
					 }, false );
}

//--------------------------------------------------------------------------------

void Player::kill()
{

}

//================================================================================
