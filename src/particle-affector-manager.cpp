//================================================================================

#include "particle-affector-manager.h"

//--------------------------------------------------------------------------------

#include "string-utils.h"

#include "particle.h"
#include "particle-emitter.h"
#include "particle-system.h"
#include "particle-affector.h"

//================================================================================

namespace Gfx::Particle::Affector {



//================================================================================

vector< unique_ptr< AffectorFactory > > factories;
//--------------------------------------------------------------------------------

void init() {
	factories.push_back( make_unique< RotateToVelocityAffectorFactory >() );

	factories.push_back( make_unique< SinPositionAffectorFactory >() );
	factories.push_back( make_unique< SinVelocityAffectorFactory >() );
	factories.push_back( make_unique< SinAlphaAffectorFactory >() );
	factories.push_back( make_unique< SinColorAffectorFactory >() );
	factories.push_back( make_unique< SinSpinAffectorFactory >() );

	factories.push_back( make_unique< FadeVelocityAffectorFactory >() );
	factories.push_back( make_unique< FadeScaleAffectorFactory >() );
	factories.push_back( make_unique< FadeAlphaAffectorFactory >() );
	factories.push_back( make_unique< FadeColorAffectorFactory >() );
}

//--------------------------------------------------------------------------------

void apply( Particle* target, sf::Time delta ) {
	for( shared_ptr< Affector > affector : target->affectors )
		affector->affect( target, delta );
}

//--------------------------------------------------------------------------------

void apply( Emitter* target, sf::Time delta ) {
	for( shared_ptr< Affector > affector : target->affectors )
		affector->affect( target, delta );
}

//--------------------------------------------------------------------------------

void apply( System* target, sf::Time delta ) {
	//
}

//--------------------------------------------------------------------------------

bool render( list< shared_ptr< AffectorCreator > >& creators ) {
	bool out = false;

	ImGui::PushID( "Affector Creator List" );

	for( unique_ptr< AffectorFactory >& factory : factories ) {
		if( ImGui::Selectable( factory->getName().c_str() ) ) {
			creators.push_back( factory->createCreator() );
			out = true;
		}
	}

	ImGui::PopID();
	
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( shared_ptr< AffectorCreator > affector ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "name", json::getValue( affector->getName() ), json::getAllocator() );
	out.AddMember( "value", affector->getValue(), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< AffectorCreator > setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return nullptr;
	if( !value.HasMember( "name" ) || !value.HasMember( "value" ) )
		return nullptr;

	string name;
	json::getValue( value[ "name" ], name );

	const auto it = std::find_if( factories.begin(), factories.end(),
								  [name]( const unique_ptr< AffectorFactory >& factory ) {
									  return factory->getName() == name;
								  } );

	if( it == factories.end() )
		return nullptr;

	shared_ptr< AffectorCreator > out = ( *it )->createCreator();
	out->setValue( value[ "value" ] );

	return out;
}

//--------------------------------------------------------------------------------

}

//================================================================================