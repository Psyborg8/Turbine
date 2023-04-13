//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "mathtypes.h"
#include "random.h"
#include "debug.h"


//================================================================================

namespace Gfx::Particle {

//--------------------------------------------------------------------------------

struct Particle;
struct Emitter;
class System;

//--------------------------------------------------------------------------------

namespace Affector {

class AffectorCreator;

//--------------------------------------------------------------------------------

void init();

void apply( Particle* target, sf::Time delta );
void apply( Emitter* target, sf::Time delta );
void apply( System* target, sf::Time delta );

bool render( list< shared_ptr< AffectorCreator > >& out );

rapidjson::Value getValue( shared_ptr< AffectorCreator > affector );
shared_ptr< AffectorCreator > setValue( const rapidjson::Value& value );

//--------------------------------------------------------------------------------

}
}

//================================================================================