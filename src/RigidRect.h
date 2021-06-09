//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "Collision.h"

//================================================================================

namespace Game {

//================================================================================

class RigidRect : public Object {
public:
	RigidRect();
	RigidRect( Math::Vec2 position, Math::Vec2 size, Math::Color color );

public:
	virtual inline void onPostUpdate( sf::Time deltaTime ) override { 
		if( m_velocity.length() > 0.0f )
			setPosition( getPosition() + m_velocity * deltaTime.asSeconds() ); 
	}

	virtual void onRender() override;
	virtual Collision::CollisionResult isColliding( shared_ptr< Object > target ) override;

	inline Math::Vec2 getVelocity() const { return m_velocity; }
	inline void setVelocity( Math::Vec2 velocity ) { m_velocity = velocity; }

	inline Math::Vec2 getPosition() const override { return Math::Vec2( m_rect.getPosition() ); }
	inline void setPosition( Math::Vec2 position ) override { m_rect.setPosition( position.sf() ); }

	inline Math::Vec2 getSize() const { return Math::Vec2( m_rect.getSize() ); }
	inline void setSize( Math::Vec2 size ) { m_rect.setSize( size.sf() ); }
		
	inline const sf::RectangleShape& getRect() const { return m_rect; }
	inline sf::RectangleShape& getRect() { return m_rect; }
	inline void setRect( const sf::RectangleShape& rect ) { m_rect = rect; }

	inline Math::Color getColor() const { return Math::Color( m_rect.getFillColor() ); }
	inline void setColor( Math::Color color ) { m_rect.setFillColor( color.sf() ); }

	inline pair< string, int > getTile() const { return m_tile; }
	inline void setTile( string tileset, int index ) { m_tile = make_pair( tileset, index ); }

protected:
	sf::RectangleShape m_rect;
	Math::Vec2 m_velocity;
	pair< string, int > m_tile;
};

//================================================================================

} // Game

//================================================================================