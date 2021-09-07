//================================================================================

#include "BulletPatternEditor.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "Debug.h"

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

bool drawValueSet( Gfx::Particle::ValueSet< int >& set, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	out |= ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		out |= ImGui::InputInt( "min", &set.min );
		out |= ImGui::InputInt( "max", &set.max );
	}
	else {
		out |= ImGui::InputInt( "", &set.min );
	}

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawValueSet( Gfx::Particle::ValueSet< float >& set, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	out |= ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		out |= ImGui::InputFloat( "min", &set.min );
		out |= ImGui::InputFloat( "max", &set.max );
	}
	else {
		out |= ImGui::InputFloat( "", &set.min );
	}

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawValueSet( Gfx::Particle::ValueSet< Math::Vec2 >& set, const char* id ) {
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

bool drawValueSet( Gfx::Particle::ValueSet< Math::Color >& set, const char* id ) {
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
	shared_ptr< Debug::MessageWindow > window = makeObject< Debug::MessageWindow >( this );
	window->setVisibility( false );

	m_defaultPattern.initial.lifetime.min = 1000;
	m_defaultPattern.initial.number.min = 1;
	m_defaultPattern.initial.color.min = Math::Color( 1.0f, 1.0f, 1.0f, 1.0f );
	m_defaultPattern.initial.size.min = 1.f;

	m_defaultEmitter.lifetime.min = 1.f;
	m_defaultEmitter.velocity.min = 1.f;
	m_defaultEmitter.acceleration.min = 1.f;
	m_defaultEmitter.size.min = 1.f;
	m_defaultEmitter.alpha.min = 1.f;
	m_defaultEmitter.number.min = 1.f;

	m_basePattern = m_defaultPattern;
	m_tabs.push_back( &m_basePattern );

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
		Gfx::Particle::killParticles();

		if( !sf::Mouse::isButtonPressed( sf::Mouse::Left ) ) {
			Gfx::Particle::spawnParticle( this, m_basePattern );
			m_changed = false;
		}
	}

	if( Gfx::Particle::getParticles().size() == 0u ) {
		Gfx::Particle::spawnParticle( this, m_basePattern );
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

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );

			continue;
		}

		if( i % 125 == 0 ) {
			sf::Vertex line[ 2 ];
			line[ 0 ].color = sf::Color( 125, 125, 125, 255 );
			line[ 1 ].color = sf::Color( 125, 125, 125, 255 );

			line[ 0 ].position = sf::Vector2f( float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( -float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( -float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( 10000.0f, -float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, -float( i ) );
			window->draw( line, 2, sf::LineStrip );

			continue;
		}

		if( i % 25 == 0 ) {
			sf::Vertex line[ 2 ];
			line[ 0 ].color = sf::Color( 50, 50, 50, 255 );
			line[ 1 ].color = sf::Color( 50, 50, 50, 255 );

			line[ 0 ].position = sf::Vector2f( float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( -float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( -float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( 10000.0f, -float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, -float( i ) );
			window->draw( line, 2, sf::LineStrip );

			continue;
		}

		if( i % 5 == 0 ) {
			sf::Vertex line[ 2 ];
			line[ 0 ].color = sf::Color( 10, 10, 10, 255 );
			line[ 1 ].color = sf::Color( 10, 10, 10, 255 );

			line[ 0 ].position = sf::Vector2f( float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( -float( i ), 10000.0f );
			line[ 1 ].position = sf::Vector2f( -float( i ), -10000.0f );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( 10000.0f, float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, float( i ) );
			window->draw( line, 2, sf::LineStrip );

			line[ 0 ].position = sf::Vector2f( 10000.0f, -float( i ) );
			line[ 1 ].position = sf::Vector2f( -10000.0f, -float( i ) );
			window->draw( line, 2, sf::LineStrip );

			continue;
		}
	}

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
	renderPattern( *m_tabs.at( m_tabs.size() - 1u ) );

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

	string tabName = m_basePattern.name + "##0";
	if( ImGui::Button( tabName.c_str() ) ) {
		m_tabs.clear();
		m_tabs.push_back( &m_basePattern );
	}

	for( size_t i = 1; i < m_tabs.size(); ++i ) {
		ImGui::SameLine();
		ImGui::Text( " > " );
		ImGui::SameLine();
		
		tabName = m_tabs.at( i )->name + "##" + std::to_string( i );
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
		"Particles: %i";

	char buffer[ 1024 ];
	sprintf_s( buffer, format, 1.0f / System::getDeltaTime().asSeconds(), Gfx::Particle::getParticles().size() );
	ImGui::Text( buffer );

	ImGui::End();

	renderPatternTreeView();
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
		Gfx::Particle::savePattern( pattern, pattern.name );

	ImGui::SameLine();

	if( ImGui::Button( "Load" ) ) {
		pattern = Gfx::Particle::loadPattern( pattern.name );
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

	drawSpacer();

	ImGui::Text( "Lifetime" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.lifetime, "Lifetime" );


	drawSpacer();

	ImGui::Text( "Number" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.number, "Number" );


	drawSpacer();

	ImGui::Text( "Direction" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.direction, "Direction" );

	drawSpacer();

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

	drawSpacer();

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

	drawSpacer();

	ImGui::Text( "Size" );
	ImGui::Spacing();
	m_changed |= drawValueSet( pattern.initial.size, "Size" );

	ImGui::Spacing();

	ImGui::PushID( "Size" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {

		m_changed |= ImGui::Checkbox( "Fade", &pattern.fade.size.active );

		ImGui::Text( "Start" );
		m_changed |= drawValueSet( pattern.fade.size.start, "FadeSizeStart" );
		ImGui::Text( "End" );
		m_changed |= drawValueSet( pattern.fade.size.end, "FadeSizeEnd" );

		ImGui::TreePop();
	}
	ImGui::PopID();

	drawSpacer();

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

	drawSpacer();

	ImGui::Text( "Emitters" );
	ImGui::Spacing();

	for( Gfx::Particle::Pattern::Emitter& emitter : pattern.emitters ) {
		renderEmitter( emitter );
		drawSpacer();
	}

	if( ImGui::Button( "Add Emitter" ) ) {
		pattern.emitters.push_back( m_defaultEmitter );
		m_changed = true;
	}

	drawDoubleSpacer();

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderEmitter( Gfx::Particle::Pattern::Emitter& emitter ) {
	ImGui::PushID( ++m_patternID );

	if( ImGui::TreeNodeEx( "Emitter", ImGuiTreeNodeFlags_Framed ) ) {
		if( ImGui::TreeNodeEx( "Position", ImGuiTreeNodeFlags_Framed ) ) {
			m_changed |= drawValueSet( emitter.position, "Position" );
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Start Time", ImGuiTreeNodeFlags_Framed ) ) {
			m_changed |= drawValueSet( emitter.startTime, "Start Time" );
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
				m_changed |= drawValueSet( emitter.rate.start, "Spawn Rate" );
			}
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Multipliers", ImGuiTreeNodeFlags_Framed ) ) {
			if( ImGui::TreeNodeEx( "Lifetime", ImGuiTreeNodeFlags_Framed ) ) {
				m_changed |= drawValueSet( emitter.lifetime, "Lifetime" );
				ImGui::TreePop();
			}

			if( ImGui::TreeNodeEx( "Number", ImGuiTreeNodeFlags_Framed ) ) {
				m_changed |= drawValueSet( emitter.number, "Number" );
				ImGui::TreePop();
			}

			if( ImGui::TreeNodeEx( "Velocity", ImGuiTreeNodeFlags_Framed ) ) {
				m_changed |= drawValueSet( emitter.velocity, "Velocity" );
				ImGui::TreePop();
			}

			if( ImGui::TreeNodeEx( "Acceleration", ImGuiTreeNodeFlags_Framed ) ) {
				m_changed |= drawValueSet( emitter.acceleration, "Acceleration" );
				ImGui::TreePop();
			}

			if( ImGui::TreeNodeEx( "Size", ImGuiTreeNodeFlags_Framed ) ) {
				m_changed |= drawValueSet( emitter.size, "Size" );
				ImGui::TreePop();
			}

			if( ImGui::TreeNodeEx( "Alpha", ImGuiTreeNodeFlags_Framed ) ) {
				m_changed |= drawValueSet( emitter.alpha, "Alpha" );
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Patterns", ImGuiTreeNodeFlags_Framed ) ) {
			for( size_t i = 0; i < emitter.patterns.size(); ++i ) {
				Gfx::Particle::Pattern& pattern = emitter.patterns.at( i );

				ImGui::Text( pattern.name.c_str() );

				// Edit
				ImGui::SameLine();
				string buttonName = "Edit##" + std::to_string( i );
				if( ImGui::Button( buttonName.c_str() ) ) {
					m_tabs.push_back( &pattern );
				}

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

			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

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

	vector< Gfx::Particle::Pattern* > stack;
	renderPatternTreeNode( m_basePattern, stack );

	ImGui::End();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPatternTreeNode( Gfx::Particle::Pattern& pattern, vector< Gfx::Particle::Pattern* > stack ) {
	stack.push_back( &pattern );
	
	if( ImGui::Button( ( pattern.name + "###" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
		m_tabs = stack;
	}

	if( stack.size() > 1u ) {
		ImGui::SameLine();
		if( ImGui::Button( ( "Remove##" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
			bool flag = false;
			for( Gfx::Particle::Pattern::Emitter& emitter : stack.at( stack.size() - 2u )->emitters ) {
				for( size_t i = 0; i < emitter.patterns.size(); ++i ) {
					if( &emitter.patterns.at( i ) == &pattern ) {
						emitter.patterns.erase( emitter.patterns.begin() + i );
						flag = true;
						m_changed = true;
						break;
					}
				}

				if( flag ) 
					break;
			}
		}
	}

	ImGui::Indent();

	for( Gfx::Particle::Pattern::Emitter& emitter : pattern.emitters ) {
		for( Gfx::Particle::Pattern& pattern : emitter.patterns ) {
			renderPatternTreeNode( pattern, stack );
		}
	}

	ImGui::Unindent();
}

//--------------------------------------------------------------------------------

}

//================================================================================