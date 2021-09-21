//================================================================================

#include "BulletPatternEditor.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "Debug.h"

#include "Particle.h"
#include "Emitter.h"
#include "Patterns.h"

//================================================================================

void drawSpacer() {
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

//--------------------------------------------------------------------------------

void drawDoubleSpacer() {
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();
}

//--------------------------------------------------------------------------------

void drawHeader( const char* label ) {
	ImGui::SetWindowFontScale( 1.1f );
	ImGui::Text( label );
	ImGui::SetWindowFontScale( 1.0f );
	drawSpacer();
}

//--------------------------------------------------------------------------------

bool drawValueSet( Math::ValueSet< int >& set, const char* id, bool slider = false, int min = std::numeric_limits< int >::min(), int max = std::numeric_limits< int >::max() ) {
	bool out = false;

	ImGui::PushID( id );

	out |= ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		if( slider ) {
			out |= ImGui::DragInt( "min", &set.min, 1.0f, min, max );
			out |= ImGui::DragInt( "max", &set.max, 1.0f, min, max );
		}
		else {
			out |= ImGui::InputInt( "min", &set.min );
			if( set.min < min )
				set.min = min;
			if( set.min > max )
				set.min = max;

			out |= ImGui::InputInt( "max", &set.max );
			if( set.max < min )
				set.max = min;
			if( set.max > max )
				set.max = max;
		}
	}
	else {
		if( slider )
			out |= ImGui::DragInt( "", &set.min, 1.0f, min, max );
		else {
			out |= ImGui::InputInt( "", &set.min );
			if( set.min < min )
				set.min = min;
			if( set.min > max )
				set.min = max;
		}
	}

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawValueSet( Math::ValueSet< float >& set, const char* id, bool slider = false, float min = std::numeric_limits< float >::min(), float max = std::numeric_limits< float >::max() ) {
	bool out = false;

	ImGui::PushID( id );

	out |= ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		if( slider ) {
			out |= ImGui::DragFloat( "min", &set.min, 1.0f, min, max );
			out |= ImGui::DragFloat( "max", &set.max, 1.0f, min, max );
		}
		else {
			out |= ImGui::InputFloat( "min", &set.min );
			if( set.min < min )
				set.min = min;
			if( set.min > max )
				set.min = max;

			out |= ImGui::InputFloat( "max", &set.max );
			if( set.max < min )
				set.max = min;
			if( set.max > max )
				set.max = max;
		}
	}
	else {
		if( slider )
			out |= ImGui::DragFloat( "", &set.min, 1.0f, min, max );
		else {
			out |= ImGui::InputFloat( "", &set.min );
			if( set.min < min )
				set.min = min;
			if( set.min > max )
				set.min = max;
		}
	}

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawValueSet( Math::ValueSet< Math::Vec2 >& set, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	out |= ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		out |= ImGui::InputFloat2( "min", &set.min.x );
		out |= ImGui::InputFloat2( "max", &set.max.x );
	}
	else {
		out |= ImGui::InputFloat2( "", &set.min.x );
	}

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawValueSet( Math::ValueSet< Math::Color >& set, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	out |= ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		out |= ImGui::ColorEdit4( "min", &set.min.r );
		out |= ImGui::ColorEdit4( "max", &set.max.r );
	}
	else
	{
		out |= ImGui::ColorEdit4( "", &set.min.r );
	}

	ImGui::PopID();

	return out;
}

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

void BulletPatternEditor::onSpawnChildren() {
	m_defaultPattern.initial.size.min = 1.0f;
	m_defaultPattern.initial.lifetime.min = 1000;
	m_defaultPattern.initial.number.min = 1;
	m_defaultPattern.initial.color.min = Colors::WHITE;
	m_defaultPattern.fade.acceleration.start.min = 1.0f;
	m_defaultPattern.fade.color.target.min.a = 0.0f;
	m_defaultPattern.fade.size.start.min = 1.0f;
	m_defaultPattern.fade.velocity.start.min = 1.0f;

	m_defaultEmitter.multipliers.acceleration.min = 1.0f;
	m_defaultEmitter.multipliers.alpha.min = 1.0f;
	m_defaultEmitter.multipliers.lifetime.min = 1.0f;
	m_defaultEmitter.multipliers.number.min = 1.0f;
	m_defaultEmitter.multipliers.size.min = 1.0f;
	m_defaultEmitter.multipliers.velocity.min = 1.0f;

	m_basePattern = m_defaultPattern;
	m_baseEmitter = m_defaultEmitter;

	if( m_emitter )
		m_tabs.push_back( Tab{ true, &m_baseEmitter } );
	else
		m_tabs.push_back( Tab{ false, &m_basePattern } );

	m_renameBuffer = new char[ 66565 ];
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::onUpdate( sf::Time deltaTime ) {
	if( m_mouseDown ) {
		Math::Vec2 mousePos = System::getWindow()->mapPixelToCoords( sf::Mouse::getPosition() );
		Math::Vec2 diff = m_mouseOrigin - mousePos;
		m_camera.setPosition( m_camera.getPosition() + diff );
	}

	if( m_changed ) {
		Gfx::Emitter::destroyAll();
		Gfx::Particle::killAll();

		if( !sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
			if( m_emitter )
				Gfx::Emitter::spawn( this, m_baseEmitter );
			else
				Gfx::Particle::spawn( this, m_basePattern );
			m_changed = false;
		}
	}

	if( Gfx::Particle::getAll().size() + Gfx::Emitter::getAll().size() == 0u ) {
		if( m_emitter )
			Gfx::Emitter::spawn( this, m_baseEmitter );
		else
			Gfx::Particle::spawn( this, m_basePattern );
	}
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::onEvent( sf::Event e ) {
	if( m_useMouse ) {

		if( e.type == sf::Event::MouseWheelScrolled ) {
			if( e.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel ) {
				m_camera.setDistance( m_camera.getDistance() - e.mouseWheelScroll.delta * 16.0f );
				return;
			}
		}

		if( e.type == sf::Event::MouseButtonPressed ) {
			if( e.mouseButton.button == sf::Mouse::Left ) {
				m_mouseOrigin = System::getWindow()->mapPixelToCoords( sf::Mouse::getPosition() );
				m_mouseDown = true;
				return;
			}
		}

	}

	if( e.type == sf::Event::MouseButtonReleased ) {
		if( e.mouseButton.button == sf::Mouse::Left ) {
			m_mouseDown = false;
			return;
		}
	}

	if( e.type == sf::Event::KeyPressed ) {
		if( e.key.control && e.key.code == sf::Keyboard::S ) {
			Tab& tab = *m_tabs.rbegin();
			if( tab.emitter ) {
				Gfx::Emitter::Pattern* pattern = static_cast< Gfx::Emitter::Pattern* >( tab.pattern );
				Gfx::saveEmitter( *pattern, pattern->name );
			}
			else {
				Gfx::Particle::Pattern* pattern = static_cast< Gfx::Particle::Pattern* >( tab.pattern );
				Gfx::savePattern( *pattern, pattern->name );
			}
		}
	}
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::onDestroy() {
	delete[] m_renameBuffer;
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::onRender() {

	sf::RenderWindow* window = System::getWindow();

	for( int i = 10000; i >= 0; --i ) {
		if( i == 0 ) {
			sf::Vertex line[ 2 ];
			line[ 0 ].color = sf::Color( 255, 255, 255, 255 );
			line[ 1 ].color = sf::Color( 255, 255, 255, 255 );

			line[ 0 ].position = sf::Vector2f( float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			continue;
		}

		if( i % 125 == 0 ) {
			sf::Vertex line[ 2 ];
			line[ 0 ].color = sf::Color( 125, 125, 125, 255 );
			line[ 1 ].color = sf::Color( 125, 125, 125, 255 );

			line[ 0 ].position = sf::Vector2f( float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( -float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( -float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( 10000.0f, -float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, -float( i ) );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			continue;
		}

		if( i % 25 == 0 ) {
			sf::Vertex line[ 2 ];
			line[ 0 ].color = sf::Color( 50, 50, 50, 255 );
			line[ 1 ].color = sf::Color( 50, 50, 50, 255 );

			line[ 0 ].position = sf::Vector2f( float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( -float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( -float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( 10000.0f, -float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, -float( i ) );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			continue;
		}

		if( i % 5 == 0 ) {
			sf::Vertex line[ 2 ];
			line[ 0 ].color = sf::Color( 10, 10, 10, 255 );
			line[ 1 ].color = sf::Color( 10, 10, 10, 255 );

			line[ 0 ].position = sf::Vector2f( float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( -float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( -float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			line[ 0 ].position = sf::Vector2f( 10000.0f, -float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, -float( i ) );
			window->draw( line, 2, sf::LineStrip );
			Debug::incDrawCall();

			continue;
		}
	}

	ImGui::PushID( "PatternEditor" );

	// Properties
	ImGui::Begin( "Properties", NULL, 
				  ImGuiWindowFlags_NoTitleBar | 
				  ImGuiWindowFlags_NoResize | 
				  ImGuiWindowFlags_NoMove | 
				  ImGuiWindowFlags_NoCollapse );

	ImGui::SetWindowSize( ImVec2( 300.0f, float( System::getSystemInfo().height - 35 ) ) );
	ImGui::SetWindowPos( ImVec2( 0.0f, 35.0f ) );

	ImGui::SetWindowFontScale( 1.2f );
	ImGui::Text( "PROPERTIES" );
	ImGui::SetWindowFontScale( 1.0f );

	drawDoubleSpacer();

	m_patternID = 0;

	const Tab tab = m_tabs.at( m_tabs.size() - 1u );
	if( tab.emitter ) {
		Gfx::Emitter::Pattern* pattern = static_cast< Gfx::Emitter::Pattern* >( tab.pattern );
		renderEmitter( *pattern );
	}
	else {
		Gfx::Particle::Pattern* pattern = static_cast< Gfx::Particle::Pattern* >( tab.pattern );
		renderPattern( *pattern );
	}

	m_useMouse = !ImGui::GetIO().WantCaptureMouse;

	ImGui::End();

	// Tabs
	ImGui::Begin( "Tabs", NULL,
				  ImGuiWindowFlags_NoTitleBar |
				  ImGuiWindowFlags_NoResize |
				  ImGuiWindowFlags_NoMove |
				  ImGuiWindowFlags_NoCollapse | 
				  ImGuiWindowFlags_NoScrollbar | 
				  ImGuiWindowFlags_NoScrollWithMouse );

	ImGui::SetWindowSize( ImVec2( float( System::getSystemInfo().width ), 30.0f ) );
	ImGui::SetWindowPos( ImVec2( 0.0f, 0.0f ) );

	string tabName;
	if( m_emitter )
		tabName = m_baseEmitter.name;
	else
		tabName = m_basePattern.name;

	if( ImGui::Button( tabName.c_str() ) ) {
		m_tabs.clear();
		
		if( m_emitter )
			m_tabs.push_back( Tab{ true, &m_baseEmitter } );
		else
			m_tabs.push_back( Tab{ false, &m_basePattern } );
	}

	for( size_t i = 1; i < m_tabs.size(); ++i ) {
		ImGui::SameLine();
		ImGui::Text( " > " );
		ImGui::SameLine();

		const Tab tab = m_tabs.at( i );
		string name;
		if( tab.emitter )
			name = static_cast< Gfx::Emitter::Pattern* >( tab.pattern )->name;
		else
			name = static_cast< Gfx::Particle::Pattern* >( tab.pattern )->name;
		
		tabName = name + "##" + std::to_string( i );
		if( ImGui::Button( tabName.c_str() ) ) {
			while( m_tabs.size() > i + 1 ) {
				m_tabs.erase( m_tabs.end() - 1 );
			}

			break;
		}
	}

	ImGui::End();

	// Performance Window
	ImGui::Begin( "Performance", NULL, 
				  ImGuiWindowFlags_AlwaysAutoResize | 
				  ImGuiWindowFlags_NoTitleBar | 
				  ImGuiWindowFlags_NoResize | 
				  ImGuiWindowFlags_NoMove | 
				  ImGuiWindowFlags_NoCollapse );

	ImGui::SetWindowPos( ImVec2( float( System::getSystemInfo().width ) - ImGui::GetWindowSize().x, 35.0f ) );
	if( ImGui::Button( "Edit UI Settings" ) ) {
		m_styleEditorOpen = !m_styleEditorOpen;
	}

	if( m_styleEditorOpen ) {
		ImGui::ShowStyleEditor();
	}

	char format[] =
		"FPS: %.0f\n"
		"Particles: %i\n"
		"Emitters: %i";

	char buffer[ 1024 ];
	sprintf_s( buffer, format, 1.0f / System::getDeltaTime().asSeconds(), Gfx::Particle::getAll().size(), Gfx::Emitter::getAll().size() );
	ImGui::Text( buffer );

	

	ImGui::End();

	renderPatternTreeView();

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPattern( Gfx::Particle::Pattern& pattern ) {
	ImGui::PushID( m_patternID++ );

	ImGui::Text( pattern.name.c_str() );

	if( ImGui::Button( "Rename" ) ) {
		ImGui::OpenPopup( "Rename" );
		delete[] m_renameBuffer;
		m_renameBuffer = new char[ 66565 ];
		strcpy_s( m_renameBuffer, 66565, pattern.name.c_str() );
	}

	ImGui::SameLine();

	if( ImGui::Button( "Save" ) )
		Gfx::savePattern( pattern, pattern.name );

	ImGui::SameLine();

	if( ImGui::Button( "Load" ) ) {
		pattern = Gfx::loadPattern( pattern.name );
		m_changed = true;
	}

	if( ImGui::BeginPopup( "Rename", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse ) ) {
		if( !ImGui::IsWindowFocused() )
			ImGui::CloseCurrentPopup();

		if( ImGui::InputText( "Rename", m_renameBuffer, 65565, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
			pattern.name = m_renameBuffer;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	drawDoubleSpacer();

	ImGui::Text( "Lifetime" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.lifetime, "Lifetime" );


	drawDoubleSpacer();

	ImGui::Text( "Number" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.number, "Number" );


	drawDoubleSpacer();

	ImGui::Text( "Direction" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.direction, "Direction" );

	drawDoubleSpacer();

	ImGui::Text( "Velocity" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.velocity, "Velocity" );

	ImGui::Spacing();

	ImGui::PushID( "Velocity" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {

		m_changed |= ImGui::Checkbox( "X", &pattern.fade.velocity.x );
		ImGui::SameLine();
		m_changed |= ImGui::Checkbox( "Y", &pattern.fade.velocity.y );

		ImGui::Text( "Start" );
		m_changed |= drawValueSet( pattern.fade.velocity.start, "FadeVelocityStart" );
		ImGui::Text( "End" );
		m_changed |= drawValueSet( pattern.fade.velocity.end, "FadeVelocityEnd" );
		ImGui::TreePop();
	}
	ImGui::PopID();

	drawDoubleSpacer();

	ImGui::Text( "Acceleration" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.acceleration, "Acceleration" );

	ImGui::Spacing();

	ImGui::PushID( "Acceleration" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {

		m_changed |= ImGui::Checkbox( "X", &pattern.fade.acceleration.x );
		ImGui::SameLine();
		m_changed |= ImGui::Checkbox( "Y", &pattern.fade.acceleration.y );

		ImGui::Text( "Start" );
		m_changed |= drawValueSet( pattern.fade.acceleration.start, "FadeAccelerationStart" );
		ImGui::Text( "End" );
		m_changed |= drawValueSet( pattern.fade.acceleration.end, "FadeAccelerationEnd" );

		ImGui::TreePop();
	}
	ImGui::PopID();

	drawDoubleSpacer();

	ImGui::Text( "Size" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.size, "Size", false, 0.0f, std::numeric_limits< float >::max() );

	ImGui::Spacing();

	ImGui::PushID( "Size" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {

		m_changed |= ImGui::Checkbox( "Fade", &pattern.fade.size.active );

		ImGui::Text( "Start" );
		m_changed |= drawValueSet( pattern.fade.size.start, "FadeSizeStart", false, 0.0f, std::numeric_limits< float >::max() );
		ImGui::Text( "End" );
		m_changed |= drawValueSet( pattern.fade.size.end, "FadeSizeEnd", false, 0.0f, std::numeric_limits< float >::max() );

		ImGui::TreePop();
	}
	ImGui::PopID();

	drawDoubleSpacer();

	ImGui::Text( "Color" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.color, "Color" );

	ImGui::Spacing();

	ImGui::PushID( "Color" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= ImGui::Checkbox( "R", &pattern.fade.color.r );
		ImGui::SameLine();
		m_changed |= ImGui::Checkbox( "G", &pattern.fade.color.g );
		ImGui::SameLine();
		m_changed |= ImGui::Checkbox( "B", &pattern.fade.color.b );
		ImGui::SameLine();
		m_changed |= ImGui::Checkbox( "A", &pattern.fade.color.a );

		ImGui::Text( "Target" );
		m_changed |= drawValueSet( pattern.fade.color.target, "ColorTarget" );

		ImGui::TreePop();
	}
	ImGui::PopID();

	drawDoubleSpacer();

	ImGui::Text( "Emitters" );

	for( size_t i = 0; i < pattern.emitters.size(); ++i ) {
		Gfx::Emitter::Pattern& emitter = pattern.emitters.at( i );

		ImGui::Text( emitter.name.c_str() );

		// Edit
		ImGui::SameLine();
		string buttonName = "Edit##" + std::to_string( i );
		if( ImGui::Button( buttonName.c_str() ) )
			m_tabs.push_back( Tab{ true, &emitter } );

		// Remove
		ImGui::SameLine();
		buttonName = "Remove##" + std::to_string( i );
		if( ImGui::Button( buttonName.c_str() ) ) {
			m_changed = true;
			pattern.emitters.erase( pattern.emitters.begin() + i );
			i--;
			continue;
		}
	}

	if( ImGui::Button( "Add Emitter" ) ) {
		pattern.emitters.push_back( m_defaultEmitter );
		m_changed = true;
	}

	drawDoubleSpacer();

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderEmitter( Gfx::Emitter::Pattern& emitter ) {
	ImGui::PushID( ++m_patternID );

	ImGui::Text( emitter.name.c_str() );

	if( ImGui::Button( "Rename" ) ) {
		ImGui::OpenPopup( "Rename" );
		delete[] m_renameBuffer;
		m_renameBuffer = new char[ 66565 ];
		strcpy_s( m_renameBuffer, 66565, emitter.name.c_str() );
	}

	ImGui::SameLine();

	if( ImGui::Button( "Save" ) )
		Gfx::saveEmitter( emitter, emitter.name );

	ImGui::SameLine();

	if( ImGui::Button( "Load" ) ) {
		emitter = Gfx::loadEmitter( emitter.name );
		m_changed = true;
	}

	if( ImGui::BeginPopup( "Rename", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse ) ) {
		if( !ImGui::IsWindowFocused() )
			ImGui::CloseCurrentPopup();

		if( ImGui::InputText( "Rename", m_renameBuffer, 65565, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
			emitter.name = m_renameBuffer;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	drawDoubleSpacer();

	ImGui::Text( "Properties" );
	drawSpacer();


	if( ImGui::TreeNodeEx( "Position", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.position, "Position" );
		ImGui::TreePop();
	}

	if( ImGui::TreeNodeEx( "Activation", ImGuiTreeNodeFlags_Framed ) ) {
		string buttonName;
		if( emitter.activation.type == Gfx::Emitter::Pattern::Activation::Type::OnSpawn )
			buttonName = "On Spawn";
		else if( emitter.activation.type == Gfx::Emitter::Pattern::Activation::Type::Alpha )
			buttonName = "Alpha";
		else if( emitter.activation.type == Gfx::Emitter::Pattern::Activation::Type::OnDestruction )
			buttonName = "On Destruction";

		if( ImGui::Button( buttonName.c_str() ) )
			ImGui::OpenPopup( "##ActivationType" );

		if( ImGui::BeginPopup( "##ActivationType",
							   ImGuiWindowFlags_NoTitleBar |
							   ImGuiWindowFlags_NoResize |
							   ImGuiWindowFlags_NoMove |
							   ImGuiWindowFlags_NoCollapse |
							   ImGuiWindowFlags_NoScrollbar |
							   ImGuiWindowFlags_NoScrollWithMouse |
							   ImGuiWindowFlags_AlwaysAutoResize ) ) {
			ImGui::PushID( "ActivationType" );

			if( ImGui::Button( "On Spawn" ) ) {
				emitter.activation.type = Gfx::Emitter::Pattern::Activation::Type::OnSpawn;
				m_changed = true;
				ImGui::CloseCurrentPopup();
			}
			else if( ImGui::Button( "Alpha" ) ) {
				emitter.activation.type = Gfx::Emitter::Pattern::Activation::Type::Alpha;
				m_changed = true;
				ImGui::CloseCurrentPopup();
			}
			else if( ImGui::Button( "On Destruction" ) ) {
				emitter.activation.type = Gfx::Emitter::Pattern::Activation::Type::OnDestruction;
				m_changed = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopID();
			ImGui::EndPopup();
		}

		if( emitter.activation.type == Gfx::Emitter::Pattern::Activation::Type::Alpha ) {
			ImGui::Text( "Start" );
			m_changed |= drawValueSet( emitter.activation.start, "Start", true, 0.0f, 1.0f );
			ImGui::Text( "End" );
			m_changed |= drawValueSet( emitter.activation.end, "End", true, 0.0f, 1.0f );
		}
		ImGui::TreePop();
	}

	if( ImGui::TreeNodeEx( "Spawn Rate", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= ImGui::Checkbox( "Fade##SpawnRate", &emitter.rate.fade );

		if( emitter.rate.fade ) {
			ImGui::Text( "Start" );
			m_changed |= drawValueSet( emitter.rate.start, "SpawnRateStart" );
			ImGui::Text( "End" );
			m_changed |= drawValueSet( emitter.rate.end, "SpawnRateEnd" );
		}
		else {
			m_changed |= drawValueSet( emitter.rate.start, "SpawnRate" );
		}
		ImGui::TreePop();
	}

	drawSpacer();
	ImGui::Text( "Multipliers" );
	drawSpacer();

	if( ImGui::TreeNodeEx( "Lifetime", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.multipliers.lifetime, "Lifetime" );
		ImGui::TreePop();
	}

	if( ImGui::TreeNodeEx( "Number", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.multipliers.number, "Number" );
		ImGui::TreePop();
	}

	if( ImGui::TreeNodeEx( "Velocity", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.multipliers.velocity, "Velocity" );
		ImGui::TreePop();
	}

	if( ImGui::TreeNodeEx( "Acceleration", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.multipliers.acceleration, "Acceleration" );
		ImGui::TreePop();
	}

	if( ImGui::TreeNodeEx( "Size", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.multipliers.size, "Size" );
		ImGui::TreePop();
	}

	if( ImGui::TreeNodeEx( "Alpha", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.multipliers.alpha, "Alpha" );
		ImGui::TreePop();
	}

	drawSpacer();
	ImGui::Text( "Patterns" );
	drawSpacer();

	for( size_t i = 0; i < emitter.patterns.size(); ++i ) {
		Gfx::Particle::Pattern& pattern = emitter.patterns.at( i );

		ImGui::Text( pattern.name.c_str() );

		// Edit
		ImGui::SameLine();
		string buttonName = "Edit##" + std::to_string( i );
		if( ImGui::Button( buttonName.c_str() ) )
			m_tabs.push_back( Tab{ false, &pattern } );

		// Remove
		ImGui::SameLine();
		buttonName = "Remove##" + std::to_string( i );
		if( ImGui::Button( buttonName.c_str() ) ) {
			m_changed = true;
			emitter.patterns.erase( emitter.patterns.begin() + i );
			i--;
			continue;
		}
	}

	if( ImGui::Button( "Add Pattern" ) ) {
		emitter.patterns.push_back( m_defaultPattern );
		m_changed = true;
	}

	drawSpacer();
	ImGui::Text( "Pattern Sets" );
	drawSpacer();

	ImGui::Indent();
	for( size_t i = 0; i < emitter.sets.size(); ++i ) {
		ImGui::PushID( int( i ) );

		Gfx::Particle::PatternSet& set = emitter.sets.at( i );

		m_changed |= ImGui::Checkbox( "Random", &set.random );

		ImGui::Spacing();

		for( size_t j = 0; j < set.patterns.size(); ++j ) {
			Gfx::Particle::Pattern& pattern = set.patterns.at( j );

			ImGui::Text( pattern.name.c_str() );

			// Edit
			ImGui::SameLine();
			string buttonName = "Edit##" + std::to_string( j );
			if( ImGui::Button( buttonName.c_str() ) )
				m_tabs.push_back( Tab{ false, &pattern } );

			// Remove
			ImGui::SameLine();
			buttonName = "Remove##" + std::to_string( i );
			if( ImGui::Button( buttonName.c_str() ) ) {
				m_changed = true;
				set.patterns.erase( set.patterns.begin() + i );
				i--;
				continue;
			}
		}

		if( ImGui::Button( "Add Pattern" ) ) {
			set.patterns.push_back( m_defaultPattern );
			m_changed = true;
		}

		drawSpacer();

		ImGui::PopID();
	}
	ImGui::Unindent();

	if( ImGui::Button( "Add Pattern Set" ) ) {
		emitter.sets.push_back( Gfx::Particle::PatternSet() );
		m_changed = true;
	}

	drawDoubleSpacer();

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPatternTreeView() {
	ImGui::Begin( "Patterns", NULL,
				  ImGuiWindowFlags_NoTitleBar |
				  ImGuiWindowFlags_NoResize |
				  ImGuiWindowFlags_NoMove |
				  ImGuiWindowFlags_NoCollapse );

	ImGui::SetWindowSize( ImVec2( 300.0f, float( System::getSystemInfo().height ) / 2.0f ) );
	ImGui::SetWindowPos( ImVec2( float( System::getSystemInfo().width ) - ImGui::GetWindowSize().x, 
								 float( System::getSystemInfo().height ) - ImGui::GetWindowSize().y ) );

	m_patternTreeID = 0;

	vector< Tab > stack;
	if( m_emitter )
		renderEmitterTreeNode( m_baseEmitter, stack );
	else
		renderPatternTreeNode( m_basePattern, stack );

	ImGui::End();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPatternTreeNode( Gfx::Particle::Pattern& pattern, vector< Tab > stack ) {
	stack.push_back( Tab{ false, &pattern } );
	
	if( ImGui::Button( ( pattern.name + "###" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
		m_tabs = stack;
	}

	if( stack.size() > 1u ) {
		ImGui::SameLine();
		if( ImGui::Button( ( "Remove##" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
			Gfx::Emitter::Pattern* parent = static_cast< Gfx::Emitter::Pattern* >( stack.at( stack.size() - 2u ).pattern );
			
			bool flag = false;
			for( size_t i = 0; i < parent->patterns.size(); ++i )
				if( &parent->patterns.at( i ) == &pattern ) {
					parent->patterns.erase( parent->patterns.begin() + i );
					flag = true;
				}

			if( !flag ) {
				for( Gfx::Particle::PatternSet& set : parent->sets ) {
					for( size_t i = 0; i < set.patterns.size(); ++i ) {
						if( &set.patterns.at( i ) == &pattern ) {
							set.patterns.erase( set.patterns.begin() + i );
							flag = true;
							break;
						}
					}

					if( flag )
						break;
				}
			}
		}
	}

	ImGui::Indent();

	for( Gfx::Emitter::Pattern& emitter : pattern.emitters )
		renderEmitterTreeNode( emitter, stack );

	ImGui::Unindent();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderEmitterTreeNode( Gfx::Emitter::Pattern& emitter, vector< Tab > stack ) {
	stack.push_back( Tab{ true, &emitter } );



	if( ImGui::Button( ( emitter.name + "###" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
		m_tabs = stack;
	}

	if( stack.size() > 1u ) {
		ImGui::SameLine();
		if( ImGui::Button( ( "Remove##" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
			Gfx::Particle::Pattern* parent = static_cast< Gfx::Particle::Pattern* >( stack.at( stack.size() - 2u ).pattern );
			
			for( size_t i = 0; i < parent->emitters.size(); ++i ) {
				if( &parent->emitters.at( i ) == &emitter ) {
					parent->emitters.erase( parent->emitters.begin() + i );
					break;
				}
			}
		}
	}

	ImGui::Indent();

	for( Gfx::Particle::Pattern& pattern : emitter.patterns )
			renderPatternTreeNode( pattern, stack );

	for( Gfx::Particle::PatternSet& set : emitter.sets ) {
		if( set.random )
			ImGui::Text( "Random Set" );
		else
			ImGui::Text( "Sequence" );

		ImGui::Indent();
		for( Gfx::Particle::Pattern& pattern : set.patterns ) {
			renderPatternTreeNode( pattern, stack );
		}
		ImGui::Unindent();
	}

	ImGui::Unindent();
}

//--------------------------------------------------------------------------------

}

//================================================================================