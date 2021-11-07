//================================================================================

#pragma once

//================================================================================

#include "global.h"

//================================================================================

namespace Gfx::Particle {

//--------------------------------------------------------------------------------

struct ParticlePattern;

//--------------------------------------------------------------------------------

namespace Loader {

//--------------------------------------------------------------------------------

ParticlePattern* get( string path );

void unload();

//--------------------------------------------------------------------------------

}
}

//================================================================================