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

	Math::Vec2 getPosition() const override { return m_sprite->getPosition(); }
	Math::Color getColor() const { return m_sprite->getFillColor(); }
	Math::Color getOutlineColor() const { return m_sprite->getOutlineColor(); }
	Math::Vec2 getSize() const { return m_sprite->getScale(); }

	void setOrigin( Math::Vec2 origin ) { 
		m_origin = origin; 
		m_sprite->setOrigin( sf::Vector2f( origin.x * m_sprite->getLocalBounds().width, origin.y * m_sprite->getLocalBounds().height ) ); 
	}

public:
	void init( const Pattern& pattern, int idx );

	void kill() { m_dead = true; }
	bool isDead() const { return m_dead; }

private:
	Math::Vec2 processVelocitySet( const VelocitySet& set, Math::Vec2 current, Math::Vec2 position );
	void initializeVelocitySet( VelocitySet& set, Math::Vec2 position );
	Math::Vec2 processPositionSet( const PositionSet& set, int idx );

private:
	shared_ptr< sf::Shape > m_sprite;

	Pattern m_pattern;
	vector< shared_ptr< Emitter::Emitter > > m_emitters;

	microseconds m_duration{ std::numeric_limits< int32_t >::min() };

	float m_alpha{ 1.0f };
	bool m_dead{ false };

	Math::Vec2 m_origin{ Math::Vec2( 0.5f, 0.5f ) };
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