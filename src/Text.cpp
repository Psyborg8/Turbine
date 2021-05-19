//================================================================================

#include "Text.h"

//================================================================================

Text::Text( string text, Vec2 pos, Color color ) : Object(), m_text( text ), m_pos( pos ), m_color( color )
{
	//
}

//--------------------------------------------------------------------------------

Text::Text( function< string() > textFunc, Vec2 pos, Color color ) : Object(), m_textFunc( textFunc ), m_pos( pos ), m_color( color )
{
	//
}

//--------------------------------------------------------------------------------

void Text::onRender()
{
	//
}