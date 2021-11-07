//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "mathtypes.h"
#include "object.h"
#include "timer.h"

//================================================================================

namespace Game { class RigidRect; }

//================================================================================

namespace Gfx {

struct CameraShake {
	milliseconds duration;
	float distance;
	float intensity;
};

//--------------------------------------------------------------------------------

class Camera {
public:
	Camera() = default;

	// Get-Set
public:
	inline Math::Vec2 getPosition() const { return m_position; }
	inline void setPosition( Math::Vec2 position ) { m_position = position; }

	inline float getDistance() const { return m_distance.y; }
	inline void setDistance( float distance ) { m_distance.y = distance; }

	shared_ptr< Game::RigidRect > getRect() const;

	// Utility
public:
	void calculate( sf::RenderTarget* target );

	Math::Vec2 scale( Math::Vec2 in );
	float scale( float in );
	int scale( int in );

	void shake( CameraShake shake );
	void shakeStep();

	// Variables
protected:
	Math::Vec2 m_position{ 0.0f, 0.0f };
	Math::Vec2 m_distance{ 1024.0f, 1024.0f };
	Math::Vec2 m_offset{ 0.0f, 0.0f };

	CameraShake m_shake;
	Timers::TimerID m_shakeTimer;
	Timers::TimerID m_shakeStepTimer;
	Math::Vec2 m_shakeStart;
	Math::Vec2 m_shakeTarget;
};

//--------------------------------------------------------------------------------

} // Gfx

//================================================================================