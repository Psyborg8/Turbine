//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "particle-pattern.h"
#include "random.h"
#include "mathtypes.h"
#include "json.h"

//================================================================================

namespace Gfx::Emitter { typedef struct Pattern Pattern; }

//================================================================================

namespace Gfx {
namespace Particle {

//--------------------------------------------------------------------------------

struct VelocitySet {
	enum Type {
		Direction,
		Value,
		Point,
		Player
	} type{ Type::Direction };

	// Direction
	struct Direction {
		Math::ValueSet< Math::Vec2 > direction{ Math::Vec2() };
		Math::ValueSet< float > power{ 0.f };

		// Fade
		struct Fade {
			Math::ValueSet< Math::Vec2 > target{ Math::Vec2() };
			Math::ValueSet< float > start{ 1.f };
			Math::ValueSet< float > end{ 0.f };
			bool direction{ false }, power{ false };
		} fade;
	} direction;

	// Value
	struct Value {
		Math::ValueSet< Math::Vec2 > value{ Math::Vec2() };

		// Fade
		struct Fade {
			Math::ValueSet< Math::Vec2 > target{ Math::Vec2() };
			bool x{ false }, y{ false };
		} fade;
	} value;

	// Point
	struct Point {
		Math::ValueSet< Math::Vec2 > point{ Math::Vec2() };
		Math::ValueSet< float > power{ 0.f };
		bool track{ false };

		// Fade
		struct Fade {
			Math::ValueSet< Math::Vec2 > target{ Math::Vec2() };
			Math::ValueSet< float > start{ 1.f };
			Math::ValueSet< float > end{ 0.f };
			bool point{ false }, power{ false };
		} fade;

		// Scale
		struct Scale {
			Math::ValueSet< float > factor{ 1.f };
			bool active{ false }, inverse{ true };
		} scale;
	} point;

	Math::Vec2 offset{ 0.f, 0.f };
	Math::Vec2 startValue{ 0.f, 0.f };
};

//--------------------------------------------------------------------------------

struct PositionSet {
	PositionSet() = default;
	PositionSet( Math::Vec2 def ) { position.min = def; position.max = def; }
	// Type
	enum Type {
		Point,
		Circle,
		Rectangle,
	} type{ Type::Point };

	Math::ValueSet< Math::Vec2 > position{ Math::Vec2() };

	struct Circle {
		Math::ValueSet< float > radius{ 64.f };
		bool fill{ true }, uniform{ false };
	} circle;

	struct Rectangle {
		Math::ValueSet< Math::Vec2 > size{ Math::Vec2( 128.f, 128.f ) };
		bool fill{ true }, uniform{ false };
	} rectangle;
};

//--------------------------------------------------------------------------------

struct Pattern {
	// Properties
	struct Properties {
		string name{ "New Pattern" };
		Math::ValueSet< int > lifetime{ 0 };
		Math::ValueSet< int > number{ 1 };
	} properties;

	// Shape
	struct Shape {
		// Type
		enum Type {
			Circle,
			Rect,
			Texture,
		} type{ Type::Circle };

		// Texture
		string texture = string( 256, '\0' );
		string shader = string( 256, '\0' );
		int priority{ 26 };

		// Size
		struct Size {
			Math::ValueSet< Math::Vec2 > size{ Math::Vec2( 1.f, 1.f ) };

			// Fade
			struct Fade {
				Math::ValueSet< float > start{ 1.f };
				Math::ValueSet< float > end{ 0.f };
				bool x{ false }, y{ false };
			} fade;
		} size;

		// Color
		struct Color {
			Math::ValueSet< Math::Color > color{ Colors::WHITE };

			// Fade
			struct Fade {
				Math::ValueSet< Math::Color > target{ Math::Color( 1.f, 1.f, 1.f, 0.f ) };
				bool r{ false }, g{ false }, b{ false }, a{ false };
			} fade;
		} color;

		// Outline
		struct Outline {
			// Color
			struct Color {
				Math::ValueSet< Math::Color > color{ Colors::WHITE };

				// Fade
				struct Fade {
					Math::ValueSet< Math::Color > target{ Math::Color( 1.f, 1.f, 1.f, 0.f ) };
					bool r{ false }, g{ false }, b{ false }, a{ false };
				} fade;
			} color;

			// Thickness
			struct Thickness {
				Math::ValueSet< float > thickness{ 0.f };

				struct Fade {
					Math::ValueSet< float > start{ 1.f };
					Math::ValueSet< float > end{ 0.f };
					bool active{ false };
				} fade;
			} thickness;
		} outline;

		// Origin
		struct Origin {
			PositionSet position{ Math::Vec2( .5f, .5f ) };
			VelocitySet velocity;
		} origin;
	} shape;

	// Physics
	struct Physics {
		// Position
		PositionSet position;

		// Velocity
		VelocitySet velocity;

		// Acceleration
		VelocitySet acceleration;

		// Rotation
		struct Rotation {
			Math::ValueSet< float > rotation{ 0.f };

			// Fade
			struct Fade {
				Math::ValueSet< float > target{ 360.f };
				bool active{ false };
			} fade;

			// Spin
			struct Spin {
				Math::ValueSet< float > spin{ 0.f };

				// Fade
				struct Fade {
					Math::ValueSet< float > start{ 1.f };
					Math::ValueSet< float > end{ 0.f };
					bool active{ false };
				} fade;
			} spin;
		} rotation;
	} physics;

	// Inheritance
	struct Inheritance {
		bool attach{ false };
		bool position{ true };
		bool velocity{ false };
		bool acceleration{ false };
		bool origin{ false };
		bool originVelocity{ false };
		bool rotation{ false };
		bool spin{ false };
		bool size{ false };
		bool color{ false };
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
			index = Random::getInt( 0, int( patterns.size() ) );
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
	struct Properties {
		string name = "New Emitter";
		Math::ValueSet< Math::Vec2 > position{ Math::Vec2() };

		// Activation
		struct Activation {
			enum Type {
				OnSpawn,
				Alpha,
				OnDestruction
			} type{ Type::OnSpawn };

			Math::ValueSet< float > start{ 1.f };
			Math::ValueSet< float > end{ 0.f };
		} activation;

		// Spawn Rate
		struct SpawnRate {
			Math::ValueSet< float > start{ 0.f };
			Math::ValueSet< float > end{ 0.f };
			bool fade{ false };
		} rate;
	} properties;

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