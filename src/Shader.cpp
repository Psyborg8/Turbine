//================================================================================

#include "Shader.h"

//--------------------------------------------------------------------------------

#include <filesystem>

#include "Debug.h"

//================================================================================

map< string, sf::Shader > shaders;

//================================================================================

namespace Gfx {
namespace Shader {

//--------------------------------------------------------------------------------

sf::Shader& get( string name ) {
	if( shaders.count( name ) )
		return shaders.at( name );

	char path[ 2048 ];
	char message[ 2048 ];
	sprintf_s( path, "%s%s.fs", Folders::FragmentShader.c_str(), name.c_str() );
	if( !std::filesystem::exists( std::filesystem::path( path ) ) ) {
		sprintf_s( message, "Shader %s doesn't exist.", path );
		Debug::addMessage( message, Debug::DebugType::Error );
		return shaders[ "default" ];
	}

	sf::Shader& shader = shaders[ name ];
	if( !shader.loadFromFile( path, sf::Shader::Type::Fragment ) ) {
		sprintf_s( message, "Shader %s failed to compile.", path );
		Debug::addMessage( message, Debug::DebugType::Error );
	}

	return shader;
}

//--------------------------------------------------------------------------------

}
}

//================================================================================