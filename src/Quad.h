#pragma once

//================================================================================

#include "Object.h"
#include "MathTypes.h"
#include "Shader.h"

//================================================================================

class Quad : public Object
{
public:
	Quad();

	void onRender() override;

public:
	inline Vec2 getPosition() { return m_position; }
	inline void setPosition( Vec2 position ) { m_position = position; }

	inline Vec2 getScale() { return m_scale; }
	inline void setScale( Vec2 scale ) { m_scale = scale; }

	inline Vec2 getAnchor() { return m_anchor; }
	inline void setAnchor( Vec2 anchor ) { m_anchor = anchor; }

	inline float getRotation() { return m_rotation; }
	inline void setRotation( float rotation ) { m_rotation = rotation; }

	inline Color getColor() { return m_color; }
	inline void setColor( Color color ) { m_color = color; }

	Shader getShader( ShaderType type ) { return m_shaders[ type ]; }
	inline void setShader( ShaderType type, string name ) { m_shaders[ type ] = Shader::getShader( type, name ); }

	array< Vec2, 4 > getVertices();

public:
	bool isPointInside( Vec2 point );
	bool isInsideQuad( Quad quad );

	b2PolygonShape toCollider();

protected:
	unordered_map< ShaderType, Shader > m_shaders;
	Vec2 m_position;
	Vec2 m_scale;
	Vec2 m_anchor;
	float m_rotation;
	Color m_color;
};

//================================================================================
