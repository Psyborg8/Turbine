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
	void onEvent( sf::Event e ) override;
	void onDestroy() override;

public:
	void renderPattern( Gfx::Particle::Pattern& pattern );
	void renderEmitter( Gfx::Particle::Pattern::Emitter& emitter );
	void renderPatternTreeView();
	void renderPatternTreeNode( Gfx::Particle::Pattern& pattern, vector< Gfx::Particle::Pattern* > stack );

private:
	Gfx::Particle::Pattern m_basePattern;

	bool m_useMouse{ false };
	bool m_mouseDown{ false };
	Math::Vec2 m_mouseOrigin{ 0.0f, 0.0f };

	bool m_changed{ false };

	int m_patternID{ 0 };
	int m_patternTreeID{ 0 };

	Gfx::Particle::Pattern m_defaultPattern;
	Gfx::Particle::Pattern::Emitter m_defaultEmitter;

	vector< Gfx::Particle::Pattern* > m_tabs;

	char* m_renameBuffer;

	bool m_styleEditorOpen;
};

//--------------------------------------------------------------------------------

}

//================================================================================