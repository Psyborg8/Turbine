//================================================================================

#include "particle-loader.h"

//--------------------------------------------------------------------------------

#include "json.h"
#include "string-utils.h"

#include "particle.h"

//================================================================================

namespace Gfx::Particle::Loader {

//--------------------------------------------------------------------------------

map< string, ParticlePattern > patterns;

//================================================================================

ParticlePattern* get( string path ) {
	if( patterns.count( path ) )
		return &patterns[ path ];

	ParticlePattern pattern;
	if( !json::load( [&pattern]( const rapidjson::Value& value ) { return pattern.setValue( value ); }, path ) )
		Debug::addMessage( Utils::format( "%s failed to load", path.c_str() ), DebugType::Error );

	patterns[ path ] = pattern;
	return &patterns[ path ];
}

//--------------------------------------------------------------------------------

void unload() {
	patterns.clear();
}

//--------------------------------------------------------------------------------

}

//================================================================================