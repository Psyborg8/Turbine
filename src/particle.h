//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "object.h"
#include "timer.h"
#include "rigidrect.h"
#include "particle-pattern.h"

//================================================================================

namespace Gfx { namespace Emitter { class Emitter; } }

//================================================================================

namespace Gfx {
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

	Math::Vec2 getPosition() const override { 
		sf::Transform transform = m_sprite->getTransform();
		sf::FloatRect rect = m_sprite->getLocalBounds();

		sf::Vector2f midpoint = sf::Vector2f( rect.left + rect.width / 2.f, rect.top + rect.height / 2.f );

		return transform.transformPoint( midpoint );
	}
	Math::Color getColor() const { return m_sprite->getFillColor(); }
	Math::Color getOutlineColor() const { return m_sprite->getOutlineColor(); }
	Math::Vec2 getSize() const { return m_sprite->getScale(); }
	shared_ptr< sf::Shape > getShape() const { return m_sprite; }
	Math::Vec2 getAcceleration() const { return m_acceleration; }
	float getSpin() const { return m_spin; }

	void setOrigin( Math::Vec2 origin ) { 
		m_origin = origin; 
		m_sprite->setOrigin( sf::Vector2f( origin.x * m_sprite->getLocalBounds().width, origin.y * m_sprite->getLocalBounds().height ) ); 
	}

	void setWorld( Object* world ) { m_world = world; }

public:
	void init( const Pattern& pattern, int idx, Particle* parent );

	void kill() { m_dead = true; }
	bool isDead() const { return m_dead; }

private:
	Math::Vec2 processVelocitySet( const VelocitySet& set, Math::Vec2 current, Math::Vec2 position );
	void initializeVelocitySet( VelocitySet& set, Math::Vec2 position, Math::Vec2 offset );
	Math::Vec2 processPositionSet( const PositionSet& set, int idx, Math::Vec2 offset );

private:
	shared_ptr< sf::Shape > m_sprite;

	Pattern m_pattern;
	vector< shared_ptr< Emitter::Emitter > > m_emitters;

	microseconds m_duration{ std::numeric_limits< int32_t >::lowest() };

	float m_alpha{ 1.0f };
	bool m_dead{ false };
	Object* m_world{ nullptr };
	Particle* m_parentParticle{ nullptr };

	Math::Vec2 m_origin{ Math::Vec2( 0.5f, 0.5f ) };
	Math::Vec2 m_acceleration{ Math::Vec2( 0.f, 0.f ) };
	float m_spin{ 0.f };
};

//================================================================================

void init( Object* parent );

void spawn( Object* world, Pattern pattern, Math::Vec2 position = Math::Vec2(), Particle* parent = nullptr );

void cleanup();

void killAll();

size_t getRenderGroups();

vector< shared_ptr< Particle > > getAll();

//--------------------------------------------------------------------------------

}
}

//================================================================================