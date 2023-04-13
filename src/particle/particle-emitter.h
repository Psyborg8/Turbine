//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "sprite.h"
#include "shader.h"
#include "json.h"
#include "imgui-utils.h"

#include "particle.h"
#include "particle-manager.h"
#include "particle-affector-manager.h"

//================================================================================

namespace Gfx::Particle {

//--------------------------------------------------------------------------------

struct PatternSet {
	vector< ParticlePattern > patterns;

	ParticlePattern get() {
		if( patterns.empty() )
			return ParticlePattern();

		const int idx = Random::getInt( 0, ( int )patterns.size() - 1 );
		return patterns.at( idx );
	}

	rapidjson::Value getValue() {
		rapidjson::Value out;
		out.SetArray();
		for( ParticlePattern& pattern : patterns )
			out.PushBack( pattern.getValue(), json::getAllocator() );
		return out;
	}

	void setValue( const rapidjson::Value& value ) {
		if( !value.IsArray() )
			return;

		patterns.clear();

		for( const rapidjson::Value& pattern : value.GetArray() ) {
			ParticlePattern p;
			p.setValue( pattern );
			patterns.push_back( p );
		}
	}
};

//--------------------------------------------------------------------------------

struct PatternSequence {
	vector< ParticlePattern > patterns;
	size_t idx = 0u;

	ParticlePattern get() {
		if( patterns.empty() )
			return ParticlePattern();

		ParticlePattern out = patterns.at( idx );
		idx++;
		if( idx >= patterns.size() )
			idx = 0u;
		return out;
	}

	rapidjson::Value getValue() {
		rapidjson::Value out;
		out.SetArray();
		for( ParticlePattern& pattern : patterns )
			out.PushBack( pattern.getValue(), json::getAllocator() );
		return out;
	}

	void setValue( const rapidjson::Value& value ) {
		if( !value.IsArray() )
			return;

		patterns.clear();

		for( const rapidjson::Value& pattern : value.GetArray() ) {
			ParticlePattern p;
			p.setValue( pattern );
			patterns.push_back( p );
		}
	}
};

//--------------------------------------------------------------------------------

struct Emitter {
	struct {
		sf::Transformable transform;
		microseconds duration{ 0 };
		microseconds delay{ 0 };
		Math::ValueSet< float > spawnRate{ 0.f };
	} initial, current;

	microseconds remaining{ 0 };

	list< shared_ptr< Affector::Affector > > affectors;

	vector< ParticlePattern > patterns;
	vector< PatternSet > sets;
	vector< PatternSequence > sequences;

	std::list< Particle* > particles;

	bool active{ false };

	// System
	void update( sf::Time deltaTime ) {

		// Cleanup
		auto it = particles.begin();
		while( it != particles.end() ) {
			auto temp = it++;
			if( !( *temp )->alive )
				particles.erase( temp );
		}

		Affector::apply( this, deltaTime );

		if( !active )
			return;

		current.duration += microseconds( deltaTime.asMicroseconds() );
		current.delay -= microseconds( deltaTime.asMicroseconds() );

		if( current.delay < 0ms ) {
			spawn();

			Math::processSet( current.spawnRate );
			current.delay = microseconds( int( 1000000.f / current.spawnRate.value ) );
		}

		if( initial.duration >= 0ms ) {
			remaining -= microseconds( deltaTime.asMicroseconds() );
			if( remaining <= 0ms )
				active = false;
		}
	}

	void spawn() {
		for( ParticlePattern& pattern : patterns )
			Gfx::Particle::Manager::spawnParticle( pattern, this );

		for( PatternSet& set : sets ) {
			ParticlePattern pattern = set.get();
			Gfx::Particle::Manager::spawnParticle( pattern, this );
		}

		for( PatternSequence& sequence : sequences ) {
			ParticlePattern pattern = sequence.get();
			Gfx::Particle::Manager::spawnParticle( pattern, this );
		}
	}

	// Controls
	void start() {
		current.duration = 0ms;
		remaining = initial.duration;
		current.delay = initial.delay;
		current.transform = initial.transform;
		current.spawnRate = initial.spawnRate;

		active = true;
	}

	void stop() {
		active = false;
	}

	void kill() {
		for( Particle* particle : particles )
			particle->dead = true;

		particles.clear();
	}
};

//--------------------------------------------------------------------------------

struct EmitterPattern {
	string name = "New Emitter";

	Math::Vec2 position{ Math::Vec2() };
	float rotation{ 0.f };
	Math::Vec2 scale{ Math::Vec2( 1.f, 1.f ) };

	Math::ValueSet< int > duration{ 0 };
	Math::ValueSet< int > delay{ 0 };
	Math::ValueSet< float > rate{ 0.f };

	vector< shared_ptr< Affector::AffectorCreator > > affectors;

	vector< ParticlePattern > patterns;
	vector< PatternSet > sets;
	vector< PatternSequence > sequences;

	Emitter process() {
		Math::processSet( duration );
		Math::processSet( delay );

		Emitter out;
		out.initial.transform.move( position.sf() );
		out.initial.transform.rotate( rotation );
		out.initial.transform.scale( scale.sf() );

		out.initial.duration = milliseconds( duration.value );
		out.initial.delay = milliseconds( delay.value );

		out.initial.spawnRate = rate;

		out.patterns = patterns;
		out.sets = sets;
		out.sequences = sequences;

		for( shared_ptr< Affector::AffectorCreator > affector : affectors )
			out.affectors.push_back( affector->get() );

		return out;
	}

	rapidjson::Value getValue() {
		rapidjson::Value out;
		out.SetObject();

		out.AddMember( "position", json::getValue( position ), json::getAllocator() );
		out.AddMember( "rotation", json::getValue( rotation ), json::getAllocator() );
		out.AddMember( "scale", json::getValue( scale ), json::getAllocator() );

		out.AddMember( "duration", json::getValue( duration ), json::getAllocator() );
		out.AddMember( "delay", json::getValue( delay ), json::getAllocator() );
		out.AddMember( "rate", json::getValue( rate ), json::getAllocator() );

		// Affectors

		{
			rapidjson::Value vPatterns;
			vPatterns.SetArray();
			for( ParticlePattern& pattern : patterns )
				vPatterns.PushBack( pattern.getValue(), json::getAllocator() );
			out.AddMember( "patterns", vPatterns, json::getAllocator() );
		}
		{
			rapidjson::Value vSets;
			vSets.SetArray();
			for( PatternSet set : sets )
				vSets.PushBack( set.getValue(), json::getAllocator() );
			out.AddMember( "sets", vSets, json::getAllocator() );
		}
		{
			rapidjson::Value vSequences;
			vSequences.SetArray();
			for( PatternSequence sequence : sequences )
				vSequences.PushBack( sequence.getValue(), json::getAllocator() );
			out.AddMember( "sequences", vSequences, json::getAllocator() );
		}

		return out;
	}

	void setValue( const rapidjson::Value& value ) {
		if( !value.IsObject() )
			return;

		if( value.HasMember( "position" ) )
			json::getValue( value[ "position" ], position );
		if( value.HasMember( "rotation" ) )
			json::getValue( value[ "rotation" ], rotation );
		if( value.HasMember( "scale" ) )
			json::getValue( value[ "scale" ], scale );
		if( value.HasMember( "duration" ) )
			json::getValue( value[ "duration" ], duration );
		if( value.HasMember( "delay" ) )
			json::getValue( value[ "delay" ], delay );
		if( value.HasMember( "rate" ) )
			json::getValue( value[ "rate" ], rate );

		// Affectors

		if( value.HasMember( "patterns" ) && value[ "patterns" ].IsArray() ) {
			for( const rapidjson::Value& pattern : value[ "patterns" ].GetArray() ) {
				ParticlePattern p;
				p.setValue( pattern );
				patterns.push_back( p );
			}
		}

		if( value.HasMember( "sets" ) && value[ "sets" ].IsArray() ) {
			for( const rapidjson::Value& set : value[ "sets" ].GetArray() ) {
				PatternSet s;
				s.setValue( set );
				sets.push_back( s );
			}
		}

		if( value.HasMember( "sequences" ) && value[ "sequences" ].IsArray() ) {
			for( const rapidjson::Value& sequence : value[ "sequences" ].GetArray() ) {
				PatternSequence s;
				s.setValue( sequence );
				sequences.push_back( s );
			}
		}
	}
};

//--------------------------------------------------------------------------------

}

//================================================================================