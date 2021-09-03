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
};

template< uint16_t n >
struct FadeSet {
	ValueSet< float, n > start;
	ValueSet< float, n > end;
	array< bool, n > active;
};

//--------------------------------------------------------------------------------

struct Emitter {
	ValueSet< float > lifetime;
	ValueSet< float > velocity;
	ValueSet< float > size;
	ValueSet< float > alpha;
	ValueSet< float > number;

	ValueSet< Math::Vec2 > position;

	FadeSet< 1u > spawnRate;

	vector< Pattern > patterns;

	Timers::TimerID spawnTimer;
};

//--------------------------------------------------------------------------------

struct Pattern {
	string name;

	struct {
		ValueSet< int > lifetime;
		ValueSet< float > velocity;
		ValueSet< Math::Vec2 > acceleration;
		ValueSet< Math::Vec2 > direction;
		ValueSet< float > size;
		ValueSet< int > number;
		ValueSet< Math::Color > color;
	} initial;

	struct {
		FadeSet< 2u > velocity;
		FadeSet< 2u > acceleration;
		FadeSet< 1u > size;
		FadeSet< 4u > color;
	} fade;

	vector< pair< Emitter, float > > emitters;
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

	Math::Vec2 getSize() const override { return Math::Vec2( m_shape.getRadius(), 0.0f ); } 

public:
	void init( const Pattern& pattern );

private:
	sf::CircleShape m_shape;
	Timers::TimerID m_timer;
	Pattern m_pattern;

	float m_alpha{ .0f };
};

//================================================================================

void spawnParticle( Object* parent, Pattern pattern );
void spawnParticle( Object* parent, Pattern pattern, Math::Vec2 position );

// Pattern loadPattern( string filepath );

// void savePattern( Pattern pattern, string filepath );

//--------------------------------------------------------------------------------

}
}

//================================================================================