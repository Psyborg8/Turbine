//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "World.h"
#include "Particle.h"
#include "Random.h"
#include "Emitter.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

struct Tab {
	bool emitter{ false };
	void* pattern;
};

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
	void renderEmitter( Gfx::Emitter::Pattern& emitter );
	void renderPatternTreeView();
	void renderPatternTreeNode( Gfx::Particle::Pattern& pattern, vector< Tab > stack );
	void renderEmitterTreeNode( Gfx::Emitter::Pattern& pattern, vector< Tab > stack );

private:
	bool m_emitter{ true };
	Gfx::Particle::Pattern m_basePattern;
	Gfx::Emitter::Pattern m_baseEmitter;

	Gfx::Particle::Pattern m_defaultPattern;
	Gfx::Emitter::Pattern m_defaultEmitter;

	bool m_useMouse{ false };
	bool m_mouseDown{ false };
	Math::Vec2 m_mouseOrigin{ 0.0f, 0.0f };

	bool m_changed{ false };

	int m_patternID{ 0 };
	int m_patternTreeID{ 0 };

	vector< Tab > m_tabs;

	char* m_renameBuffer;

	bool m_styleEditorOpen;
};

//--------------------------------------------------------------------------------

}

//================================================================================