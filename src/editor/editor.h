//================================================================================

#pragma once

//================================================================================

#include "editor-window-base.h"
#include "global.h"
#include "world.h"

//================================================================================

namespace Editor {

//--------------------------------------------------------------------------------

class Editor : public World {
public:
	Editor() = default;

public:
	void onSpawnChildren() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender( sf::RenderTarget* target ) override;
	void onEvent( sf::Event e ) override;

public:
	void newParticle();
	void openParticle( string path );
	void save();
	void saveAll();

private:
	vector< unique_ptr< EditorWindow > > m_tabs;
	EditorWindow* m_currentTab;
};

//--------------------------------------------------------------------------------

}	 // namespace Editor

//================================================================================
