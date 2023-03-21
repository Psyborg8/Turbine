//================================================================================

#include "shader.h"

//--------------------------------------------------------------------------------

#include "debug.h"

#include <filesystem>
#include <fmt/format.h>

//================================================================================

map< string, sf::Shader > shaders;

//================================================================================

namespace Gfx {
namespace Shader {

//--------------------------------------------------------------------------------

sf::Shader& get( string name ) {
	if( shaders.count( name ) )
		return shaders.at( name );

	std::string path = fmt::format( "{}{}.fs", Folders::FragmentShader, name );
	if( !std::filesystem::exists( std::filesystem::path( path ) ) ) {
		Debug::addMessage( fmt::format( "Shader {} doesn't exist.", path ), DebugType::Error );
		return shaders["default"];
	}

	sf::Shader& shader = shaders[name];
	if( !shader.loadFromFile( path, sf::Shader::Type::Fragment ) ) {
		Debug::addMessage( fmt::format( "Shader {} failed to compile.", path ),
						   DebugType::Error );
	}

	return shader;
}

//--------------------------------------------------------------------------------

}	 // namespace Shader
}	 // namespace Gfx

//================================================================================
