//================================================================================

#include "particle-editor.h"

//--------------------------------------------------------------------------------

#include "global.h"
#include "object.h"
#include "particle-loader.h"
#include "sprite.h"
#include "string-utils.h"
#include "system.h"

#include <filesystem>

//================================================================================

namespace Editor {

//--------------------------------------------------------------------------------

ParticleEditor::ParticleEditor() {}

//--------------------------------------------------------------------------------

ParticleEditor::ParticleEditor( string path ) {
	json::load(
		[this]( const rapidjson::Value& value ) {
			return m_pattern.setValue( value );
		},
		path );
}

//--------------------------------------------------------------------------------

void ParticleEditor::init( World* world ) {
	m_system = Object::makeObject< Gfx::Particle::System >( world );
}

//--------------------------------------------------------------------------------

void ParticleEditor::open() {
	reset();
}

//--------------------------------------------------------------------------------

void ParticleEditor::update( sf::Time delta ) {
	if( m_changed ) {
		reset();
		m_changed = false;
		m_unsaved = true;
	}
	if( m_saved ) {
		reset();
		m_saved = false;
	}

	if( Gfx::Particle::Manager::getParticleCount() == 0u )
		reset();
}

//--------------------------------------------------------------------------------

void ParticleEditor::renderLeftWindow() {
	m_changed |= m_pattern.render();
}

//--------------------------------------------------------------------------------

void ParticleEditor::renderRightWindow() {
	ImGui::BeginTabBar( "Right Window Tabs" );

	if( ImGui::BeginTabItem( "Affectors" ) ) {
		renderAffectorWindow();
		ImGui::EndTabItem();
	}
	if( ImGui::BeginTabItem( "Texture" ) ) {
		renderTextureWindow();
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

//--------------------------------------------------------------------------------

void ParticleEditor::close() {
	m_system->stop();
	m_system->kill();
}

//--------------------------------------------------------------------------------

void ParticleEditor::rename( string name ) {
	m_pattern.name = name;
}

//--------------------------------------------------------------------------------

void ParticleEditor::onEvent( sf::Event e ) {
	if( e.type == sf::Event::EventType::KeyPressed ) {
		if( e.key.code == sf::Keyboard::S && e.key.control ) {
			save();
		}
	}
}

//--------------------------------------------------------------------------------

string ParticleEditor::getName() const {
	string out = m_pattern.name;
	if( m_unsaved )
		out += "*";
	return out;
}

//--------------------------------------------------------------------------------

void ParticleEditor::reset() {

	m_system->stop();
	m_system->kill();

	Gfx::Particle::Manager::clearAll();
	Gfx::Particle::Loader::unload();

	m_system->getEmitters().clear();

	m_emitter.patterns.clear();
	m_emitter.patterns.push_back( m_pattern );

	m_system->addEmitter( m_emitter );

	m_system->start();
}

//--------------------------------------------------------------------------------

void ParticleEditor::renderTextureWindow() {
	std::function< void( std::filesystem::path ) > iterate;

	iterate = [this]( std::filesystem::path path ) {
		std::filesystem::recursive_directory_iterator it
			= std::filesystem::recursive_directory_iterator( path );
		int columns = 0;
		for( const auto& directory : it ) {
			if( directory.path().extension() == ".png" ) {
				if( columns > 0 && columns <= 4 )
					ImGui::SameLine();
				if( columns > 4 )
					columns = 0;

				ImGui::PushID( directory.path().string().c_str() );
				ImVec2 pos	  = ImGui::GetCursorScreenPos();
				bool selected = m_pattern.texture == directory.path().string();
				if( ImGui::Selectable( "", &selected, 0, ImVec2( 47.f, 47.f ) ) ) {
					m_changed		  = true;
					m_pattern.texture = directory.path().string();
				}

				ImGui::SetCursorScreenPos( pos );
				sf::Sprite sprite;
				sprite.setTexture( Gfx::Sprite::get(
					Gfx::Sprite::get( directory.path().string() ) ) );

				ImGui::Image( sprite, sf::Vector2f( 47.f, 47.f ) );

				columns++;
				ImGui::PopID();
			}
		}
	};

	iterate( Folders::Particles );
}

//--------------------------------------------------------------------------------

void ParticleEditor::renderAffectorWindow() {
	ImGui::PushID( "Affectors" );
	{
		auto it = m_pattern.affectors.begin();
		int i	= 0;
		while( it != m_pattern.affectors.end() ) {
			const auto temp = it++;
			ImGui::PushID( i++ );

			m_changed |= ( *temp )->render();
			if( ImGui::Button( "Remove" ) ) {
				m_pattern.affectors.erase( temp );
				m_changed = true;
			}

			if( it != m_pattern.affectors.end() ) {
				ImGui::Separator();
				ImGui::Separator();
			}

			ImGui::PopID();
		}
	}

	ImGui::PopID();

	if( ImGui::BeginPopupContextWindow( "Affector Menu" ) ) {
		if( Gfx::Particle::Affector::render( m_pattern.affectors ) ) {
			m_changed = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

//--------------------------------------------------------------------------------

void ParticleEditor::save() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[96];

	time( &rawtime );
	timeinfo = localtime( &rawtime );

	strftime( buffer, sizeof( buffer ), "%d-%m-%Y %H:%M:%S", timeinfo );
	std::string str( buffer );

	if( json::save( [this] { return m_pattern.getValue(); },
					Folders::Bullets + m_pattern.name + ".bullet" ) )
		Debug::addMessage(
			Utils::format( "Saved \"%s\" at %s",
						   ( Folders::Bullets + m_pattern.name + ".bullet" ).c_str(),
						   str.c_str() ),
			DebugType::Info );

	m_unsaved = false;
	m_saved	  = true;
}

//--------------------------------------------------------------------------------

}	 // namespace Editor

//================================================================================
