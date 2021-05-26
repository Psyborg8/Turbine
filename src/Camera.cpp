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

void Camera::calculate() {
	sf::RenderWindow* window = System::getWindow();
	System::SystemInfo systemInfo = System::getSystemInfo();
	m_distance.x = m_distance.y / systemInfo.height * systemInfo.width;

	sf::FloatRect viewport;
	viewport.left = m_position.x - m_distance.x / 2.0f;
	viewport.top = m_position.y - m_distance.y / 2.0f;
	viewport.width = m_distance.x;
	viewport.height = m_distance.y;

	sf::View view{ viewport };

	window->setView( view );
}

//================================================================================

} // Gfx

//================================================================================