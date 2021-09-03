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
	string name;

	// Initial
	struct {
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
	struct {
		struct {
			ValueSet< float > start;
			ValueSet< float > end;
			bool x, y;	
		} velocity;

		struct {
			ValueSet< float > start;
			ValueSet< float > end;
			bool x, y;
		} acceleration;

		struct {
			ValueSet< float > start;
			ValueSet< float > end;
			bool active;
		} size;

		struct {
			ValueSet< Math::Color > target;
			bool r, g, b, a;
		} color;

	} fade;

	// Emitters
	struct Emitter {
		ValueSet< float > lifetime;
		ValueSet< float > velocity;
		ValueSet< float > acceleration;
		ValueSet< float > size;
		ValueSet< float > alpha;
		ValueSet< float > number;

		ValueSet< Math::Vec2 > position;

		struct {
			ValueSet< float > start;
			ValueSet< float > end;
		} rate;

		vector< Pattern > patterns;

		Timers::TimerID timer;
	};

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

	Math::Vec2 getSize() const override { return Math::Vec2( m_shape.getRadius(), m_shape.getRadius() ); } 

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