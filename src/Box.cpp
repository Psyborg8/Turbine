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

Box::Box( Vec2 pos, Vec2 size, Color color ) : Object( "Box" ), m_pos( pos ), m_size( size ), m_color( color )
{

}

//--------------------------------------------------------------------------------

void Box::onRender()
{
	if( m_color.a == 0.0f )
		return;

	// Render
	glBegin( GL_QUADS );

	glColor4f( m_color.r, m_color.g, m_color.b, m_color.a );

	array< Vec2, 4 > vertices = calculateVertices();
	for( Vec2 vert : vertices )
	{
		glVertex2d( vert.x, vert.y );
	}

	glEnd();
}

//--------------------------------------------------------------------------------

void Box::onCollision( shared_ptr< Object > target, Vec2 normal )
{
	shared_ptr< Box > boxPtr = std::dynamic_pointer_cast< Box >( target );
	if( boxPtr != nullptr )
	{
		if( m_collisionType == CollisionType::DynamicBlocking && 
			boxPtr->getCollisionType() == CollisionType::StaticBlocking )
		{
			if( normal == Vec2( 1.0, 0.0 ) )
			{
				const double left = boxPtr->getPos().x;
				Vec2 pos = m_pos;
				m_pos.x = left - m_size.x;
			}
			else if( normal == Vec2( -1.0, 0.0 ) )
			{
				const double right = boxPtr->getPos().x + boxPtr->getSize().x;
				Vec2 pos = m_pos;
				m_pos.x = right;
			}
			else if( normal == Vec2( 0.0, 1.0 ) )
			{
				const double bottom = boxPtr->getPos().y;
				Vec2 pos = m_pos;
				m_pos.y = bottom - m_size.y;
			}
			else
			{
				const double top = boxPtr->getPos().y + boxPtr->getSize().y;
				Vec2 pos = m_pos;
				m_pos.y = top;
			}
		}
	}
}

//--------------------------------------------------------------------------------

Vec2 Box::getMidpoint() const
{
	return m_pos + ( m_size / 2.0 );
}


//--------------------------------------------------------------------------------

bool Box::isColliding( shared_ptr< Object > target, Vec2& normal )
{
	shared_ptr< Box > boxPtr = std::dynamic_pointer_cast< Box >( target );
	if( boxPtr != nullptr )
		return isColliding( boxPtr, normal );

	return false;
}

//--------------------------------------------------------------------------------

bool Box::isColliding( Vec2 target, Vec2& normal )
{
	bool out = true;

	out &= target.x > m_pos.x && target.x < ( m_pos.x + m_size.x );
	out &= target.y > m_pos.y && target.y < ( m_pos.y + m_size.y );

	if( !out )
	{
		normal = Vec2();
		return false;
	}

	// Calculate normal
	const Vec2 midpoint = getMidpoint();
	const Vec2 dist = target - midpoint;

	if( abs( dist.x ) > abs( dist.y ) )
	{
		normal.x = dist.x / abs( dist.x );
		normal.y = 0.0;
	}
	else
	{
		normal.y = dist.y / abs( dist.y );
		normal.x = 0.0;
	}

	return true;
}

//--------------------------------------------------------------------------------

bool Box::isColliding( shared_ptr< Box > box, Vec2& normal )
{
	bool out = true;

	const Vec2 midPointA = getMidpoint();
	const Vec2 sizeA = getSize();

	const Vec2 midPointB = box->getMidpoint();
	const Vec2 sizeB = box->getSize();

	out &= abs( midPointA.x - midPointB.x ) < ( sizeA.x / 2.0 ) + ( sizeB.x / 2.0 );
	out &= abs( midPointA.y - midPointB.y ) < ( sizeA.y / 2.0 ) + ( sizeB.y / 2.0 );

	if( !out )
	{
		normal = Vec2();
		return false;
	}

	// Calculate normal
	const Vec2 midpoint = getMidpoint();
	const Vec2 dist = box->getMidpoint() - midpoint;

	if( abs( dist.x ) > abs( dist.y ) )
	{
		normal.x = dist.x / abs( dist.x );
		normal.y = 0.0;
	}
	else
	{
		normal.y = dist.y / abs( dist.y );
		normal.x = 0.0;
	}

	return true;
}

//--------------------------------------------------------------------------------

array< Vec2, 4 > Box::calculateVertices()
{
	Vec2 a, b, c, d;

	// Bottom-Left
	a.x = m_pos.x;
	a.y = m_pos.y;

	// Top-Left
	b.x = m_pos.x;
	b.y = m_pos.y + m_size.y;

	// Top-Right
	c.x = m_pos.x + m_size.x;
	c.y = m_pos.y + m_size.y;

	// Bottom-Right
	d.x = m_pos.x + m_size.x;
	d.y = m_pos.y;

	return array< Vec2, 4 > { a, b, c, d };
}

//================================================================================