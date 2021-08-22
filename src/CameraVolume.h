//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "RigidRect.h"

//================================================================================

namespace Game {

//--------------------------------------------------------------------------------

class CameraVolume : public RigidRect {
public:
	CameraVolume() = default;

public:
	void onProcessCollisions();

public:
	inline float getDistance() const { return m_distance; }
	inline void setDistance( float distance ) { m_distance = distance; }

	inline Math::Vec2 getCameraPosition() const { return m_position; }
	inline void setCameraPosition( Math::Vec2 position ) { m_position = position; }

protected:
	float m_distance{ 128.0f };
	Math::Vec2 m_cameraPosition;
};

//--------------------------------------------------------------------------------

}

//================================================================================