//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "Camera.h"

//================================================================================

namespace Worlds {

//================================================================================

class World : public Object {
public:
	World();

	// Events
public:
	virtual void onSpawnChildren() override;
	virtual void reset() {}
	virtual void loadWorld( string name ) {};
	virtual void unloadWorld() {};
	virtual void setCheckpoint( size_t checkpoint ) {};

public:
	inline shared_ptr< Gfx::Camera > getCamera() { return m_camera; }

protected:
	shared_ptr< Gfx::Camera > m_camera;
};

//================================================================================

} // Worlds

//================================================================================