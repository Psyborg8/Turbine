//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "Wall.h"
#include "Camera.h"

//================================================================================

class World : public Object
{
public:
	World();

// Events
public:
	virtual void onSpawnChildren() override;
	virtual void onUpdate( float deltaTime ) override;

// Get-Set
public:
	inline b2Vec2 getGravity() { return m_gravity; }
	inline void setGravity( b2Vec2 gravity ) { m_gravity = gravity; m_b2World.SetGravity( gravity ); }

	inline shared_ptr< Camera > getCamera() { return m_camera; }

// Variables
protected:
	b2Vec2 m_gravity;
	b2World m_b2World;
	
	shared_ptr< Camera > m_camera;
};

//================================================================================
