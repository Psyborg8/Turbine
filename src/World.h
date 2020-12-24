//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "Camera.h"

//================================================================================

class World : public Object
{
public:
	World();

// Events
public:
	virtual void onSpawnChildren() override;

public:
	inline shared_ptr< Camera > getCamera() { return m_camera; }

	inline float getGravity() const { return m_gravity; }
	inline void setGravity( c2v gravity ) { m_gravity = gravity; }
	
protected:
	shared_ptr< Camera > m_camera;
	c2v m_gravity;
};

//================================================================================
