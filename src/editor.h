//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "world.h"
#include "editor-window-base.h"

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

	string m_renameWindowBuffer;
	bool m_renameWindowOpen{ false };

	string m_loadWindowBuffer;
	bool m_loadWindowOpen{ false };
};

//--------------------------------------------------------------------------------

}

//================================================================================