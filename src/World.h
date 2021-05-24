//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "Camera.h"

//================================================================================

namespace Worlds {

//================================================================================

class World : public Game::Object {
public:
	World();

	// Events
public:
	virtual void onSpawnChildren() override;
	virtual void reset() {}

public:
	inline shared_ptr< Gfx::Camera > getCamera() { return m_camera; }

protected:
	shared_ptr< Gfx::Camera > m_camera;
};

//================================================================================

} // Worlds

//================================================================================