//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "World.h"
#include "Particle.h"
#include "Random.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

class BulletPatternEditor : public World {
private:
public:
	BulletPatternEditor() = default;

public:
	void onSpawnChildren() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;

public:
	void renderPattern( Gfx::Particle::Pattern& pattern );
	void renderPosition( Gfx::Particle::Pattern& pattern );
	void renderDirection( Gfx::Particle::Pattern& pattern );
	void renderVelocity( Gfx::Particle::Pattern& pattern );
	void renderGravity( Gfx::Particle::Pattern& pattern );
	void renderColor( Gfx::Particle::Pattern& pattern );
	void renderNumber( Gfx::Particle::Pattern& pattern );
	void renderSize( Gfx::Particle::Pattern& pattern );
	void renderLifetime( Gfx::Particle::Pattern& pattern );

private:
	Gfx::Particle::Pattern m_basePattern;
};

//--------------------------------------------------------------------------------

}

//================================================================================