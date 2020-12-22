//================================================================================

#include "Player.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

// CLASS - Player

//================================================================================

Player::Player() :  m_velocity( 0.0, 0.0 ),
					m_position( 0.0, 0.0 ),
					m_target( 0.0, 0.0 ),
					m_color(),
					m_collider()
{

}

//--------------------------------------------------------------------------------

void Player::onSpawnChildren()
{
	m_line = Object::makeObject< Line >( "Player Line", this );
	m_face = Object::makeObject< Quad >( "Player Face", this  );
}

//--------------------------------------------------------------------------------

void Player::onStart()
{
	// Graphics
	{
		m_face->setScale( Vec2( 0.1f, 0.1f ) );
		m_face->setAnchor( Vec2( 0.5f, 0.5f ) );
		m_face->setPosition( Vec2( 0.0f, 0.0f ) );

		m_face->setColor( m_color );
	}

	{
		m_line->setColor( Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
		m_line->setThickness( 1.0f );
	}
}

//--------------------------------------------------------------------------------

void Player::onUpdate( float deltaTime )
{
	Vec2 oldPosition = m_position;
	// Movement
	Vec2 direction;
	if( m_target != m_position )
	{
		const Vec2 difference = m_target - m_position;
		const Vec2 normal = difference.Normalized();
		const float length = mathfu::Clamp( abs( difference.Length() ), 0.0f, 0.5f ) * 2;
		direction = normal * length;
	}
	else
	{
		direction = Vec2( 0.0f, 0.0f );
	}
	
	m_velocity.x += direction.x * m_acceleration * deltaTime;
	m_velocity.y += direction.y * m_acceleration * deltaTime;
	m_velocity.y += m_gravity * deltaTime;

	if( m_velocity.Length() > m_maxVelocity )
	{
		m_velocity.Normalize();
		m_velocity.x *= m_maxVelocity;
		m_velocity.y *= m_maxVelocity;
	}

	// Collision
	{
		b2RayCastInput input;
		input.p1 = oldPosition;
		input.p2 = m_position;

		b2Transform transform;
		transform.SetIdentity();

		b2RayCastOutput output;
		int32 childIdx = 0;
		bool hit = m_face->toCollider().RayCast( &output, input, transform, childIdx );

		if( hit )
		{
			m_velocity.x = -m_velocity.x;
			m_velocity.y = -m_velocity.y;
		}
	}

	// Finish movement
	m_position += m_velocity;

	m_face->setPosition( m_position );

	Vec2 position = position.toScreenRatio();
	m_line->setStart( position );
}

//--------------------------------------------------------------------------------

void Player::onMouseMove( float x, float y )
{
	const Vec2 target = Vec2( x, y );
	m_target = target;

	m_line->setEnd( target );
}

//--------------------------------------------------------------------------------

void Player::setColor( Color color )
{
	m_color = color;

	if( m_face != nullptr )
	{
		m_face->setColor( color );
	}

	if( m_line != nullptr )
	{
		m_line->setColor( color );
	}
}

//================================================================================