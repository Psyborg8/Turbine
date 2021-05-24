//================================================================================

#pragma once

//================================================================================

#include "Collision.h"
#include "MathTypes.h"

//================================================================================

namespace Gfx {

//================================================================================

struct Rect : public Collision::Rect {
	bool fill = true;
	Math::Color fillColor = Colors::WHITE;
	bool line = false;
	Math::Color lineColor = Colors::WHITE;
	float lineWeight = 0.1f;

	//--------------------------------------------------------------------------------

	void render() {
		if( !( fill || line ) )
			return;

		array< Math::Vec2, 4 > verts = vertices();

		if( fill ) {
			glBegin( GL_QUADS );

			glColor4f( fillColor.r, fillColor.g, fillColor.b, fillColor.a );

			for( Math::Vec2 vert : verts )
				glVertex2d( vert.x, vert.y );

			glEnd();
		}
		if( line ) {
			glBegin( GL_LINES );

			glColor4f( lineColor.r, lineColor.g, lineColor.b, lineColor.a );
			glLineWidth( lineWeight );

			for( Math::Vec2 vert : verts )
				glVertex2d( vert.x, vert.y );

			glEnd();
		}
	}

	//--------------------------------------------------------------------------------

	array< Math::Vec2, 4 > vertices() {
		array< Math::Vec2, 4 > out = {
			position,
			Math::Vec2( position.x, position.y + size.y ),
			position + size,
			Math::Vec2( position.x + size.x, position.y )
		};

		return out;
	}
};

//================================================================================

} // namespace Gfx

//================================================================================