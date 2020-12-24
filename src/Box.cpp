//================================================================================

#include "Box.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

Box::Box() : Object( "Box" )
{
	//
}

//--------------------------------------------------------------------------------

Box::Box( c2AABB box, Color color ) : Object( "Box" ), m_box( box ), m_color( color )
{
}

//--------------------------------------------------------------------------------

void Box::onRender()
{
	if( m_color.a == 0.0 )
	{
		return;
	}

	// Render
	glBegin( GL_POLYGON );

	glColor4f( m_color.r, m_color.g, m_color.b, m_color.a );

	array< Vec2, 4 > vertices = calculateVertices();
	for( Vec2 vert : vertices )
	{
		glVertex2f( vert.x, vert.y );
	}

	glEnd();
}

//--------------------------------------------------------------------------------

Vec2 Box::getMidpoint()
{
	Vec2 size = getSize();
	return c2Add( m_box.min, c2Div( size, 2.0f ) );
}

//--------------------------------------------------------------------------------

Vec2 Box::getSize()
{
	return c2Sub( m_box.max, m_box.min );
}

//--------------------------------------------------------------------------------

array< Vec2, 4 > Box::calculateVertices()
{
	Vec2 a, b, c, d;

	// Bottom-Left
	a.x = m_box.min.x;
	a.y = m_box.min.y;

	// Top-Left
	b.x = m_box.min.x;
	b.y = m_box.max.y;

	// Top-Right
	c.x = m_box.max.x;
	c.y = m_box.max.y;

	// Bottom-Right
	d.x = m_box.max.x;
	d.y = m_box.min.y;

	return array< Vec2, 4 > { a, b, c, d };
}

//--------------------------------------------------------------------------------

void Box::moveBox( Vec2 direction )
{
	m_box.min = c2Add( m_box.min, direction );
	m_box.max = c2Add( m_box.max, direction );
}

//--------------------------------------------------------------------------------

void Box::scaleBox( Vec2 scale )
{
	Vec2 size = c2Sub( m_box.max, m_box.min );
	Vec2 halfSize = c2Div( size, 2.0f );
	Vec2 middle = c2Add( m_box.min, c2Div( size, 2.0f ) );
	
	m_box.min = c2Sub( middle, halfSize );
	m_box.max = c2Add( middle, halfSize );
}

//================================================================================