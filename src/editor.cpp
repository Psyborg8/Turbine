//================================================================================

#include "editor.h"

//--------------------------------------------------------------------------------

#include "particle-editor.h"

#include "system.h"
#include "string-utils.h"
#include "debug.h"

#include "imgui_internal.h"

//================================================================================

namespace Editor {

//--------------------------------------------------------------------------------

void Editor::onSpawnChildren() {

}

//--------------------------------------------------------------------------------

void Editor::onUpdate( sf::Time deltaTime ) {
	if( m_currentTab != nullptr )
		m_currentTab->update( deltaTime );
}

//--------------------------------------------------------------------------------

void Editor::onRender( sf::RenderTarget* target ) {
	// Menu Bar
	ImGui::BeginMainMenuBar();
	if( ImGui::Button( "File" ) ) {
		ImGui::OpenPopup( "File Menu" );
	}
	if( ImGui::BeginPopupContextWindow( "File Menu" ) ) {
		if( ImGui::Selectable( "New Particle (Ctrl+P)" ) ) {
			newParticle();

			ImGui::CloseCurrentPopup();
		}
		if( ImGui::Selectable( "Open Particle (Ctrl+Shift+P)" ) ) {
			m_loadWindowOpen = true;

			ImGui::CloseCurrentPopup();
		}
		ImGui::Separator();
		if( ImGui::Selectable( "Save (Ctrl+S)" ) ) {
			if( m_currentTab != nullptr )
				m_currentTab->save();

			ImGui::CloseCurrentPopup();
		}
		if( ImGui::Selectable( "Save All (Ctrl+Shift+S)" ) ) {
			for( unique_ptr< EditorWindow >& tab : m_tabs )
				tab->save();

			ImGui::CloseCurrentPopup();
		}
		ImGui::Separator();
		if( ImGui::Selectable( "Rename (Ctrl+R)" ) && m_currentTab != nullptr ) {
			m_renameWindowBuffer = m_currentTab->getName();
			m_renameWindowBuffer.resize( 256u );
			m_renameWindowOpen = true;

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::EndMainMenuBar();

	// Message Bar
	ImGui::Begin( "Message Bar", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( System::getSystemInfo().width, 25 ) );
	ImGui::SetWindowPos( ImVec2( 0, System::getSystemInfo().height - ImGui::GetWindowHeight() ) );

	ImGui::Text( Debug::getLastMessage().c_str() );

	ImGui::End();

	// Rename Window
	if( m_renameWindowOpen ) {
		ImGui::Begin( "Rename", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove );

		ImGui::SetWindowSize( ImVec2( 200, 75 ) );
		ImGui::SetWindowPos( ImVec2( System::getSystemInfo().width / 2.f - ImGui::GetWindowWidth() / 2.f,
									 System::getSystemInfo().height / 2.f - ImGui::GetWindowHeight() / 2.f ) );

		ImGui::Text( "Rename" );

		if( ImGui::IsWindowAppearing() )
			ImGui::SetKeyboardFocusHere();

		ImGui::PushItemWidth( 183 );
		if( ImGui::InputText( "##RenameText", m_renameWindowBuffer.data(), 256u, 
							  ImGuiInputTextFlags_EnterReturnsTrue 
							  | ImGuiInputTextFlags_AutoSelectAll ) ) {

			if( m_currentTab != nullptr )
				m_currentTab->rename( string( m_renameWindowBuffer.data() ) );
			m_renameWindowOpen = false;
		}
		ImGui::PopItemWidth();

		if( ImGui::Button( "Apply" ) ) {
			if( m_currentTab != nullptr )
				m_currentTab->rename( string( m_renameWindowBuffer.data() ) );
			m_renameWindowOpen = false;
		}
		ImGui::SameLine();
		if( ImGui::Button( "Cancel" ) ) {
			m_renameWindowOpen = false;
		}

		ImGui::End();
	}

	// Tab Bar
	ImGui::Begin( "Tab Bar", ( bool* )0,
					   ImGuiWindowFlags_NoTitleBar 
					   | ImGuiWindowFlags_NoScrollbar 
					   | ImGuiWindowFlags_NoResize 
					   | ImGuiWindowFlags_NoBackground
					   | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( System::getSystemInfo().width, 0 ) );
	ImGui::SetWindowPos( ImVec2( 0, 20 ) );

	ImGui::BeginTabBar( "Tabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs );

	for( size_t i = 0; i < m_tabs.size(); ++i ) {
		ImGui::PushID( m_tabs.at( i )->getID() );
		bool open = true;
		string tabName = Utils::format( "%s###%i", m_tabs.at( i )->getName().c_str(), m_tabs.at( i )->getID() );
		if( ImGui::BeginTabItem( tabName.c_str(), &open ) ) {
			if( m_tabs.at( i ).get() != m_currentTab ) {
				if( m_currentTab != nullptr )
					m_currentTab->close();

				m_currentTab = m_tabs.at( i ).get();
				m_currentTab->open();
			}

			ImGui::EndTabItem();
		}
		if( !open ) {
			if( m_currentTab == m_tabs.at( i ).get() )
				m_currentTab = nullptr;

			m_tabs.erase( m_tabs.begin() + i );
		}

		ImGui::PopID();
	}

	ImGui::EndTabBar();

	ImGui::End();

	// Editor Window
	ImGui::Begin( "Left Window", ( bool* )0,
				  ImGuiWindowFlags_NoCollapse
				  | ImGuiWindowFlags_NoTitleBar
				  | ImGuiWindowFlags_NoResize
				  | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( 300, System::getSystemInfo().height - 82.f ) );
	ImGui::SetWindowPos( ImVec2( 0, 50 ) );

	if( m_currentTab != nullptr )
		m_currentTab->renderLeftWindow();

	ImGui::End();

	ImGui::Begin( "Right Window", ( bool* )0,
				  ImGuiWindowFlags_NoCollapse
				  | ImGuiWindowFlags_NoTitleBar
				  | ImGuiWindowFlags_NoResize
				  | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( 300, ( System::getSystemInfo().height - 82.f ) ) );
	ImGui::SetWindowPos( ImVec2( System::getSystemInfo().width - ImGui::GetWindowWidth(), 50 ) );

	if( m_currentTab != nullptr )
		m_currentTab->renderRightWindow();

	ImGui::End();

	// Load Window
	if( m_loadWindowOpen )
		if( ImGui::renderPatternSelector( m_loadWindowBuffer ) )
			openParticle( m_loadWindowBuffer );
}

//--------------------------------------------------------------------------------

void Editor::onEvent( sf::Event e ) {
	if( e.type == sf::Event::EventType::KeyPressed ) {
		// Saving
		if( e.key.code == sf::Keyboard::S && e.key.control ) {
			if( e.key.shift ) {
				for( unique_ptr< EditorWindow >& window : m_tabs ) {
					saveAll();
				}
				return;
			}

			save();
			return;
		}

		if( e.key.code == sf::Keyboard::R && e.key.control && m_currentTab != nullptr ) {
			m_renameWindowBuffer = m_currentTab->getName();
			m_renameWindowBuffer.resize( 256u );
			m_renameWindowOpen = true;
			
			return;
		}

		if( e.key.code == sf::Keyboard::P && e.key.control ) {
			if( e.key.shift ) {
				m_loadWindowOpen = true;
				return;
			}

			newParticle();
			return;
		}
	}

	if( m_currentTab != nullptr )
		m_currentTab->onEvent( e );
}

//--------------------------------------------------------------------------------

void Editor::newParticle() {
	m_tabs.push_back( make_unique< ParticleEditor >() );
	m_tabs.rbegin()->get()->init( this );
}

//--------------------------------------------------------------------------------

void Editor::openParticle( string path ) {
	for( size_t i = 0; i < m_tabs.size(); ++i ) {
		string p = Folders::Bullets + m_tabs.at( i )->getName() + ".bullet";
		if( p != path )
			continue;

		m_tabs.erase( m_tabs.begin() + i );
		break;
	}

	m_tabs.push_back( make_unique< ParticleEditor >( path ) );
	m_tabs.rbegin()->get()->init( this );
	m_loadWindowOpen = false;
}

//--------------------------------------------------------------------------------

void Editor::save() {
	if( m_currentTab != nullptr )
		m_currentTab->save();
}

//--------------------------------------------------------------------------------

void Editor::saveAll() {
	for( const unique_ptr< EditorWindow >& tab : m_tabs )
		tab->save();
}

//--------------------------------------------------------------------------------

}

//================================================================================