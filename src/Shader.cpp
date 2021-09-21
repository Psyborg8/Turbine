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

	sf::Shader& shader = shaders[ name ];
	string path = Folders::FragmentShader + name + ".fs";
	if( !std::filesystem::exists( std::filesystem::path( path ) ) ) {
		Debug::addMessage( "Shader " + path + " doesn't exist.", Debug::DebugType::Error );
		return shader;
	}
	if( !shader.loadFromFile( path, sf::Shader::Type::Fragment ) )
		Debug::addMessage( "Shader " + path + " failed to compile.", Debug::DebugType::Error );
	return shader;
}

//--------------------------------------------------------------------------------

}
}

//================================================================================