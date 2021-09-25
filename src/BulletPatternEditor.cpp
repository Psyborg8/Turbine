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

bool drawValueSet( Math::ValueSet< int >& set, const char* id, bool inverse = false ) {
	bool out = false;

	ImGui::PushID( id );

	if( inverse ) {
		out |= ImGui::Checkbox( "Inverse", &set.inverse );
		ImGui::SameLine();
	}
	out |= ImGui::Checkbox( "Random", &set.random );
	if( set.random ) {
		ImGui::SameLine();
		out |= ImGui::Checkbox( "Lock", &set.lock );

	}

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

bool drawValueSet( Math::ValueSet< float >& set, const char* id, bool inverse = false ) {
	bool out = false;

	ImGui::PushID( id );

	if( inverse ) {
		out |= ImGui::Checkbox( "Inverse", &set.inverse );
		ImGui::SameLine();
	}
	out |= ImGui::Checkbox( "Random", &set.random );
	if( set.random ) {
		ImGui::SameLine();
		out |= ImGui::Checkbox( "Lock", &set.lock );

	}

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

bool drawValueSet( Math::ValueSet< Math::Vec2 >& set, const char* id, bool inverse = false ) {
	bool out = false;

	ImGui::PushID( id );

	if( inverse ) {
		out |= ImGui::Checkbox( "Inverse", &set.inverse );
		ImGui::SameLine();
	}
	out |= ImGui::Checkbox( "Random", &set.random );
	if( set.random ) {
		ImGui::SameLine();
		out |= ImGui::Checkbox( "Lock", &set.lock );

	}

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
		ImGui::SameLine();
		out |= ImGui::Checkbox( "Lock", &set.lock );
		ImGui::SameLine();
		out |= ImGui::Checkbox( "HSV", &set.hsv );
	}

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

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< float >& start, Math::ValueSet< float >& end, bool& active, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "Fade", &active );
		ImGui::Text( "Start" );
		out |= drawValueSet( start, "Start" );
		ImGui::Text( "End" );
		out |= drawValueSet( end, "End" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< float >& start, Math::ValueSet< float >& end, bool& x, bool& y, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "X", &x );
		ImGui::SameLine();
		out |= ImGui::Checkbox( "Y", &y );

		ImGui::Text( "Start" );
		out |= drawValueSet( start, "Start" );
		ImGui::Text( "End" );
		out |= drawValueSet( end, "End" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< float >& target, bool& active, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "Fade", &active );

		ImGui::Text( "Target" );
		out |= drawValueSet( target, "Target" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< Math::Vec2 >& start, Math::ValueSet< Math::Vec2 >& end, bool& active, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "Fade", &active );

		ImGui::Text( "Start" );
		out |= drawValueSet( start, "Start" );
		ImGui::Text( "End" );
		out |= drawValueSet( end, "End" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< Math::Vec2 >& target, bool& active, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "Fade", &active );

		ImGui::Text( "Target" );
		out |= drawValueSet( target, "Target" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< Math::Vec2 >& start, Math::ValueSet< Math::Vec2 >& end, bool& x, bool& y, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "X", &x );
		ImGui::SameLine();
		out |= ImGui::Checkbox( "Y", &y );
		
		ImGui::Text( "Start" );
		out |= drawValueSet( start, "Start" );
		ImGui::Text( "End" );
		out |= drawValueSet( end, "End" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< Math::Vec2 >& target, bool& x, bool& y, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "X", &x );
		ImGui::SameLine();
		out |= ImGui::Checkbox( "Y", &y );

		ImGui::Text( "Target" );
		out |= drawValueSet( target, "Target" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool drawFade( Math::ValueSet< Math::Color >& target, bool& r, bool& g, bool& b, bool& a, const char* id ) {
	bool out = false;

	ImGui::PushID( id );
	ImGui::PushID( "Fade" );
	if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
		out |= ImGui::Checkbox( "R", &r );
		ImGui::SameLine();
		out |= ImGui::Checkbox( "G", &g );
		ImGui::SameLine();
		out |= ImGui::Checkbox( "B", &b );
		ImGui::SameLine();
		out |= ImGui::Checkbox( "A", &a );

		ImGui::Text( "Target" );
		out |= drawValueSet( target, "Target" );

		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

	return out;
}

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

void BulletPatternEditor::onSpawnChildren() {
	if( m_emitter )
		m_tabs.push_back( Tab{ true, &m_baseEmitter } );
	else
		m_tabs.push_back( Tab{ false, &m_basePattern } );

	m_renameBuffer = new char[ 66565 ];

	// Grid
	m_grid.setPrimitiveType( sf::PrimitiveType::Lines );

	std::function< void( float, sf::Color, sf::VertexArray& ) > drawGrid =
		[]( float resolution, sf::Color color, sf::VertexArray& vArray ) {
		sf::Vertex start, end;
		start.color = color;
		end.color = color;

		float idx = resolution;
		while( idx < 10000.f ) {
			start.position = sf::Vector2f( idx, 10000.f );
			end.position = sf::Vector2f( idx, -10000.f );
			vArray.append( start );
			vArray.append( end );

			start.position = sf::Vector2f( -idx, 10000.f );
			end.position = sf::Vector2f( -idx, -10000.f );
			vArray.append( start );
			vArray.append( end );

			start.position = sf::Vector2f( 10000.f, idx );
			end.position = sf::Vector2f( -10000.f, idx );
			vArray.append( start );
			vArray.append( end );

			start.position = sf::Vector2f( 10000.f, -idx );
			end.position = sf::Vector2f( -10000.f, -idx );
			vArray.append( start );
			vArray.append( end );

			idx += resolution;
		}
	};

	drawGrid( 16.f, sf::Color( 25u, 25u, 25u, 255u ), m_grid );
	drawGrid( 64.f, sf::Color( 50u, 50u, 50u, 255u ), m_grid );
	drawGrid( 256.f, sf::Color( 100u, 100u, 100u, 255u ), m_grid );

	{
		sf::Vertex start, end;
		start.color = sf::Color( 200u, 200u, 200u, 255u );
		end.color = sf::Color( 200u, 200u, 200u, 255u );

		start.position = sf::Vector2f( 0.f, 10000.f );
		end.position = sf::Vector2f( 0.f, -10000.f );
		m_grid.append( start );
		m_grid.append( end );

		start.position = sf::Vector2f( 10000.f, 0.f );
		end.position = sf::Vector2f( -10000.f, 0.f );
		m_grid.append( start );
		m_grid.append( end );
	}
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
				Gfx::saveEmitter( *pattern, pattern->properties.name );
			}
			else {
				Gfx::Particle::Pattern* pattern = static_cast< Gfx::Particle::Pattern* >( tab.pattern );
				Gfx::savePattern( *pattern, pattern->properties.name );
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
	window->draw( m_grid );

	ImGui::PushID( "PatternEditor" );

	// Properties
	ImGui::Begin( "Properties", NULL, 
				  ImGuiWindowFlags_NoTitleBar | 
				  ImGuiWindowFlags_NoResize | 
				  ImGuiWindowFlags_NoMove | 
				  ImGuiWindowFlags_NoCollapse );

	ImGui::SetWindowSize( ImVec2( 300.0f, float( System::getSystemInfo().height - 35 ) ) );
	ImGui::SetWindowPos( ImVec2( 0.0f, 35.0f ) );

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
		tabName = m_baseEmitter.properties.name;
	else
		tabName = m_basePattern.properties.name;

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
			name = static_cast< Gfx::Emitter::Pattern* >( tab.pattern )->properties.name;
		else
			name = static_cast< Gfx::Particle::Pattern* >( tab.pattern )->properties.name;
		
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

	ImGui::SetWindowFontScale( 1.2f );
	ImGui::Text( pattern.properties.name.c_str() );
	ImGui::SetWindowFontScale( 1.0f );

	drawSpacer();

	if( ImGui::Button( "Rename" ) ) {
		ImGui::OpenPopup( "Rename" );
		delete[] m_renameBuffer;
		m_renameBuffer = new char[ 66565 ];
		strcpy_s( m_renameBuffer, 66565, pattern.properties.name.c_str() );
	}

	ImGui::SameLine();

	if( ImGui::Button( "Save" ) )
		Gfx::savePattern( pattern, pattern.properties.name );

	ImGui::SameLine();

	if( ImGui::Button( "Load" ) ) {
		pattern = Gfx::loadPattern( pattern.properties.name );
		m_changed = true;
	}

	if( ImGui::BeginPopup( "Rename", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse ) ) {
		if( !ImGui::IsWindowFocused() )
			ImGui::CloseCurrentPopup();

		if( ImGui::InputText( "Rename", m_renameBuffer, 65565, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
			pattern.properties.name = m_renameBuffer;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup(); // Rename
	}

	drawDoubleSpacer();

	/* Properties */
	ImGui::PushID( "Properties" );
	ImGui::Text( "Properties" );
	drawSpacer();

	// Lifetime
	ImGui::PushID( "Lifetime" );
	if( ImGui::TreeNodeEx( "Lifetime", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( pattern.properties.lifetime, "Lifetime" );
		ImGui::TreePop(); // Lifetime
	}
	ImGui::PopID(); // Lifetime

	// Number
	ImGui::PushID( "Number" );
	if( ImGui::TreeNodeEx( "Number", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( pattern.properties.number, "Number" );
		ImGui::TreePop(); // Number
	}
	ImGui::PopID(); // Number
	ImGui::PopID(); // Properties

	drawDoubleSpacer();

	/* Shape */
	ImGui::PushID( "Shape" );
	ImGui::Text( "Shape" );
	drawSpacer();

	// Type
	ImGui::PushID( "Type" );
	if( ImGui::Button( "Circle" ) ) {
		pattern.shape.type = Gfx::Particle::Pattern::Shape::Type::Circle;
		m_changed = true;
	}
	ImGui::SameLine();
	if( ImGui::Button( "Rectangle" ) ) {
		pattern.shape.type = Gfx::Particle::Pattern::Shape::Type::Rect;
		m_changed = true;
	}
	ImGui::SameLine();
	if( ImGui::Button( "Texture" ) ) {
		pattern.shape.type = Gfx::Particle::Pattern::Shape::Type::Texture;
		m_changed = true;
	}
	ImGui::PopID(); // Type

	if( pattern.shape.type == Gfx::Particle::Pattern::Shape::Type::Texture ) {
		// Texture
		ImGui::PushID( "Texture" );

		if( ImGui::InputText( "Texture", pattern.shape.texture.data(), 1024u, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
			m_changed = true;
		}
		if( ImGui::InputText( "Shader", pattern.shape.shader.data(), 1024u, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
			m_changed = true;
		}

		ImGui::PopID();// Texture;
	}

	// Size
	ImGui::PushID( "Size" );
	if( ImGui::TreeNodeEx( "Size", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( pattern.shape.size.size, "Size" );

		m_changed |= drawFade( pattern.shape.size.fade.start,
				  pattern.shape.size.fade.end,
				  pattern.shape.size.fade.x,
				  pattern.shape.size.fade.y,
				  "Size" );

		ImGui::TreePop(); // Size
	}
	ImGui::PopID(); // Size

	// Color
	ImGui::PushID( "Color" );
	if( ImGui::TreeNodeEx( "Color", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( pattern.shape.color.color, "Color" );

		m_changed |= drawFade( pattern.shape.color.fade.target,
				  pattern.shape.color.fade.r,
				  pattern.shape.color.fade.g,
				  pattern.shape.color.fade.b,
				  pattern.shape.color.fade.a,
				  "Color" );

		ImGui::TreePop(); // Color
	}
	ImGui::PopID(); // Color;

	if( pattern.shape.type != Gfx::Particle::Pattern::Shape::Type::Texture ) {
		// Outline
		ImGui::PushID( "Outline" );
		if( ImGui::TreeNodeEx( "Outline", ImGuiTreeNodeFlags_Framed ) ) {
			// Color
			ImGui::PushID( "Color" );
			ImGui::Text( "Color" );
			m_changed |= drawValueSet( pattern.shape.outline.color.color, "Color" );

			m_changed |= drawFade( pattern.shape.outline.color.fade.target,
					  pattern.shape.outline.color.fade.r,
					  pattern.shape.outline.color.fade.g,
					  pattern.shape.outline.color.fade.b,
					  pattern.shape.outline.color.fade.a,
					  "Color" );

			ImGui::PopID(); // Color

			ImGui::Spacing();

			// Thickness
			ImGui::PushID( "Thickness" );
			ImGui::Text( "Thickness" );
			m_changed |= drawValueSet( pattern.shape.outline.thickness.thickness, "Thickness" );

			m_changed |= drawFade( pattern.shape.outline.thickness.fade.start,
					  pattern.shape.outline.thickness.fade.end,
					  pattern.shape.outline.thickness.fade.active,
					  "Thickness" );

			ImGui::PopID(); // Thickness

			ImGui::TreePop(); // Outline
		}
		ImGui::PopID(); // Outline
	}

	// Origin
	ImGui::PushID( "Origin" );
	if( ImGui::TreeNodeEx( "Origin", ImGuiTreeNodeFlags_Framed ) ) {
		ImGui::PushID( "Position" );
		ImGui::Text( "Position" );
		ImGui::Indent();
		renderPositionSet( pattern.shape.origin.position );
		ImGui::Unindent();
		ImGui::PopID(); // Position

		ImGui::Spacing();

		ImGui::PushID( "Velocity" );
		ImGui::Text( "Velocity" );
		ImGui::Indent();
		renderVelocitySet( pattern.shape.origin.velocity );
		ImGui::Unindent();
		ImGui::PopID(); // Velocity

		ImGui::TreePop(); //Origin
	}
	ImGui::PopID(); // Origin
	ImGui::PopID(); // Shape

	drawDoubleSpacer();

	/* Physics */
	ImGui::PushID( "Physics" );
	ImGui::Text( "Physics" );
	drawSpacer();

	// Position
	ImGui::PushID( "Position" );
	if( ImGui::TreeNodeEx( "Position", ImGuiTreeNodeFlags_Framed ) ) {
		renderPositionSet( pattern.physics.position );

		ImGui::TreePop(); // Position
	}
	ImGui::PopID(); // Position

	// Velocity
	ImGui::PushID( "Velocity" );
	if( ImGui::TreeNodeEx( "Velocity", ImGuiTreeNodeFlags_Framed ) ) {
		renderVelocitySet( pattern.physics.velocity );

		ImGui::TreePop(); // Velocity
	}
	ImGui::PopID(); // Velocity

	// Acceleration
	ImGui::PushID( "Acceleration" );
	if( ImGui::TreeNodeEx( "Acceleration", ImGuiTreeNodeFlags_Framed ) ) {
		renderVelocitySet( pattern.physics.acceleration );

		ImGui::TreePop(); // Acceleration
	}
	ImGui::PopID(); // Acceleration

	// Rotation
	ImGui::PushID( "Rotation" );
	if( ImGui::TreeNodeEx( "Rotation", ImGuiTreeNodeFlags_Framed ) ) {
		ImGui::PushID( "Rotation" );
		ImGui::Text( "Rotation" );
		m_changed |= drawValueSet( pattern.physics.rotation.rotation, "Rotation", true );

		m_changed |= drawFade( pattern.physics.rotation.fade.target,
				  pattern.physics.rotation.fade.active,
				  "Rotation" );
		ImGui::PopID(); // Rotation

		ImGui::Spacing();

		// Spin
		ImGui::PushID( "Spin" );
		ImGui::Text( "Spin" );
		m_changed |= drawValueSet( pattern.physics.rotation.spin.spin, "Spin", true );
			
		m_changed |= drawFade( pattern.physics.rotation.spin.fade.start,
					pattern.physics.rotation.spin.fade.end,
					pattern.physics.rotation.spin.fade.active,
					"Spin" );
		ImGui::PopID(); // Spin

		ImGui::TreePop(); // Rotation
	}
	ImGui::PopID(); // Rotation
	ImGui::PopID(); // Physics

	drawDoubleSpacer();

	/* Inheritance */
	ImGui::PushID( "Inheritance" );
	ImGui::Text( "Inheritance" );
	drawSpacer();

	m_changed |= ImGui::Checkbox( "Position", &pattern.inheritance.position );
	m_changed |= ImGui::Checkbox( "Velocity", &pattern.inheritance.velocity );
	m_changed |= ImGui::Checkbox( "Size", &pattern.inheritance.size );
	m_changed |= ImGui::Checkbox( "Alpha", &pattern.inheritance.alpha );

	ImGui::PopID(); // Inheritance

	drawDoubleSpacer();

	/* Emitters */
	ImGui::PushID( "Emitters" );
	ImGui::Text( "Emitters" );
	drawSpacer();

	for( size_t i = 0; i < pattern.emitters.size(); ++i ) {
		Gfx::Emitter::Pattern& emitter = pattern.emitters.at( i );

		ImGui::Text( emitter.properties.name.c_str() );

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
		pattern.emitters.push_back( Gfx::Emitter::Pattern() );
		m_changed = true;
	}

	ImGui::PopID();

	drawDoubleSpacer();

	ImGui::PopID(); // m_patternID
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderEmitter( Gfx::Emitter::Pattern& emitter ) {
	ImGui::PushID( ++m_patternID );

	ImGui::SetWindowFontScale( 1.2f );
	ImGui::Text( emitter.properties.name.c_str() );
	ImGui::SetWindowFontScale( 1.0f );

	drawSpacer();

	if( ImGui::Button( "Rename" ) ) {
		ImGui::OpenPopup( "Rename" );
		delete[] m_renameBuffer;
		m_renameBuffer = new char[ 66565 ];
		strcpy_s( m_renameBuffer, 66565, emitter.properties.name.c_str() );
	}

	ImGui::SameLine();

	if( ImGui::Button( "Save" ) )
		Gfx::saveEmitter( emitter, emitter.properties.name );

	ImGui::SameLine();

	if( ImGui::Button( "Load" ) ) {
		emitter = Gfx::loadEmitter( emitter.properties.name );
		m_changed = true;
	}

	if( ImGui::BeginPopup( "Rename", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse ) ) {
		if( !ImGui::IsWindowFocused() )
			ImGui::CloseCurrentPopup();

		if( ImGui::InputText( "Rename", m_renameBuffer, 65565, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
			emitter.properties.name = m_renameBuffer;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	drawDoubleSpacer();

	ImGui::Text( "Properties" );

	drawSpacer();

	if( ImGui::TreeNodeEx( "Position", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= drawValueSet( emitter.properties.position, "Position" );
		ImGui::TreePop();
	}

	if( &emitter != &m_baseEmitter ) {
		if( ImGui::TreeNodeEx( "Activation", ImGuiTreeNodeFlags_Framed ) ) {
			string buttonName;
			if( emitter.properties.activation.type == Gfx::Emitter::Pattern::Properties::Activation::Type::OnSpawn )
				buttonName = "On Spawn";
			else if( emitter.properties.activation.type == Gfx::Emitter::Pattern::Properties::Activation::Type::Alpha )
				buttonName = "Alpha";
			else if( emitter.properties.activation.type == Gfx::Emitter::Pattern::Properties::Activation::Type::OnDestruction )
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
					emitter.properties.activation.type = Gfx::Emitter::Pattern::Properties::Activation::Type::OnSpawn;
					m_changed = true;
					ImGui::CloseCurrentPopup();
				}
				else if( ImGui::Button( "Alpha" ) ) {
					emitter.properties.activation.type = Gfx::Emitter::Pattern::Properties::Activation::Type::Alpha;
					m_changed = true;
					ImGui::CloseCurrentPopup();
				}
				else if( ImGui::Button( "On Destruction" ) ) {
					emitter.properties.activation.type = Gfx::Emitter::Pattern::Properties::Activation::Type::OnDestruction;
					m_changed = true;
					ImGui::CloseCurrentPopup();
				}

				ImGui::PopID();
				ImGui::EndPopup();
			}

			if( emitter.properties.activation.type == Gfx::Emitter::Pattern::Properties::Activation::Type::Alpha ) {
				ImGui::Text( "Start" );
				m_changed |= drawValueSet( emitter.properties.activation.start, "Start" );
				ImGui::Text( "End" );
				m_changed |= drawValueSet( emitter.properties.activation.end, "End" );
			}
			ImGui::TreePop();
		}
	}

	if( ImGui::TreeNodeEx( "Spawn Rate", ImGuiTreeNodeFlags_Framed ) ) {
		m_changed |= ImGui::Checkbox( "Fade##SpawnRate", &emitter.properties.rate.fade );

		if( emitter.properties.rate.fade ) {
			ImGui::Text( "Start" );
			m_changed |= drawValueSet( emitter.properties.rate.start, "SpawnRateStart" );
			ImGui::Text( "End" );
			m_changed |= drawValueSet( emitter.properties.rate.end, "SpawnRateEnd" );
		}
		else {
			m_changed |= drawValueSet( emitter.properties.rate.start, "SpawnRate" );
		}
		ImGui::TreePop();
	}

	drawDoubleSpacer();

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

	drawDoubleSpacer();

	ImGui::Text( "Patterns" );

	drawSpacer();

	for( size_t i = 0; i < emitter.patterns.size(); ++i ) {
		Gfx::Particle::Pattern& pattern = emitter.patterns.at( i );

		ImGui::Text( pattern.properties.name.c_str() );

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
		emitter.patterns.push_back( Gfx::Particle::Pattern() );
		m_changed = true;
	}

	drawDoubleSpacer();

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

			ImGui::Text( pattern.properties.name.c_str() );

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
			set.patterns.push_back( Gfx::Particle::Pattern() );
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

void BulletPatternEditor::renderVelocitySet( Gfx::Particle::VelocitySet& set ) {
	// Type
	ImGui::PushID( "Type" );
	if( ImGui::Button( "Direction" ) ) {
		set.type = Gfx::Particle::VelocitySet::Type::Direction;
		m_changed = true;
	}
	ImGui::SameLine();
	if( ImGui::Button( "Value" ) ) {
		set.type = Gfx::Particle::VelocitySet::Type::Value;
		m_changed = true;
	}
	ImGui::SameLine();
	if( ImGui::Button( "Point" ) ) {
		set.type = Gfx::Particle::VelocitySet::Type::Point;
		m_changed = true;
	}
	ImGui::SameLine();
	if( ImGui::Button( "Player" ) ) {
		set.type = Gfx::Particle::VelocitySet::Type::Player;
		m_changed = true;
	}
	ImGui::PopID(); // Type

	if( set.type == Gfx::Particle::VelocitySet::Type::Direction ) {
		// Direction
		ImGui::PushID( "Direction" );
		ImGui::Text( "Direction" );
		m_changed |= drawValueSet( set.direction.direction, "Direction", true );
		m_changed |= drawFade( set.direction.fade.target,
								set.direction.fade.direction,
								"Direction" );

		ImGui::Spacing();

		ImGui::PushID( "Speed" );
		ImGui::Text( "Speed" );
		m_changed |= drawValueSet( set.direction.power, "Speed", true );
		m_changed |= drawFade( set.direction.fade.start,
								set.direction.fade.end,
								set.direction.fade.power,
								"Speed" );
		ImGui::PopID(); // Speed

		ImGui::Spacing();

		ImGui::PopID(); // Direction
	}
	else if( set.type == Gfx::Particle::VelocitySet::Type::Value ) {
		// Value
		ImGui::PushID( "Value" );
		ImGui::Text( "Value" );
		m_changed |= drawValueSet( set.value.value, "Value", true );
		m_changed |= drawFade( set.value.fade.target,
								set.value.fade.x,
								set.value.fade.y,
								"Value" );

		ImGui::PopID(); // Value
	}
	else if( set.type == Gfx::Particle::VelocitySet::Type::Point ) {
		// Point
		ImGui::PushID( "Point" );
		ImGui::Text( "Point" );
		m_changed |= drawValueSet( set.point.point, "Point", true );
		m_changed |= drawFade( set.point.fade.target,
								set.point.fade.point,
								"Point" );

		ImGui::Spacing();

		// Speed
		ImGui::PushID( "Speed" );
		ImGui::Text( "Speed" );
		m_changed |= drawValueSet( set.point.power, "Speed", true );
		m_changed |= drawFade( set.point.fade.start,
								set.point.fade.end,
								set.point.fade.power,
								"Speed" );
		ImGui::PopID(); // Speed

		// Scale
		ImGui::PushID( "Scale" );
		if( ImGui::TreeNodeEx( "Scale", ImGuiTreeNodeFlags_Framed ) ) {
			m_changed |= ImGui::Checkbox( "Scale", &set.point.scale.active );
			ImGui::SameLine();
			m_changed |= ImGui::Checkbox( "Inverted", &set.point.scale.inverse );
			ImGui::Text( "Factor" );
			m_changed |= drawValueSet( set.point.scale.factor, "Factor" );


			ImGui::TreePop(); // Scale
		}
		ImGui::PopID(); // Scale
		ImGui::PopID(); // Point
	}
	else if( set.type == Gfx::Particle::VelocitySet::Type::Player ) {
		// Player
		ImGui::PushID( "Player" );

		// Speed
		m_changed |= ImGui::Checkbox( "Track", &set.point.track );
		ImGui::Text( "Speed" );
		m_changed |= drawValueSet( set.point.power, "Speed", true );
		m_changed |= drawFade( set.point.fade.start,
								set.point.fade.end,
								set.point.fade.power,
								"Speed" );

		// Scale
		if( ImGui::TreeNodeEx( "Scale", ImGuiTreeNodeFlags_Framed ) ) {
			m_changed |= ImGui::Checkbox( "Scale", &set.point.scale.active );
			ImGui::SameLine();
			m_changed |= ImGui::Checkbox( "Inverted", &set.point.scale.inverse );
			ImGui::Text( "Factor" );
			m_changed |= drawValueSet( set.point.scale.factor, "Factor" );

			ImGui::TreePop(); // Scale
		}

		ImGui::PopID(); // Player
	}
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPositionSet( Gfx::Particle::PositionSet& set ) {
	// Type
	ImGui::PushID( "Type" );
	if( ImGui::Button( "Point" ) ) {
		set.type = Gfx::Particle::PositionSet::Type::Point;
		m_changed = true;
	}
	ImGui::SameLine();
	if( ImGui::Button( "Circle" ) ) {
		set.type = Gfx::Particle::PositionSet::Type::Circle;
		m_changed = true;
	}
	ImGui::SameLine();
	if( ImGui::Button( "Rectangle" ) ) {
		set.type = Gfx::Particle::PositionSet::Type::Rectangle;
		m_changed = true;
	}
	ImGui::PopID(); // Type

	ImGui::Text( "Point" );
	m_changed |= drawValueSet( set.position, "Position", true );

	ImGui::Spacing();

	if( set.type == Gfx::Particle::PositionSet::Type::Circle ) {
		// Circle
		ImGui::PushID( "Circle" );
		ImGui::Text( "Circle" );
		m_changed |= ImGui::Checkbox( "Fill", &set.circle.fill );
		if( !set.circle.fill ) {
			ImGui::SameLine();
			m_changed |= ImGui::Checkbox( "Uniform", &set.circle.uniform );
		}
		ImGui::Text( "Radius" );
		m_changed |= drawValueSet( set.circle.radius, "Radius" );
		ImGui::PopID(); // Circle
	}
	else if( set.type == Gfx::Particle::PositionSet::Type::Rectangle ) {
		// Rectangle
		ImGui::PushID( "Rectangle" );
		ImGui::Text( "Rectangle" );
		m_changed |= ImGui::Checkbox( "Fill", &set.rectangle.fill );
		if( !set.rectangle.fill ) {
			ImGui::SameLine();
			m_changed |= ImGui::Checkbox( "Uniform", &set.rectangle.uniform );
		}

		ImGui::Text( "Size" );
		m_changed |= drawValueSet( set.rectangle.size, "Size" );
		ImGui::PopID(); // Rectangle
	}

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
	
	if( ImGui::Button( ( pattern.properties.name + "###" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
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



	if( ImGui::Button( ( emitter.properties.name + "###" + std::to_string( m_patternTreeID++ ) ).c_str() ) ) {
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

	if( emitter.patterns.size() )
		ImGui::Text( "Patterns" );

	for( Gfx::Particle::Pattern& pattern : emitter.patterns )
			renderPatternTreeNode( pattern, stack );

	for( Gfx::Particle::PatternSet& set : emitter.sets ) {
		if( set.patterns.empty() )
			continue;

		if( set.random )
			ImGui::Text( "Random Set" );
		else
			ImGui::Text( "Sequence" );

		for( Gfx::Particle::Pattern& pattern : set.patterns ) {
			renderPatternTreeNode( pattern, stack );
		}
	}

	ImGui::Unindent();
}

//--------------------------------------------------------------------------------

}

//================================================================================