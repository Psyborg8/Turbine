//================================================================================

#include "Rect.h"

//================================================================================

namespace Gfx
{

//================================================================================

void Rect::render()
{
	if( fillColor.a == 0.0f && lineColor.a == 0.0f )
		return;

	array< Vec2, 4 > verts = vertices();

	if( fill )
	{
		glBegin( GL_QUADS );

		glColor4f( fillColor.r, fillColor.g, fillColor.b, fillColor.a );

		for( Vec2 vert : verts )
			glVertex2d( vert.x, vert.y );

		glEnd();
	}
	if( line )
	{
		glBegin( GL_LINE );

		glColor4f( lineColor.r, lineColor.g, lineColor.b, lineColor.a );
		glLineWidth( lineWeight );

		for( Vec2 vert : verts )
			glVertex2d( vert.x, vert.y );

		glEnd();
	}
}

//--------------------------------------------------------------------------------

array< Vec2, 4 > Rect::vertices()
{
	array< Vec2, 4 > out = {
		position,
		Vec2( position.x, position.y + size.y ),
		position + size,
		Vec2( position.x + size.x, position.y )
	};

	return out;
}

//================================================================================

} // namespace Gfx

//================================================================================