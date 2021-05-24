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
	
protected:
	shared_ptr< Camera > m_camera;
};

//================================================================================
