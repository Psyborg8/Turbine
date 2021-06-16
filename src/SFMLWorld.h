//================================================================================

#pragma once

//================================================================================

#include "World.h"
#include "GameTimer.h"
#include "Player.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

class SFMLWorld : public World {
public:
	SFMLWorld() = default;

public:
	void onSpawnChildren() override;
	void onStart() override;
	void onRender() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onProcessCollisions() override;
	void onMessage( string message ) override;

	void reset() override;

public:
	inline Math::Vec2 getCameraTarget() const { return m_cameraTarget; }
	inline void setCameraTarget( Math::Vec2 target ) { m_cameraTarget = target; }

	inline float getCameraDistanceTarget() const { return m_cameraDistanceTarget; }
	inline void setCameraDistanceTarget( float target ) { m_cameraDistanceTarget = target; }
	
protected:
	Math::Vec2 m_levelStart;
	Math::Vec2 m_cameraTarget;
	float m_cameraDistanceTarget;

	Timers::TimerID m_dashPromptTimer;
	Math::Vec2 m_dashPromptPosition;

// Debug
protected:
	shared_ptr< Object > m_physicsPage;
	shared_ptr< Object > m_performancePage;
	shared_ptr< Object > m_joystickPage;
	shared_ptr< Gfx::GameTimer > m_timer;

	vector< string > m_activeEvents;

	int m_debugPage{ 0 };
};

//--------------------------------------------------------------------------------

}

//================================================================================