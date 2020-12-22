//================================================================================

#include "Wall.h"

//================================================================================

// CLASS - Wall

//================================================================================

Wall::Wall() : Quad(), m_length( 0.0f ), m_width( 0.0f ), m_middle(), m_rotation( 0.0f )
{
	Object::setName( "Wall" );
	Object::setAspectType( AspectType::Stretch );
}

//--------------------------------------------------------------------------------

Wall::Wall( float length, float width, Vec2 middle, float rotation, Color color ) : Quad(), m_length( length ), m_width( width ), m_middle( middle ), m_rotation( rotation )
{
	Object::setAspectType( AspectType::Stretch );

	calculateQuad(),
	calculateCollider();

	Quad::setColor( color );
}

//--------------------------------------------------------------------------------

void Wall::onRender()
{
	Quad::onRender();
}

//--------------------------------------------------------------------------------

void Wall::calculateCollider()
{
	m_collider = Quad::toCollider();
}

//--------------------------------------------------------------------------------

void Wall::calculateQuad()
{
	Quad::setPosition( m_middle );
	Quad::setAnchor( Vec2( 0.5, 0.5 ) );
	Quad::setRotation( m_rotation );
	Quad::setScale( Vec2( m_width, m_length ) );

	Quad::setColor( m_color );
}

//================================================================================