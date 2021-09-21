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

	struct Shape {
		enum Type {
			Circle,
			Rect,
			Texture,
		} type{ Type::Texture };

		Math::ValueSet< Math::Color > color{ Colors::WHITE };

		Math::ValueSet< Math::Color > outlineColor{ Colors::WHITE };
		Math::ValueSet< float > outlineThickness{ .0f };

		string texture{ "circle_05" };
		string shader{ "default" };

		Math::ValueSet < Math::Vec2 > size{ Math::Vec2( 1.f, 1.f ) };
		Math::ValueSet< float > rotation{ .0f };
		Math::ValueSet< float > spin{ .0f };
	} shape;

	// Initial
	struct Initial {
		Math::ValueSet< int > lifetime{ 1000 };
		Math::ValueSet< Math::Vec2 > position{ Math::Vec2() };
		Math::ValueSet< Math::Vec2 > direction{ Math::Vec2() };
		Math::ValueSet< float > velocity{ .0f };
		Math::ValueSet< Math::Vec2 > acceleration{ Math::Vec2() };
		Math::ValueSet< int > number{ 1 };
	} initial;

	// Fades
	struct Fade {
		struct Velocity {
			Math::ValueSet< float > start{ 1.f };
			Math::ValueSet< float > end{ .0f };
			bool x{ false }, y{ false };
		} velocity;

		struct Acceleration {
			Math::ValueSet< float > start{ 1.f };
			Math::ValueSet< float > end{ .0f };
			bool x{ false }, y{ false };
		} acceleration;

		struct Size {
			Math::ValueSet< float > start{ 1.f };
			Math::ValueSet< float > end{ .0f };
			bool active{ false };
		} size;

		struct Color {
			Math::ValueSet< Math::Color > target{ Math::Color( 1.f, 1.f, 1.f, .0f ) };
			bool r{ false }, g{ false }, b{ false }, a{ false };
		} color;

		struct Spin {
			Math::ValueSet< float > start{ 1.f };
			Math::ValueSet< float > end{ .0f };
			bool active{ false };
		};

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
	Math::ValueSet< Math::Vec2 > position{ Math::Vec2() };

	// Activation
	struct Activation {
		enum class Type {
			OnSpawn,
			Alpha,
			OnDestruction
		} type{ Type::OnSpawn };

		Math::ValueSet< float > start{ 1.f };
		Math::ValueSet< float > end{ 0.f };
	} activation;

	// Spawn Rate
	struct SpawnRate {
		Math::ValueSet< float > start{ 1.f };
		Math::ValueSet< float > end{ 0.f };
		bool fade{ false };
	} rate;

	// Multpliers
	struct {
		Math::ValueSet< float > lifetime{ 1.f };
		Math::ValueSet< float > velocity{ 1.f };
		Math::ValueSet< float > acceleration{ 1.f };
		Math::ValueSet< float > size{ 1.f };
		Math::ValueSet< float > alpha{ 1.f };
		Math::ValueSet< float > number{ 1.f };
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