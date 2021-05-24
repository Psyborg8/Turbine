//================================================================================

#include "Camera.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

// CLASS - Camera

//================================================================================

namespace Gfx {

//================================================================================

Camera::Camera() : m_distance( Math::Vec2( 0.0, 1.0 ) ), m_position( Math::Vec2() ) {
	calculate();
}

//--------------------------------------------------------------------------------

Math::Vec2 Camera::screenToWorld( Math::Vec2 screenPosition ) {
	Math::Vec2 out;

	out.x = m_position.x + ( m_distance.x * screenPosition.x );
	out.y = m_position.y + ( m_distance.y * screenPosition.y );

	return out;
}

//--------------------------------------------------------------------------------

Math::Vec2 Camera::worldToScreen( Math::Vec2 worldPosition ) {
	Math::Vec2 out;

	out.x = ( worldPosition.x - m_position.x ) / m_distance.x;
	out.y = ( worldPosition.y - m_position.y ) / m_distance.y;

	return out;
}

//--------------------------------------------------------------------------------

void Camera::calculate() {
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

} // Gfx

//================================================================================