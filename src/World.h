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
	virtual void reset() {}

public:
	inline shared_ptr< Camera > getCamera() { return m_camera; }

	inline Vec2 getGravity() const { return m_gravity; }
	inline void setGravity( Vec2 gravity ) { m_gravity = gravity; }
	
protected:
	shared_ptr< Camera > m_camera;
	Vec2 m_gravity;
};

//================================================================================
