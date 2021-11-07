//================================================================================

#pragma once

//================================================================================

#include "global.h"
#include "world.h"

#include "particle-system.h"
#include "particle-affector.h"
#include "particle-emitter.h"

//================================================================================

class ParticleTestWorld : public World {
public:
	ParticleTestWorld();

	// Events
public:
	virtual void onSpawnChildren() override;

	shared_ptr< Gfx::Particle::System > m_particleSystem;
};

//================================================================================