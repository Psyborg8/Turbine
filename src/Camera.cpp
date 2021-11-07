//================================================================================

#include "camera.h"

//--------------------------------------------------------------------------------

#include "system.h"
#include "rigidrect.h"
#include "random.h"

//================================================================================

// CLASS - Camera

//================================================================================

namespace Gfx {

//================================================================================

shared_ptr< Game::RigidRect > Camera::getRect() const {
	shared_ptr< Game::RigidRect > out = make_shared< Game::RigidRect >();

	out->setSize( m_distance );
	out->setPosition( m_position - m_distance / 2.0f );

	return out;
}

//--------------------------------------------------------------------------------

void Camera::calculate( sf::RenderTarget* target ) {
	System::SystemInfo systemInfo = System::getSystemInfo();
	m_distance.x = m_distance.y / systemInfo.height * systemInfo.width;

	sf::FloatRect viewport;
	viewport.left = m_position.x - m_distance.x / 2.0f - m_offset.x / 2.0f;
	viewport.top = m_position.y - m_distance.y / 2.0f - m_offset.y / 2.0f;
	viewport.width = m_distance.x;
	viewport.height = m_distance.y;

	sf::View view{ viewport };

	target->setView( view );
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

//--------------------------------------------------------------------------------

void Camera::shake( CameraShake shake ) {
	m_shake = shake;
	Timers::removeTimer( m_shakeTimer );

	m_shakeTimer = Timers::addTimer( shake.duration,
									 [shake, this]( float alpha ) {
										 m_shake.distance = shake.distance / alpha;
									 },
									 [this] {
										 Timers::removeTimer( m_shakeStepTimer );
										 m_shakeTimer.reset();
									 }, false );

	shakeStep();
}

//--------------------------------------------------------------------------------

void Camera::shakeStep() {
	const float angle = Random::getFloat( 0.0f, 360.0f );
	const float radians = ( angle * PI ) / 180.f;
	const Math::Vec2 dist{ cos( radians ), sin( radians ) };

	m_shakeTarget = dist * m_shake.distance;
	m_shakeStart = m_offset;

	m_shakeStepTimer = Timers::addTimer( int( 1000.f / m_shake.intensity ),
										 [this]( float alpha ) {
											 m_offset = Math::mix( m_shakeStart, m_shakeTarget, alpha );
										 },
										 std::bind( &Camera::shakeStep, this ), false );
}

//================================================================================

} // Gfx

//================================================================================