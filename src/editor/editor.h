//================================================================================

#pragma once

//================================================================================

#include "app.h"
#include "editor-window-base.h"
#include "global.h"

//================================================================================

namespace Editor {

//--------------------------------------------------------------------------------

class Editor : public App {
public:
	Editor()		  = default;
	virtual ~Editor() = default;

public:
	void onSpawnChildren() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender( sf::RenderTarget* target ) override;
	void onEvent( sf::Event e ) override;
	void onMessage( string message ) override;

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
