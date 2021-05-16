//================================================================================

#include "Player.h"

//--------------------------------------------------------------------------------

#include "MathTypes.h"
#include "System.h"

//================================================================================

Player::Player() : Box( Vec2(), Vec2( 0.1, 0.15 ), Colors::WHITE ), m_velocity( Vec2() ), m_acceleration( Vec2() )
{
	//
}

//--------------------------------------------------------------------------------

Player::Player( Vec2 pos ) : Box( pos, m_size, Colors::WHITE ), m_velocity( Vec2() ), m_acceleration( Vec2() )
{
	//
}

//--------------------------------------------------------------------------------

void Player::onUpdate( double deltaTime )
{
	m_pos += m_velocity * deltaTime;
}

//--------------------------------------------------------------------------------

void Player::onRender()
{
	Box::onRender();
}

//--------------------------------------------------------------------------------

void Player::onKeyboardPress( int key )
{
	const KeyCode keyCode = static_cast< KeyCode >( key );
	if( keyCode == KeyCode::d )
		m_velocity.x = m_moveSpeed;
	else if( keyCode == KeyCode::a )
		m_velocity.x = -m_moveSpeed;
}

//--------------------------------------------------------------------------------

void Player::onKeyboardRelease( int key )
{
	const KeyCode keyCode = static_cast< KeyCode >( key );
	if( keyCode == KeyCode::d )
		m_velocity.x = 0.0;
	else if( keyCode == KeyCode::a )
		m_velocity.x = 0.0;
}

//--------------------------------------------------------------------------------

void Player::onCollision( shared_ptr<Object> target, Vec2 normal )
{
	//
}

//================================================================================
