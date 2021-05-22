//================================================================================

#pragma once

//================================================================================

#include "Object.h"

//================================================================================

class Text : public Object
{
public:
	Text( string text, Vec2 pos, Color color );
	Text( function< string() > textFunc, Vec2 pos, Color color );

	// Events
public:
	void onRender();

	// Variables
protected:
	string m_text;
	function< string() > m_textFunc;

	Vec2 m_pos;
	Color m_color;
};

//================================================================================