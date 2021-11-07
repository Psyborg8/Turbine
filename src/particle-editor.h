//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "particle.h"
#include "particle-emitter.h"
#include "particle-system.h"
#include "particle-affector.h"
#include "particle-manager.h"

#include "world.h"
#include "editor-window-base.h"

//================================================================================

namespace Editor {

//--------------------------------------------------------------------------------

class ParticleEditor : public EditorWindow {
public:
	ParticleEditor();
	ParticleEditor( string path );

	void init( World* world ) override;
	void open() override;
	void update( sf::Time delta ) override;
	void renderLeftWindow() override;
	void renderRightWindow() override;
	void close() override;
	void rename( string name ) override;
	void onEvent( sf::Event e ) override;

	string getName() const override;

private:
	void reset();
	void renderTextureWindow();
	void renderAffectorWindow();
	void save();

private:
	Gfx::Particle::ParticlePattern m_pattern;
	Gfx::Particle::EmitterPattern m_emitter;
	shared_ptr< Gfx::Particle::System > m_system;

	bool m_changed{ false };
	bool m_unsaved{ false };
	bool m_saved{ false };
};

//--------------------------------------------------------------------------------

}

//================================================================================