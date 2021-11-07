//================================================================================

#pragma once

//================================================================================

#include "global.h"
#include "mathtypes.h"

//================================================================================

namespace ImGui {

//--------------------------------------------------------------------------------

bool render( Math::ValueSet< Math::Vec2 >& value, const char* id );
bool render( Math::ValueSet< Math::Color >& value, const char* id );
bool render( Math::ValueSet< float >& value, const char* id );
bool render( Math::ValueSet< int >& value, const char* id );

bool renderPatternSelector( string& target );

//--------------------------------------------------------------------------------

}

//================================================================================