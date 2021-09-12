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

	void setSize( Math::Vec2 size ) override { setSize( size.x ); }
	void setSize( float radius ) { m_shape.setRadius( radius ); }

	sf::CircleShape getShape() const { return m_shape; }

	Math::Vec2 getSize() const override { return Math::Vec2( m_shape.getRadius(), m_shape.getRadius() ); }

	void kill() { m_dead = true; }
	bool isDead() const { return m_dead; }

public:
	void init( const Pattern& pattern );

private:
	sf::CircleShape m_shape;
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