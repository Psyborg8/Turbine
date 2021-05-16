//================================================================================

#include "Camera.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

// CLASS - Camera

//================================================================================

Camera::Camera( Vec2 position, float distance ) : Object(), m_position( position )
{
	m_distance = Vec2();
	m_distance.y = distance;

	setName( "Camera" );
	calculate();
}

//--------------------------------------------------------------------------------

Vec2 Camera::screenToWorld( Vec2 screenPosition )
{
	Vec2 out;

	out.x = m_position.x + ( m_distance.x * screenPosition.x );
	out.y = m_position.y + ( m_distance.y * screenPosition.y );

	return out;
}

//--------------------------------------------------------------------------------

Vec2 Camera::worldToScreen( Vec2 worldPosition )
{
	Vec2 out;

	out.x = ( worldPosition.x - m_position.x ) / m_distance.x;
	out.y = ( worldPosition.y - m_position.y ) / m_distance.y;

	return out;
}

//--------------------------------------------------------------------------------

void Camera::calculate()
{
	GLdouble left, right, top, bottom;

	System::SystemInfo systemInfo = System::getSystemInfo();
	m_distance.x = m_distance.y / systemInfo.height * systemInfo.width;

	left = GLdouble( m_position.x - m_distance.x );
	right = GLdouble( m_position.x + m_distance.x );
	top = GLdouble( m_position.y + m_distance.y );
	bottom = GLdouble( m_position.y - m_distance.y );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( left, right, bottom, top );
	glMatrixMode( GL_MODELVIEW );
}

//================================================================================