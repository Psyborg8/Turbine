//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "Object.h"
#include "Random.h"
#include "Timer.h"
#include "RigidRect.h"
#include "Patterns.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

namespace Emitter {
class Emitter;
}

//--------------------------------------------------------------------------------

namespace Particle {

//--------------------------------------------------------------------------------

class Particle : public Object {
public:

public:
	Particle() = default;

public:
	void onUpdate( sf::Time deltaTime ) override;
	void onPostUpdate( sf::Time deltaTime ) override;
	void onRender() override;
	void onDestroy() override;

	void setSize( Math::Vec2 size ) override { m_sprite.setScale( size.sf() ); m_shape->setScale( size.sf() ); }
	void setColor( Math::Color color ) { m_sprite.setColor( color.sf() ); m_shape->setFillColor( color.sf() ); }

	std::shared_ptr< sf::Shape > getShape() const { return m_shape; }

	Math::Vec2 getSize() const override { return Math::Vec2( m_shape->getScale() ); }

	void kill() { m_dead = true; }
	bool isDead() const { return m_dead; }

public:
	void init( const Pattern& pattern );

private:
	std::shared_ptr< sf::Shape > m_shape;
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	Pattern m_pattern;
	vector< shared_ptr< Emitter::Emitter > > m_emitters;

	milliseconds m_duration{ std::numeric_limits< int32_t >::min() };

	float m_alpha{ 1.0f };
	bool m_dead{ false };
};

//================================================================================

void spawn( Object* parent, Pattern pattern, Math::Vec2 position = Math::Vec2() );

void killAll();

void cleanup();

vector< shared_ptr< Particle > > getAll();

//--------------------------------------------------------------------------------

}
}

//================================================================================