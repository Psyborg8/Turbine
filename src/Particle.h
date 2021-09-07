//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "Object.h"
#include "Random.h"
#include "Timer.h"
#include "RigidRect.h"

//================================================================================

namespace Gfx {
namespace Particle {

//--------------------------------------------------------------------------------

template< class T >
struct ValueSet {
	T min;
	T max;
	T value;

	bool random;
};

//--------------------------------------------------------------------------------

struct Pattern {
	string name{ "New Pattern" };

	// Initial
	struct Initial {
		ValueSet< int > lifetime;
		ValueSet< Math::Vec2 > position;
		ValueSet< Math::Vec2 > direction;
		ValueSet< float > velocity;
		ValueSet< Math::Vec2 > acceleration;
		ValueSet< float > size;
		ValueSet< int > number;
		ValueSet< Math::Color > color;
	} initial;

	// Fades
	struct Fade {
		struct Velocity {
			ValueSet< float > start;
			ValueSet< float > end;
			bool x, y;	
		} velocity;

		struct Acceleration {
			ValueSet< float > start;
			ValueSet< float > end;
			bool x, y;
		} acceleration;

		struct Size {
			ValueSet< float > start;
			ValueSet< float > end;
			bool active;
		} size;

		struct Color {
			ValueSet< Math::Color > target;
			bool r, g, b, a;
		} color;

	} fade;

	// Emitters
	struct Emitter {
		string name;

		ValueSet< float > lifetime;
		ValueSet< float > velocity;
		ValueSet< float > acceleration;
		ValueSet< float > size;
		ValueSet< float > alpha;
		ValueSet< float > number;

		ValueSet< Math::Vec2 > position;
		ValueSet< float > startTime;

		struct SpawnRate {
			ValueSet< float > start;
			ValueSet< float > end;
			bool fade{ false };
		} rate;

		vector< Pattern > patterns;

		Timers::TimerID timer;
		bool active{ true };
	};

	vector< Emitter > emitters;
};

//================================================================================

class Particle : public Object {
public:
	Particle() = default;

public:
	void onUpdate( sf::Time deltaTime ) override;
	void onPostUpdate( sf::Time deltaTime ) override;
	void onRender() override;
	void onDestroy() override;

	void setSize( Math::Vec2 size ) override { setSize( size.x ); }
	void setSize( float radius ) { m_shape.setRadius( radius ); }

	Math::Vec2 getSize() const override { return Math::Vec2( m_shape.getRadius(), m_shape.getRadius() ); }

	void deactivate() { m_active = false; }
	bool isActive() const { return m_active; }

public:
	void init( const Pattern& pattern );

private:
	sf::CircleShape m_shape;
	Timers::TimerID m_timer;
	Pattern m_pattern;

	float m_alpha{ .0f };

	bool m_active{ true };
};

//================================================================================

void spawnParticle( Object* parent, Pattern pattern );
void spawnParticle( Object* parent, Pattern pattern, Math::Vec2 position );

void killParticles();

void cleanup();

vector< shared_ptr< Particle > > getParticles();

Pattern loadPattern( string filepath );
void savePattern( Pattern pattern, string filepath );

//--------------------------------------------------------------------------------

}
}

//================================================================================