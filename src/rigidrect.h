//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "object.h"
#include "collision.h"

//================================================================================

namespace Game {

//================================================================================

class RigidRect : public Object {
public:
	RigidRect();
	RigidRect( Math::Vec2 position, Math::Vec2 size, Math::Color color );

public:
	virtual void onRender() override;
	virtual Collision::CollisionResult isColliding( shared_ptr< Object > target ) override;

	inline Math::Vec2 getPosition() const override { return Math::Vec2( m_rect.getPosition() ); }
	inline void setPosition( Math::Vec2 position ) override { m_rect.setPosition( position.sf() ); }

	inline Math::Vec2 getSize() const { return Math::Vec2( m_rect.getSize() ); }
	inline void setSize( Math::Vec2 size ) { m_rect.setSize( size.sf() ); }
		
	inline const sf::RectangleShape& getRect() const { return m_rect; }
	inline sf::RectangleShape& getRect() { return m_rect; }
	inline void setRect( const sf::RectangleShape& rect ) { m_rect = rect; }

	inline Math::Color getColor() const { return Math::Color( m_rect.getFillColor() ); }
	inline void setColor( Math::Color color ) { m_rect.setFillColor( color.sf() ); }

protected:
	sf::RectangleShape m_rect;
};

//================================================================================

} // Game

//================================================================================