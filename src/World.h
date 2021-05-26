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
	inline Gfx::Camera& getCamera() { return m_camera; }
	
	inline Math::Color getBackgroundColor() const { return m_backgroundColor; }

protected:
	Gfx::Camera m_camera;
	Math::Color m_backgroundColor;
};

//================================================================================

} // Worlds

//================================================================================