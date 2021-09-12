//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "Patterns.h"
#include "Random.h"
#include "MathTypes.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

namespace Emitter {
typedef struct Pattern Pattern;
}

//--------------------------------------------------------------------------------

namespace Particle {

//--------------------------------------------------------------------------------

struct Pattern {
	string name{ "New Pattern" };

	// Initial
	struct Initial {
		Math::ValueSet< int > lifetime;
		Math::ValueSet< Math::Vec2 > position;
		Math::ValueSet< Math::Vec2 > direction;
		Math::ValueSet< float > velocity;
		Math::ValueSet< Math::Vec2 > acceleration;
		Math::ValueSet< float > size;
		Math::ValueSet< int > number;
		Math::ValueSet< Math::Color > color;
	} initial;

	// Fades
	struct Fade {
		struct Velocity {
			Math::ValueSet< float > start;
			Math::ValueSet< float > end;
			bool x, y;
		} velocity;

		struct Acceleration {
			Math::ValueSet< float > start;
			Math::ValueSet< float > end;
			bool x, y;
		} acceleration;

		struct Size {
			Math::ValueSet< float > start;
			Math::ValueSet< float > end;
			bool active;
		} size;

		struct Color {
			Math::ValueSet< Math::Color > target;
			bool r, g, b, a;
		} color;

	} fade;

	// Inheritance
	struct {
		bool position{ true };
		bool velocity{ false };
		bool size{ false };
		bool alpha{ false };
	} inheritance;

	vector< Emitter::Pattern > emitters;
};

//--------------------------------------------------------------------------------

// Pattern Set
struct PatternSet {
	vector< Pattern > patterns;
	size_t index{ 0 };
	bool random{ false };

	Pattern inc() {
		if( patterns.empty() )
			return Pattern();

		if( random ) {
			index = Random::getRandomIntInRange( 0, int( patterns.size() ) );
			return patterns.at( index );
		}

		Pattern out = patterns.at( index++ );
		if( index >= patterns.size() )
			index = 0u;
		return out;
	}
};

//--------------------------------------------------------------------------------

}
namespace Emitter {

//================================================================================

struct Pattern {
	// Properties
	string name = "New Emitter";
	Math::ValueSet< Math::Vec2 > position;

	// Activation
	struct Activation {
		enum class Type {
			OnSpawn,
			Alpha,
			OnDestruction
		} type{ Type::OnSpawn };

		Math::ValueSet< float > start;
		Math::ValueSet< float > end;
	} activation;

	// Spawn Rate
	struct SpawnRate {
		Math::ValueSet< float > start;
		Math::ValueSet< float > end;
		bool fade{ false };
	} rate;

	// Multpliers
	struct {
		Math::ValueSet< float > lifetime;
		Math::ValueSet< float > velocity;
		Math::ValueSet< float > acceleration;
		Math::ValueSet< float > size;
		Math::ValueSet< float > alpha;
		Math::ValueSet< float > number;
	} multipliers;

	// Pattern
	vector< Particle::Pattern > patterns;
	vector< Particle::PatternSet > sets;
};

//--------------------------------------------------------------------------------

}

//================================================================================

Particle::Pattern loadPattern( string name );
void savePattern( Particle::Pattern pattern, string path );

Emitter::Pattern loadEmitter( string name );
void saveEmitter( Emitter::Pattern pattern, string path );

//--------------------------------------------------------------------------------

}

//--------------------------------------------------------------------------------