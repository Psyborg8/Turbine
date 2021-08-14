//================================================================================

#include "Camera.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

// CLASS - Camera

//================================================================================

namespace Gfx {

//================================================================================

Camera::Camera() {
	calculate();
}

//--------------------------------------------------------------------------------

shared_ptr< Game::RigidRect > Camera::getRect() const {
	shared_ptr< Game::RigidRect > out = make_shared< Game::RigidRect >();

	out->setSize( m_distance );
	out->setPosition( m_position - m_distance / 2.0f );

	return out;
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

//--------------------------------------------------------------------------------

Math::Vec2 Camera::scale( Math::Vec2 in ) {
	const float factor = getDistance() / 196.0f;
	const Math::Vec2 out = in * factor;
	return out;
}

//--------------------------------------------------------------------------------

float Camera::scale( float in ) {
	const float factor = getDistance() / 196.0f;
	const float out = in * factor;
	return out;
}

//--------------------------------------------------------------------------------

int Camera::scale( int in ) {
	const float factor = getDistance() / 196.0f;
	const int out = int( in * factor );
	return out;
}

//================================================================================

} // Gfx

//================================================================================