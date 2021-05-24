//================================================================================

#pragma once

//================================================================================

enum class ShaderType {
	Vertex = 0,
	Fragment,
};

//================================================================================

class Shader {
public:
	Shader();

private:
	Shader( ShaderType type, string name );

protected:
	void load();

	string findPath() const;

public:
	void setName( string name );
	void setType( ShaderType type );

	string getName() const { return m_name; }
	const string& getData() const { return m_data; }
	string getPath() const { return m_path; }
	ShaderType getType() const { return m_type; }
	GLuint getGlShader() const { return m_glShader; }

private:
	string m_name;
	string m_path;
	ShaderType m_type;

	GLuint m_glShader;

	string m_data;

public:
	static void loadShader( ShaderType type, string name );

	static const Shader& getGlobalShader( ShaderType type );
	static const Shader& getShader( ShaderType type, string name );

	static void setGlobal( ShaderType type, string name );

private:
	static vector< shared_ptr< Shader > > s_shaders;
	static unordered_map< ShaderType, shared_ptr< Shader > > s_global;
};

//================================================================================