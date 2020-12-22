#include "Quad.h"

#include "System.h"

//================================================================================

Quad::Quad() : m_position(), m_scale(), m_anchor(), m_rotation( 0.0f ), m_color()
{
	Object::setName( "Quad" );
}

//--------------------------------------------------------------------------------

void Quad::onRender()
{
	if( m_color.a == 0.0 )
	{
		return;
	}

	// Render
	glBegin( GL_QUADS );

	glColor4d( m_color.r, m_color.g, m_color.b, m_color.a );

	array< Vec2, 4 > vertices = getVertices();
	for( Vec2 vert : vertices )
	{
		glVertex2f( GLfloat( vert.x ), GLfloat( vert.y ) );
	}

	glEnd();
}

//--------------------------------------------------------------------------------

array< Vec2, 4 > Quad::getVertices()
{
	Vec2 scale = m_scale.toScreenRatio();
	scale = Vec2::HadamardProduct( scale, Vec2( 2.0f, 2.0f ) );
	scale -= Vec2( 1.0f, 1.0f );

	array< Vec2, 4 > vertices = { Vec2{ 0.0, 0.0 }, Vec2{ 0.0, scale.y }, Vec2{ scale.x, scale.y }, Vec2{ scale.x, 0.0 } };

	// Anchor
	const Vec2 anchor = m_anchor;
	for( Vec2& vert : vertices )
	{
		vert.x -= anchor.x * scale.x;
		vert.y -= anchor.y * scale.y;
	}

	// Rotation
	const float rotation = m_rotation;
	if( rotation )
	{
		float rot = 1.0f - rotation; // We want clockwise rotation

		for( Vec2& vert : vertices )
		{
			Vec2 vertOut;

			vertOut.x = vert.x * cos( rot ) - vert.y * sin( rot );
			vertOut.y = vert.x * sin( rot ) + vert.y * cos( rot );

			vert = vertOut;
		}
	}

	// Position
	const Vec2 position = m_position;
	for( Vec2& vert : vertices )
	{
		vert.x += position.x;
		vert.y += position.y;
	}

	return vertices;
}

//--------------------------------------------------------------------------------

bool Quad::isPointInside( Vec2 point )
{
	array< Vec2, 4 > vertices = getVertices();

	Vec2 tris[ 4 ][ 3 ] = { { vertices.at( 0 ), point, vertices.at( 3 ) },
							{ vertices.at( 3 ), point, vertices.at( 2 ) },
							{ vertices.at( 2 ), point, vertices.at( 1 ) },
							{ point, vertices.at( 1 ), vertices.at( 0 ) } };

	Vec2 inTris[ 2 ][ 3 ] = { { vertices.at( 0 ), vertices.at( 1 ), vertices.at( 2 ) },
							  { vertices.at( 2 ), vertices.at( 3 ), vertices.at( 0 ) } };

	const auto areaOfTri = []( Vec2* tri ) {
		Vec2 a, b, c;
		a = tri[ 0 ];
		b = tri[ 1 ];
		c = tri[ 2 ];

		return abs( ( b.x * a.y - a.x * b.y ) + ( c.x * b.y - b.x * c.y ) + ( a.x * c.y - c.x * a.y ) ) / 2;
	};

	float sumOfTris = 0.0;
	float areaOfQuad = 0.0;

	for( size_t i = 0; i < 4; ++i )
	{
		sumOfTris += areaOfTri( tris[ i ] );
	}

	for( size_t i = 0; i < 2; ++i )
	{
		areaOfQuad = areaOfTri( inTris[ i ] );
	}

	return sumOfTris <= areaOfQuad;
}

//--------------------------------------------------------------------------------

bool Quad::isInsideQuad( Quad quad )
{
	array< Vec2, 4 > vertices = getVertices();

	bool outside = false;
	for( size_t i = 0u; i < 4; ++i )
	{
		if( !quad.isPointInside( vertices.at( i ) ) )
		{
			return false;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------

b2PolygonShape Quad::toCollider()
{
	Vec2 position = m_position;
	float rotation = m_rotation;
	Vec2 anchor = m_anchor;
	Vec2 scale = m_scale;
	float width = scale.x;
	float height = scale.y;

	// Aspect ratio

	position -= anchor;
	position += Vec2( 0.5, 0.5 );

	position = position.toScreenRatio();

	b2PolygonShape collider;
	collider.SetAsBox( width, height, position, rotation );

	return collider;
}

//================================================================================