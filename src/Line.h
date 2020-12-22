//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "MathTypes.h"
#include "Shader.h"

//================================================================================

class Line : public Object
{
public:
	Line();

	void onRender() override;

public:
	inline Vec2 getStart() { return m_start; }
	inline void setStart( Vec2 start ) { m_start = start; }
	inline Vec2 getEnd() { return m_end; }
	inline void setEnd( Vec2 end ) { m_end = end; }

	inline pair< Vec2, Vec2 > getPoints() { return make_pair( m_start, m_end ); }
	inline void setPoints( Vec2 start, Vec2 end ) { m_start = start; m_end = end; }

	inline Color getColor() { return m_color; }
	inline void setColor( Color color ) { m_color = color; }

	inline GLfloat getThickness() { return m_thickness; }
	inline void setThickness( GLfloat thickness ) { m_thickness = thickness; }

	Shader getShader( ShaderType type ) { return m_shaders[ type ]; }
	inline void setShader( ShaderType type, string name ) { m_shaders[ type ] = Shader::getShader( type, name ); }

private:
	Vec2 m_start;
	Vec2 m_end;
	Color m_color;
	GLfloat m_thickness;
	unordered_map< ShaderType, Shader > m_shaders;
};

//================================================================================