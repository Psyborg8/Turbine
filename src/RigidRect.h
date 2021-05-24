//================================================================================

#pragma once

//================================================================================

#include "Rect.h"
#include "Object.h"

//================================================================================

class RigidRect : public Gfx::Rect, public Object {
public:
	RigidRect();
	RigidRect( Vec2 position, Vec2 size, Color color );

public:
	virtual inline void onRender() override { render(); };
	virtual inline void onPostUpdate( double deltaTime ) override { position += velocity * deltaTime; }

	virtual Collision::CollisionResult isColliding( ObjectPtr target ) override;

	virtual inline Vec2 getPosition() const override { return position; }
	virtual inline void setPosition( Vec2 pos ) override { position = pos; }
};