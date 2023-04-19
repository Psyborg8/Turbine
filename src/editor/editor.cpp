//================================================================================

#include "editor.h"

//--------------------------------------------------------------------------------

#include "debug.h"
#include "imgui_internal.h"
#include "particle-editor.h"
#include "string-utils.h"
#include "system.h"

//================================================================================

int main( int argc, char** argv ) {
	System::init( make_shared< Editor::Editor >() );
	System::start();

	return 0;
}

// ======================================================================

namespace Editor {

//--------------------------------------------------------------------------------

void Editor::onSpawnChildren() {}

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
			ImGui::openPatternSelector(
				[this]( string path ) { openParticle( path ); } );

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
			ImGui::openRenamePopup(
				[this]( string name ) { m_currentTab->rename( name ); },
				m_currentTab->getName() );
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::EndMainMenuBar();

	// Message Bar
	ImGui::Begin( "Message Bar", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( System::getSystemInfo().width, 25 ) );
	ImGui::SetWindowPos(
		ImVec2( 0, System::getSystemInfo().height - ImGui::GetWindowHeight() ) );

	ImGui::Text( "%s", Debug::getLastMessage().c_str() );

	ImGui::End();

	// Tab Bar
	ImGui::Begin( "Tab Bar",
				  ( bool* ) 0,
				  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
					  | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( System::getSystemInfo().width, 0 ) );
	ImGui::SetWindowPos( ImVec2( 0, 20 ) );

	ImGui::BeginTabBar( "Tabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs );

	for( size_t i = 0; i < m_tabs.size(); ++i ) {
		ImGui::PushID( m_tabs.at( i )->getID() );
		bool open	   = true;
		string tabName = Utils::format(
			"%s###%i", m_tabs.at( i )->getName().c_str(), m_tabs.at( i )->getID() );
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
	ImGui::Begin( "Left Window",
				  ( bool* ) 0,
				  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar
					  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( 300, System::getSystemInfo().height - 82.f ) );
	ImGui::SetWindowPos( ImVec2( 0, 50 ) );

	if( m_currentTab != nullptr )
		m_currentTab->renderLeftWindow();

	ImGui::End();

	ImGui::Begin( "Right Window",
				  ( bool* ) 0,
				  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar
					  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( 300, ( System::getSystemInfo().height - 82.f ) ) );
	ImGui::SetWindowPos(
		ImVec2( System::getSystemInfo().width - ImGui::GetWindowWidth(), 50 ) );

	if( m_currentTab != nullptr )
		m_currentTab->renderRightWindow();

	ImGui::End();

	// Popups
	ImGui::renderPatternSelector();
	ImGui::renderRenamePopup();
}

//--------------------------------------------------------------------------------

void Editor::onEvent( sf::Event e ) {
	if( e.type == sf::Event::EventType::KeyPressed ) {
		// Saving
		if( e.key.code == sf::Keyboard::S && e.key.control ) {
			if( e.key.shift ) {
				for( size_t i = 0u; i < m_tabs.size(); ++i ) {
					saveAll();
				}
				return;
			}

			save();
			return;
		}

		if( e.key.code == sf::Keyboard::R && e.key.control && m_currentTab != nullptr ) {
			ImGui::openRenamePopup(
				[this]( string name ) { m_currentTab->rename( name ); },
				m_currentTab->getName() );
			return;
		}

		if( e.key.code == sf::Keyboard::P && e.key.control ) {
			if( e.key.shift ) {
				ImGui::openPatternSelector(
					[this]( string path ) { openParticle( path ); } );
				return;
			}

			newParticle();
			return;
		}
	}

	if( m_currentTab != nullptr )
		m_currentTab->onEvent( e );
}

// ----------------------------------------------------------------------

void Editor::onMessage( string message ) {
	vector< string > args = Utils::tokenize( message, " " );

	if( args.empty() == 0u || args.at( 0u ) != "editor" )
		return;

	args.erase( args.begin() );

	if( args.empty() )
		return;

	if( args.at( 0u ) == "open" ) {
		args.erase( args.begin() );

		if( args.empty() )
			return;

		if( args.at( 0u ) == "particle" ) {
			args.erase( args.begin() );

			if( args.empty() )
				newParticle();
			else {
				// Allows for spaces in paths
				string path;
				for( string token : args )
					path += token;

				openParticle( path );
			}
		}
	}
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

}	 // namespace Editor

//================================================================================
