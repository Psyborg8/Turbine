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
private:
	Gfx::Particle::Pattern m_basePattern;

	struct {
		bool lifetime{ false };
		bool number{ true };
		bool direction{ false };
		bool velocity{ false };
		bool acceleration{ false };
		bool size{ false };
		bool color{ false };
	} propertyExpanded;
};

//--------------------------------------------------------------------------------

}

//================================================================================