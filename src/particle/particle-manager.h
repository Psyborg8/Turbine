//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "particle.h"

//================================================================================

namespace Gfx::Particle {

struct Emitter;

namespace Manager {

//--------------------------------------------------------------------------------

void init();
void update( sf::Time delta );
void postUpdate( sf::Time delta );
void render( sf::RenderTarget* target );
void clearAll();

list< Particle* > spawnParticle( ParticlePattern pattern, sf::Transformable parentTransform = sf::Transformable() );
list< Particle* > spawnParticle( ParticlePattern pattern, Particle* parent );
list< Particle* > spawnParticle( ParticlePattern pattern, Emitter* parent );

size_t getParticleCount();
size_t getRenderGroupCount();

//--------------------------------------------------------------------------------

}
}

//================================================================================