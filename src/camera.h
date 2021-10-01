//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "mathtypes.h"
#include "object.h"

//================================================================================

namespace Game { class RigidRect; }

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

class Camera {
public:
	Camera();

	// Get-Set
public:
	inline Math::Vec2 getPosition() const { return m_position; }
	inline void setPosition( Math::Vec2 position ) { m_position = position; calculate(); }

	inline float getDistance() const { return m_distance.y; }
	inline void setDistance( float distance ) { m_distance.y = distance; calculate(); }

	shared_ptr< Game::RigidRect > getRect() const;

	// Utility
public:
	void calculate();

	Math::Vec2 scale( Math::Vec2 in );
	float scale( float in );
	int scale( int in );

	// Variables
protected:
	Math::Vec2 m_position{ 0.0f, 0.0f };
	Math::Vec2 m_distance{ 1024.0f, 1024.0f };
};

//--------------------------------------------------------------------------------

} // Gfx

//================================================================================