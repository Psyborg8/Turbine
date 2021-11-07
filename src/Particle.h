//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "sprite.h"
#include "random.h"
#include "imgui-utils.h"
#include "string-utils.h"
#include "json.h"
#include "system.h"
#include "editor.h"

#include "particle-affector-manager.h"
#include "particle-affector.h"
#include "particle-loader.h"

//================================================================================

namespace Gfx::Particle {

struct Emitter;
class System;

//--------------------------------------------------------------------------------

struct ParticleProperties {
	sf::Transformable transform{};

	Math::Vec2 velocity{ Math::Vec2() };
	Math::Vec2 acceleration{ Math::Vec2() };
	float spin{ 0.f };

	Math::Color color{ Colors::WHITE };

	microseconds duration{ 0ms };
	microseconds remaining{ 0ms };
};

//--------------------------------------------------------------------------------

struct Particle {
	ParticleProperties initial;
	ParticleProperties current;
	ParticleProperties frame;

	list< shared_ptr< Affector::Affector > > affectors;
	size_t group{ std::numeric_limits< size_t >::max() };

	Particle* parent;
	Emitter* emitter;

	bool dead{ false };
	bool alive{ false };
	bool fresh{ true };
};

//--------------------------------------------------------------------------------

struct RenderProperties {
	Sprite::ID texture{ 0u };
	int priority{ 0 };

	bool operator==( const RenderProperties& rh ) {
		return rh.priority == priority
			&& rh.texture == texture;
	}
};

//--------------------------------------------------------------------------------

struct Inheritance {
	struct Value {
		Value( InheritanceType _type ) : type( _type ){}

		InheritanceType type;
		bool attach{ false };

		rapidjson::Value getValue() {
			rapidjson::Value out;
			out.SetObject();

			out.AddMember( "type", json::getValue( ( int& )type ), json::getAllocator() );
			out.AddMember( "attach", json::getValue( attach ), json::getAllocator() );

			return out;
		}

		void setValue( const rapidjson::Value& value ) {
			if( !value.IsObject() )
				return;

			if( value.HasMember( "type" ) )
				json::getValue( value[ "type" ], ( int& )type );
			if( value.HasMember( "attach" ) )
				json::getValue( value[ "attach" ], attach );
		}

		bool render( string name ) {
			bool out = false;

			ImGui::PushID( name.c_str() );

			string typeString;
			switch( type ) {
			case InheritanceType::None: typeString = "None"; break;
			case InheritanceType::Copy: typeString = "Copy"; break;
			case InheritanceType::Offset: typeString = "Offset"; break;
			case InheritanceType::Multiply: typeString = "Multiply"; break;
			}

			ImGui::Text( name.c_str() );

			if( ImGui::Button( Utils::format( "Type: %s", typeString ).c_str() ) )
				ImGui::OpenPopup( name.c_str() );

			if( ImGui::BeginPopup( name.c_str(), ImGuiWindowFlags_NoDecoration ) ) {
				if( ImGui::Selectable( "None" ) ) {
					type = InheritanceType::None;
					out = true;
					ImGui::CloseCurrentPopup();
				}
				if( ImGui::Selectable( "Copy" ) ) {
					type = InheritanceType::Copy;
					out = true;
					ImGui::CloseCurrentPopup();
				}
				if( ImGui::Selectable( "Offset" ) ) {
					type = InheritanceType::Offset;
					out = true;
					ImGui::CloseCurrentPopup();
				}
				if( ImGui::Selectable( "Multiply" ) ) {
					type = InheritanceType::Multiply;
					out = true;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			out |= ImGui::Checkbox( "Attach", &attach );

			ImGui::PopID();

			return out;
		}
	};

	Value lifetime{ InheritanceType::None };
	Value position{ InheritanceType::Offset };
	Value rotation{ InheritanceType::None };
	Value spin{ InheritanceType::None };
	Value scale{ InheritanceType::None };
	Value color{ InheritanceType::None };
	Value alpha{ InheritanceType::None };
	Value velocity{ InheritanceType::None };
	Value acceleration{ InheritanceType::None };
	bool scaleVelocity{ false };
	bool rotateVelocity{ false };

	rapidjson::Value getValue() {
		rapidjson::Value out;
		out.SetObject();
		out.AddMember( "lifetime", lifetime.getValue(), json::getAllocator() );
		out.AddMember( "position", position.getValue(), json::getAllocator() );
		out.AddMember( "rotation", rotation.getValue(), json::getAllocator() );
		out.AddMember( "spin", spin.getValue(), json::getAllocator() );
		out.AddMember( "scale", scale.getValue(), json::getAllocator() );
		out.AddMember( "color", color.getValue(), json::getAllocator() );
		out.AddMember( "alpha", alpha.getValue(), json::getAllocator() );
		out.AddMember( "velocity", velocity.getValue(), json::getAllocator() );
		out.AddMember( "acceleration", acceleration.getValue(), json::getAllocator() );
		out.AddMember( "scaleVelocity", json::getValue( scaleVelocity ), json::getAllocator() );
		out.AddMember( "rotateVelocity", json::getValue( rotateVelocity ), json::getAllocator() );

		return out;
	}

	void setValue( const rapidjson::Value& value ) {
		if( !value.IsObject() )
			return;

		if( value.HasMember( "lifetime" ) )
			lifetime.setValue( value[ "lifetime" ] );
		if( value.HasMember( "position" ) )
			position.setValue( value[ "position" ] );
		if( value.HasMember( "rotation" ) )
			rotation.setValue( value[ "rotation" ] );
		if( value.HasMember( "spin" ) )
			spin.setValue( value[ "spin" ] );
		if( value.HasMember( "scale" ) )
			scale.setValue( value[ "scale" ] );
		if( value.HasMember( "color" ) )
			color.setValue( value[ "color" ] );
		if( value.HasMember( "alpha" ) )
			alpha.setValue( value[ "alpha" ] );
		if( value.HasMember( "velocity" ) )
			velocity.setValue( value[ "velocity" ] );
		if( value.HasMember( "acceleration" ) )
			acceleration.setValue( value[ "acceleration" ] );
		if( value.HasMember( "scaleVelocity" ) )
			json::getValue( value[ "scaleVelocity" ], scaleVelocity );
		if( value.HasMember( "rotateVelocity" ) )
			json::getValue( value[ "rotateVelocity" ], rotateVelocity );
	}

	bool render() {
		bool out = false;
		ImGui::PushID( "Inheritance" );
		out |= lifetime.render( "Lifetime" );
		ImGui::Separator();
		out |= position.render( "Position" );
		ImGui::Separator();
		out |= rotation.render( "Rotation" );
		ImGui::Separator();
		out |= spin.render( "Spin" );
		ImGui::Separator();
		out |= scale.render( "Scale" );
		ImGui::Separator();
		out |= color.render( "Color" );
		ImGui::Separator();
		out |= alpha.render( "Alpha" );
		ImGui::Separator();
		out |= velocity.render( "Velocity" );
		out |= ImGui::Checkbox( "Scale", &scaleVelocity );
		out |= ImGui::Checkbox( "Rotate", &rotateVelocity );
		ImGui::Separator();
		out |= acceleration.render( "Acceleration" );
		ImGui::PopID();
		return out;
	}
};

//--------------------------------------------------------------------------------

struct ParticleEmitter {
	EmitterType type;

	Math::ValueSet< int > duration{ 0.f };
	Math::ValueSet< int > delay{ 0.f };
	Math::ValueSet< float > rate{ 0.f };

	vector< string > patterns;

	bool onDeath{ false };

	size_t selectorIdx{ 0 };
	bool selectorOpen{ false };

	shared_ptr< Affector::Affector > process() {
		vector< ParticlePattern* > vPatterns;
		for( string pattern : patterns )
			vPatterns.push_back( Loader::get( pattern ) );

		if( onDeath )
			return make_shared< Affector::OnDeathEmitterAffector >( type, vPatterns );
		else {
			Math::processSet( duration );
			Math::processSet( delay );
			
			return make_shared< Affector::EmitterAffector >( type, milliseconds( delay.value ), milliseconds( duration.value ), rate, vPatterns );
		}
	}

	rapidjson::Value getValue() {
		rapidjson::Value out;
		out.SetObject();

		out.AddMember( "type", json::getValue( ( int& )type ), json::getAllocator() );
		out.AddMember( "duration", json::getValue( duration ), json::getAllocator() );
		out.AddMember( "delay", json::getValue( delay ), json::getAllocator() );
		out.AddMember( "rate", json::getValue( rate ), json::getAllocator() );
		out.AddMember( "onDeath", json::getValue( onDeath ), json::getAllocator() );

		rapidjson::Value vPatterns;
		vPatterns.SetArray();
		for( string pattern : patterns )
			vPatterns.PushBack( json::getValue( pattern ), json::getAllocator() );

		out.AddMember( "patterns", vPatterns, json::getAllocator() );

		return out;
	}

	void setValue( const rapidjson::Value& value ) {
		if( !value.IsObject() )
			return;

		if( value.HasMember( "type" ) )
			json::getValue( value[ "type" ], ( int& )type );
		if( value.HasMember( "duration" ) )
			json::getValue( value[ "duration" ], duration );
		if( value.HasMember( "delay" ) )
			json::getValue( value[ "delay" ], delay );
		if( value.HasMember( "rate" ) )
			json::getValue( value[ "rate" ], rate );
		if( value.HasMember( "onDeath" ) )
			json::getValue( value[ "onDeath" ], onDeath );

		if( value.HasMember( "patterns" ) && value[ "patterns" ].IsArray() ) {
			for( const rapidjson::Value& pattern : value[ "patterns" ].GetArray() ) {
				string s;
				json::getValue( pattern, s );
				patterns.push_back( s );
			}
		}
	}

	bool render() {
		bool out = false;

		string typeString;
		switch( type ) {
		case EmitterType::Set: typeString = "Type: Set"; break;
		case EmitterType::Random: typeString = "Type: Random"; break;
		case EmitterType::Sequence: typeString = "Type: Sequence"; break;
		}

		if( ImGui::Button( typeString.c_str() ) )
			ImGui::OpenPopup( "Type Popup" );

		if( ImGui::BeginPopup( "Type Popup", ImGuiWindowFlags_NoDecoration ) ) {
			if( ImGui::Selectable( "Set" ) ) {
				type = EmitterType::Set;
				out = true;
				ImGui::CloseCurrentPopup();
			}
			if( ImGui::Selectable( "Random" ) ) {
				type = EmitterType::Random;
				out = true;
				ImGui::CloseCurrentPopup();
			}
			if( !onDeath && ImGui::Selectable( "Sequence" ) ) {
				type = EmitterType::Sequence;
				out = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if( ImGui::Checkbox( "On Death", &onDeath ) ) {
			out = true;
			if( onDeath && type == EmitterType::Sequence )
				type = EmitterType::Set;
		}

		if( !onDeath ) {
			out |= ImGui::render( duration, "Duration" );
			out |= ImGui::render( delay, "Delay" );
			out |= ImGui::render( rate, "Rate" );
		}

		ImGui::Separator();
		ImGui::Text( "Patterns" );
		ImGui::Separator();
		for( size_t i = 0; i < patterns.size(); ++i ) {
			ImGui::PushID( ( int )i );

			vector< string > tokens = Utils::tokenize( patterns.at( i ), Folders::Bullets, true );
			string name = tokens.size() > 0 ? tokens.at( 0 ) : patterns.at( i );
			ImGui::Text( name.c_str() );

			if( ImGui::Button( "Change" ) ) {
				selectorIdx = i;
				selectorOpen = true;
			}
			ImGui::SameLine();
			if( ImGui::Button( "Remove" ) ) {
				out = true;
				patterns.erase( patterns.begin() + i );
				i--;
			}
			ImGui::SameLine();
			if( ImGui::Button( "Open" ) ) {
				shared_ptr< Editor::Editor > editor = std::dynamic_pointer_cast< Editor::Editor >( ::System::getWorld() );
				if( editor != nullptr )
					editor->openParticle( patterns.at( i ) );
			}

			if( i < patterns.size() - 1u && ImGui::Button( "Move Down" ) ) {
				std::iter_swap( patterns.begin() + i, patterns.begin() + i + 1u );
				ImGui::SameLine();
			}
			if( i > 0 && ImGui::Button( "Move Up" ) )
				std::iter_swap( patterns.begin() + i, patterns.begin() + i - 1u );

			ImGui::Separator();

			ImGui::PopID();
		}

		if( selectorOpen && selectorIdx < patterns.size() ) {
			if( ImGui::renderPatternSelector( patterns.at( selectorIdx ) ) ) {
				selectorOpen = false;
				out = true;
			}
		}

		if( ImGui::Button( "Add Pattern" ) ) {
			patterns.push_back( string() );
			selectorOpen = true;
			selectorIdx = patterns.size() - 1u;
		}

		return out;
	}
};

//--------------------------------------------------------------------------------

struct ParticlePattern {
	string name = "New Pattern";

	Math::ValueSet< int > lifetime{ 0 };
	Math::ValueSet< int > number{ 0 };
	Math::ValueSet< Math::Color > color{ Colors::WHITE };
	Math::ValueSet< float > rotation{ 0.f };
	Math::ValueSet< Math::Vec2 > scale{ Math::Vec2() };
	Math::ValueSet< float > spin{ 0.f };

	string texture{ "./Data/Assets/Particles/default.png" };
	int priority{ 0 };

	Math::Position position{};
	Math::Velocity velocity{};
	Math::Velocity acceleration{};

	Inheritance inheritance;

	list< shared_ptr< Affector::AffectorCreator > > affectors;
	vector< ParticleEmitter > emitters;

	Particle process( int index = 0, int total = 0 ) {
		Math::processSet( lifetime );
		Math::processSet( color );
		Math::processSet( scale );
		Math::processSet( rotation );
		Math::processSet( spin );

		ParticleProperties properties;
		properties.color = color.value;
		properties.remaining = milliseconds( lifetime.value );

		Math::Vec2 pos = position.process( index, total );
		properties.transform.setOrigin( sf::Vector2f( .5f, .5f ) );
		properties.transform.setPosition( pos.sf() );
		properties.transform.setScale( scale.value.sf() );
		properties.transform.setRotation( rotation.value );
		properties.spin = spin.value;

		properties.velocity = velocity.process( pos );
		properties.acceleration = acceleration.process( pos );

		Particle out;
		out.initial = properties;
		out.current = properties;
		out.frame   = properties;

		for( shared_ptr< Affector::AffectorCreator > affector : affectors )
			out.affectors.push_back( affector->get() );

		for( ParticleEmitter& emitter : emitters )
			out.affectors.push_back( emitter.process() );

		return out;
	}

	Particle process( Particle* parent, int index = 0, int total = 0 ) {
		if( parent == nullptr )
			return process( index, total );

		Particle out = process( index, total );
		out.parent = parent;
		if( parent->emitter != nullptr )
			out.emitter = parent->emitter;

		// Inheritance

		// Lifetime
		if( !inheritance.lifetime.attach ) {
			switch( inheritance.lifetime.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.remaining = parent->initial.remaining; break;
			case InheritanceType::Offset: out.initial.remaining += parent->initial.remaining; break;
			case InheritanceType::Multiply: out.initial.remaining = microseconds( out.initial.remaining.count() * parent->initial.remaining.count() ); break;
			}
		}

		// Position
		if( !inheritance.position.attach ) {
			switch( inheritance.position.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.transform.setPosition( parent->frame.transform.getPosition() ); break;
			case InheritanceType::Offset: out.initial.transform.move( parent->frame.transform.getPosition() ); break;
			case InheritanceType::Multiply:
				Math::Vec2 pos = out.initial.transform.getPosition();
				pos *= parent->frame.transform.getPosition();
				out.initial.transform.setPosition( pos.sf() );
				break;
			}
		}

		// Rotation
		if( !inheritance.rotation.attach ) {
			switch( inheritance.rotation.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.transform.setRotation( parent->frame.transform.getRotation() ); break;
			case InheritanceType::Offset: out.initial.transform.rotate( parent->frame.transform.getRotation() ); break;
			case InheritanceType::Multiply:
				float rot = out.initial.transform.getRotation();
				rot *= parent->frame.transform.getRotation();
				out.initial.transform.setRotation( rot );
			}
		}

		// Spin
		if( !inheritance.spin.attach ) {
			switch( inheritance.spin.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.spin = parent->frame.spin; break;
			case InheritanceType::Offset: out.initial.spin += parent->frame.spin; break;
			case InheritanceType::Multiply: out.initial.spin *= parent->frame.spin; break;
			}
		}

		// Scale
		if( !inheritance.scale.attach ) {
			switch( inheritance.scale.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.transform.setScale( parent->frame.transform.getScale() ); break;
			case InheritanceType::Offset: out.initial.transform.scale( parent->frame.transform.getScale() ); break;
			case InheritanceType::Multiply:
				Math::Vec2 s = out.initial.transform.getScale();
				s *= parent->frame.transform.getScale();
				out.initial.transform.setScale( s.sf() );
				break;
			}
		}

		// Color
		if( !inheritance.color.attach ) {
			switch( inheritance.color.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy:
				out.initial.color.r = parent->frame.color.r;
				out.initial.color.g = parent->frame.color.g;
				out.initial.color.b = parent->frame.color.b;
				break;
			case InheritanceType::Offset:
				out.initial.color.r += parent->frame.color.r;
				out.initial.color.g += parent->frame.color.g;
				out.initial.color.b += parent->frame.color.b;
				break;
			case InheritanceType::Multiply:
				out.initial.color.r *= parent->frame.color.r;
				out.initial.color.g *= parent->frame.color.g;
				out.initial.color.b *= parent->frame.color.b;
				break;
			}
		}

		// Alpha
		if( !inheritance.alpha.attach ) {
			switch( inheritance.alpha.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.color.a = parent->frame.color.a; break;
			case InheritanceType::Offset: out.initial.color.a += parent->frame.color.a; break;
			case InheritanceType::Multiply: out.initial.color.a *= parent->frame.color.a; break;
			}
		}

		// Velocity
		if( !inheritance.velocity.attach ) {
			switch( inheritance.velocity.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.velocity = parent->frame.velocity; break;
			case InheritanceType::Offset: out.initial.velocity += parent->frame.velocity; break;
			case InheritanceType::Multiply: out.initial.velocity *= parent->frame.velocity; break;
			}

			if( inheritance.scaleVelocity ) {
				sf::Transform t;
				t.scale( parent->frame.transform.getScale() );
				out.initial.velocity = t.transformPoint( out.initial.velocity.sf() );
			}
			if( inheritance.rotateVelocity ) {
				sf::Transform t;
				t.rotate( parent->frame.transform.getRotation() );
				out.initial.velocity = t.transformPoint( out.initial.velocity.sf() );
			}
		}

		// Acceleration
		if( !inheritance.acceleration.attach ) {
			switch( inheritance.acceleration.type ) {
			case InheritanceType::None: break;
			case InheritanceType::Copy: out.initial.acceleration = parent->frame.acceleration; break;
			case InheritanceType::Offset: out.initial.acceleration += parent->frame.acceleration; break;
			case InheritanceType::Multiply: out.initial.acceleration *= parent->frame.acceleration; break;
			}
		}

		if( inheritance.lifetime.attach )
			out.affectors.push_back( make_shared< Affector::InheritLifetimeAffector >( inheritance.lifetime.type ) );
		if( inheritance.position.attach )
			out.affectors.push_back( make_shared< Affector::InheritPositionAffector >( inheritance.position.type ) );
		if( inheritance.rotation.attach )
			out.affectors.push_back( make_shared< Affector::InheritRotationAffector >( inheritance.rotation.type ) );
		if( inheritance.spin.attach )
			out.affectors.push_back( make_shared< Affector::InheritSpinAffector >( inheritance.spin.type ) );
		if( inheritance.scale.attach )
			out.affectors.push_back( make_shared< Affector::InheritScaleAffector >( inheritance.scale.type ) );
		if( inheritance.color.attach )
			out.affectors.push_back( make_shared< Affector::InheritColorAffector >( inheritance.color.type ) );
		if( inheritance.alpha.attach )
			out.affectors.push_back( make_shared< Affector::InheritAlphaAffector >( inheritance.alpha.type ) );
		if( inheritance.velocity.attach )
			out.affectors.push_back( make_shared< Affector::InheritVelocityAffector >( inheritance.velocity.type, inheritance.scaleVelocity, inheritance.rotateVelocity ) );
		if( inheritance.acceleration.attach )
			out.affectors.push_back( make_shared< Affector::InheritAccelerationAffector >( inheritance.acceleration.type ) );


		out.current = out.initial;
		out.frame   = out.initial;

		return out;
	}

	rapidjson::Value getValue() {
		rapidjson::Value out;
		out.SetObject();

		out.AddMember( "name", json::getValue( name ), json::getAllocator() );
		out.AddMember( "lifetime", json::getValue( lifetime ), json::getAllocator() );
		out.AddMember( "number", json::getValue( number ), json::getAllocator() );
		out.AddMember( "color", json::getValue( color ), json::getAllocator() );
		out.AddMember( "rotation", json::getValue( rotation ), json::getAllocator() );
		out.AddMember( "scale", json::getValue( scale ), json::getAllocator() );
		out.AddMember( "position", position.getValue(), json::getAllocator() );
		out.AddMember( "velocity", velocity.getValue(), json::getAllocator() );
		out.AddMember( "acceleration", acceleration.getValue(), json::getAllocator() );
		out.AddMember( "spin", json::getValue( spin ), json::getAllocator() );
		out.AddMember( "inheritance", inheritance.getValue(), json::getAllocator() );
		out.AddMember( "texture", json::getValue( texture ), json::getAllocator() );
		out.AddMember( "priority", json::getValue( priority ), json::getAllocator() );

		rapidjson::Value vAffectors;
		vAffectors.SetArray();
		for( shared_ptr< Affector::AffectorCreator > affector : affectors )
			vAffectors.PushBack( Affector::getValue( affector ), json::getAllocator() );
		out.AddMember( "affectors", vAffectors, json::getAllocator() );

		rapidjson::Value vEmitters;
		vEmitters.SetArray();
		for( ParticleEmitter emitter : emitters )
			vEmitters.PushBack( emitter.getValue(), json::getAllocator() );
		out.AddMember( "emitters", vEmitters, json::getAllocator() );

		return out;
	}

	bool setValue( const rapidjson::Value& v ) {
		if( !v.IsObject() )
			return false;

		if( v.HasMember( "name" ) )
			json::getValue( v[ "name" ], name );
		if( v.HasMember( "lifetime" ) )
			json::getValue( v[ "lifetime" ], lifetime );
		if( v.HasMember( "number" ) )
			json::getValue( v[ "number" ], number );
		if( v.HasMember( "color" ) )
			json::getValue( v[ "color" ], color );
		if( v.HasMember( "rotation" ) )
			json::getValue( v[ "rotation" ], rotation );
		if( v.HasMember( "scale" ) )
			json::getValue( v[ "scale" ], scale );
		if( v.HasMember( "position" ) )
			position.setValue( v[ "position" ] );
		if( v.HasMember( "velocity" ) )
			velocity.setValue( v[ "velocity" ] );
		if( v.HasMember( "acceleration" ) )
			acceleration.setValue( v[ "acceleration" ] );
		if( v.HasMember( "spin" ) )
			json::getValue( v[ "spin" ], spin );
		if( v.HasMember( "inheritance" ) )
			inheritance.setValue( v[ "inheritance" ] );
		if( v.HasMember( "texture" ) )
			json::getValue( v[ "texture" ], texture );
		if( v.HasMember( "priority" ) )
			json::getValue( v[ "priority" ], priority );
		if( v.HasMember( "affectors" ) )
			for( const rapidjson::Value& value : v[ "affectors" ].GetArray() )
				affectors.push_back( Affector::setValue( value ) );
		if( v.HasMember( "emitters" ) ) {
			for( const rapidjson::Value& value : v[ "emitters" ].GetArray() ) {
				ParticleEmitter emitter;
				emitter.setValue( value );
				emitters.push_back( emitter );
			}
		}

		return true;
	}

	bool render() {
		bool out = false;

		ImGui::PushID( "Particle Pattern" );

		ImGui::BeginTabBar( "" );

		if( ImGui::BeginTabItem( "Properties" ) ) {

			ImGui::Text( "Lifetime" );
			ImGui::Separator();
			out |= ImGui::render( lifetime, "##Lifetime" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Number" );
			ImGui::Separator();
			out |= ImGui::render( number, "##Number" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Color" );
			ImGui::Separator();
			out |= ImGui::render( color, "##Color" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Priority" );
			ImGui::Separator();
			out |= ImGui::InputInt( "##Priority", &priority );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Rotation" );
			ImGui::Separator();
			out |= ImGui::render( rotation, "##Rotation" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Spin" );
			ImGui::Separator();
			out |= ImGui::render( spin, "##Spin" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Scale" );
			ImGui::Separator();
			out |= ImGui::render( scale, "##Scale" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Position" );
			ImGui::Separator();
			out |= position.render();
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Velocity" );
			ImGui::Separator();
			out |= velocity.render( "" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text( "Acceleration" );
			ImGui::Separator();
			out |= acceleration.render( "Acceleration" );
			ImGui::Separator();
			ImGui::Separator();

			ImGui::EndTabItem();
		}

		if( ImGui::BeginTabItem( "Inheritance" ) ) {
			out |= inheritance.render();
			ImGui::EndTabItem();
		}

		if( ImGui::BeginTabItem( "Emitters" ) ) {
			ImGui::PushID( "Emitters" );

			for( size_t i = 0; i < emitters.size(); ++i ) {
				ImGui::PushID( ( int )i );

				out |= emitters.at( i ).render();

				ImGui::Separator();

				if( ImGui::Button( "Remove" ) ) {
					out = true;
					emitters.erase( emitters.begin() + i );
					i--;
				}

				ImGui::Separator();
				ImGui::Separator();

				ImGui::PopID();
			}

			if( ImGui::Button( "Add Emitter" ) )
				emitters.push_back( ParticleEmitter() );

			ImGui::PopID();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		ImGui::PopID();

		return out;
	}
};

//--------------------------------------------------------------------------------

}

//================================================================================