
#include "Shader.h"

#include "System.h"

//================================================================================

Shader::Shader() : m_type( ShaderType::Vertex ), m_name( "" )
{
	//
}

//--------------------------------------------------------------------------------

Shader::Shader( ShaderType type, string name ) : m_type( type ), m_name( name )
{
	m_path = findPath();
	load();
}

//--------------------------------------------------------------------------------

void Shader::load()
{
	std::ifstream file;
	file.open( m_path, std::ios::in );

	if( file.is_open() )
	{
		std::streampos size;
		char * memblock;

		size = file.tellg();
		memblock = new char[ size ];
		file.seekg( 0, std::ios::beg );
		file.read( memblock, size );
		file.close();

		m_data = memblock;

		delete[]  memblock;
	}
	else
	{
		assert( !"Shader file doesn't exist!" );
		return;
	}

	int type;

	switch( m_type )
	{
	case ShaderType::Fragment:
		type = GL_FRAGMENT_SHADER;
		break;
	case ShaderType::Vertex:
		type = GL_VERTEX_SHADER;
		break;
	default:
		break;
	}

	m_glShader = glCreateShader( type );

	const char* data = m_data.c_str();
	glShaderSource( m_glShader, 1, &data, NULL );
		
	glCompileShader( m_glShader );

	GLint success;
	glGetShaderiv( m_glShader, GL_COMPILE_STATUS, &success );

	if( !success )
	{
		char infoLog[ 512 ];
		glGetShaderInfoLog( m_glShader, 512, NULL, infoLog );
		assert( !"Shader compilation failed!" );
		return;
	}
}

//--------------------------------------------------------------------------------

string Shader::findPath() const
{
	string out;

	string folder, extension;
	switch( m_type )
	{
	case ShaderType::Vertex:
		folder = Folders::VERTEX_SHADER;
		extension = ".vs";
		break;
	case ShaderType::Fragment:
		folder = Folders::FRAGMENT_SHADER;
		extension = ".fs";
		break;
	default:
		break;
	}

	out += folder + m_name + extension;

	return out;
}

//--------------------------------------------------------------------------------

void Shader::setName( string name )
{
	m_name = name;
	m_path = findPath();
	load();
}

//--------------------------------------------------------------------------------

void Shader::setType( ShaderType type )
{
	m_type = type;
	m_path = findPath();
	load();
}

//================================================================================

// STATIC

//================================================================================

void Shader::loadShader( ShaderType type, string name )
{
	const Shader shader = Shader( type, name );
	s_shaders.push_back( std::make_shared< Shader >( shader ) );
}

//--------------------------------------------------------------------------------

const Shader& Shader::getGlobalShader( ShaderType type )
{
	if( s_global[ type ] == nullptr )
	{
		s_global[ type ] = std::make_unique< Shader >( getShader( type, "Default" ) );
	}

	return *s_global.at( type );
}

//--------------------------------------------------------------------------------

const Shader& Shader::getShader( ShaderType type, string name )
{
	const auto it = std::find_if( s_shaders.begin(), s_shaders.end(), 
		[ type, name ]( shared_ptr< Shader > shader ) { 
			return shader->getType() == type && shader->getName() == name;
		} 
	);

	if( it != s_shaders.end() )
	{
		return **it;
	}

	if( name == "Default" )
	{
		loadShader( type, name ); // Construct the default shader so we have something to pass back
	}

	return getShader( type, "Default" ); // Return the default shader if none found
}

//--------------------------------------------------------------------------------

void Shader::setGlobal( ShaderType type, string name )
{
	Shader s = getShader( type, name );
	if( name != s.getName() )
	{
		// Shader doesn't exist, don't replace global with default
		return;
	}
	
	s_global.at( type ) = std::make_unique< Shader >( s );
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Shader > > Shader::s_shaders;

unordered_map< ShaderType, shared_ptr< Shader > > Shader::s_global;

//================================================================================