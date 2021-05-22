//================================================================================

#include "Line.h"

//================================================================================

namespace Gfx
{

//================================================================================

void Line::render()
{
	glBegin( GL_LINES );

	glColor4f( color.r, color.g, color.b, color.a );
	glPointSize( weight );

	glVertex2d( start.x, start.y );
	glVertex2d( end.x, end.y );

	glEnd();
}

//================================================================================

}