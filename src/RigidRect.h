//================================================================================

#pragma once

//================================================================================

#include "Rect.h"
#include "Object.h"

//================================================================================

namespace Game {

//================================================================================

class RigidRect : public Gfx::Rect, public Object {
public:
	RigidRect();
	RigidRect( Math::Vec2 position, Math::Vec2 size, Math::Color color );

public:
	virtual inline void onRender() override { render(); };
	virtual inline void onPostUpdate( double deltaTime ) override { position += velocity * deltaTime; }

	virtual Collision::CollisionResult isColliding( shared_ptr< Object > target ) override;

	virtual inline Math::Vec2 getPosition() const override { return position; }
	virtual inline void setPosition( Math::Vec2 pos ) override { position = pos; }
};

//================================================================================

} // Game

//================================================================================