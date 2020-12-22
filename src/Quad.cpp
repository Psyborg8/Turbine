#include "Quad.h"

#include "System.h"

//================================================================================

Quad::Quad() : 
m_color(), 
m_width( 0.0f ), 
m_height( 0.0f ), 
m_position( b2Vec2( 0.0f, 0.0f ) ), 
m_rotation( 0.0f )
{
	Object::setName( "Quad" );
}

//--------------------------------------------------------------------------------

Quad::Quad( float width, float height, b2Vec2 center, float rotation ) : 
m_color(), 
m_width( width ), 
m_height( height ), 
m_position( center ), 
m_rotation( rotation )
{
	Object::setName( "Quad" );
	calculate();
}

//--------------------------------------------------------------------------------

void Quad::onRender()
{
	if( m_color.a == 0.0 )
	{
		return;
	}

	// Render
	glBegin( GL_POLYGON );

	glColor4f( m_color.r, m_color.g, m_color.b, m_color.a );

	// Convert from b2PolygonShape and draw
	uint32 count = b2PolygonShape::m_count;
	for( size_t i = 0u; i < count; ++i )
	{
		b2Vec2 vertex = b2PolygonShape::m_vertices[ i ];
		glVertex2f( vertex.x, vertex.y );
	}

	b2Vec2 first = b2PolygonShape::m_vertices[ 0 ];
	glVertex2f( first.x, first.y );

	glEnd();
}

//--------------------------------------------------------------------------------

void Quad::calculate()
{
	SetAsBox( m_width, m_height, m_position, m_rotation );
}

//================================================================================