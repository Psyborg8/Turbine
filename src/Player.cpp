//================================================================================

#include "Player.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "Camera.h"
#include "World.h"

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
		m_face->setWidth( 1.0f );
		m_face->setHeight( 1.0f );
		m_face->setPosition( b2Vec2( 0.0f, 0.0f ) );
		m_face->setRotation( 0.0f );
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
	b2Vec2 oldPosition = m_position;
	shared_ptr< Camera > camera = System::getWorld()->getCamera();
	// Movement
	b2Vec2 direction;
	b2Vec2 target = camera->screenToWorld( m_target );
	if( target != m_position )
	{
		b2Vec2 difference = target - m_position;
		difference.Normalize();
		const float length = mathfu::Clamp( abs( difference.Length() ), 0.0f, 0.5f ) * 2;
		direction = b2Vec2( difference.x * length, difference.y * length );
	}
	else
	{
		direction = b2Vec2( 0.0f, 0.0f );
	}
	
	m_velocity.x += direction.x * m_acceleration * deltaTime;
	m_velocity.y += direction.y * m_acceleration * deltaTime;

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
		bool hit = m_face->RayCast( &output, input, transform, childIdx );

		if( hit )
		{
			m_velocity.x = -m_velocity.x;
			m_velocity.y = -m_velocity.y;
		}
	}

	// Finish movement
	m_position += m_velocity;

	m_face->setPosition( m_position );
	System::getWorld()->getCamera()->setPosition( m_position );
	m_line->setStart( m_position );
	m_line->setEnd( target - ( oldPosition - m_position ) );
}

//--------------------------------------------------------------------------------

void Player::onMouseMove( float x, float y )
{
	m_target = b2Vec2( x, y );
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