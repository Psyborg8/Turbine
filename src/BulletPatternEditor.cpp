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

void drawValueSet( Gfx::Particle::ValueSet< int >& set, const char* id ) {
	ImGui::PushID( id );

	ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		ImGui::InputInt( "min", &set.min );
		ImGui::InputInt( "max", &set.max );
	}
	else {
		ImGui::InputInt( "", &set.min );
	}

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void drawValueSet( Gfx::Particle::ValueSet< float >& set, const char* id ) {
	ImGui::PushID( id );

	ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		ImGui::InputFloat( "min", &set.min );
		ImGui::InputFloat( "max", &set.max );
	}
	else {
		ImGui::InputFloat( "", &set.min );
	}

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void drawValueSet( Gfx::Particle::ValueSet< Math::Vec2 >& set, const char* id ) {
	ImGui::PushID( id );

	ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		ImGui::InputFloat2( "min", &set.min.x );
		ImGui::InputFloat2( "max", &set.max.x );
	}
	else {
		ImGui::InputFloat2( "", &set.min.x );
	}

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void drawValueSet( Gfx::Particle::ValueSet< Math::Color >& set, const char* id ) {
	ImGui::PushID( id );

	ImGui::Checkbox( "Random", &set.random );

	if( set.random ) {
		ImGui::ColorEdit4( "min", &set.min.r );
		ImGui::ColorEdit4( "max", &set.max.r );
	}
	else
	{
		ImGui::ColorEdit4( "", &set.min.r );
	}

	ImGui::PopID();
}

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

void BulletPatternEditor::onSpawnChildren() {
	shared_ptr< Debug::MessageWindow > window = makeObject< Debug::MessageWindow >( this );
	window->setVisibility( false );
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::onUpdate( sf::Time deltaTime ) {
	//
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
			line[ 0 ].color = sf::Color( 175, 175, 175, 255 );
			line[ 1 ].color = sf::Color( 175, 175, 175, 255 );

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
			line[ 0 ].color = sf::Color( 100, 100, 100, 255 );
			line[ 1 ].color = sf::Color( 100, 100, 100, 255 );

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
			line[ 0 ].color = sf::Color( 25, 25, 25, 255 );
			line[ 1 ].color = sf::Color( 25, 25, 25, 255 );

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

	// Control
	ImGui::Begin( "Control", NULL,
				  ImGuiWindowFlags_NoTitleBar |
				  ImGuiWindowFlags_NoResize |
				  ImGuiWindowFlags_NoMove |
				  ImGuiWindowFlags_NoCollapse |
				  ImGuiWindowFlags_NoScrollbar |
				  ImGuiWindowFlags_AlwaysAutoResize );

	ImGui::Text( "Control" );

	if( ImGui::Button( "Spawn" ) ) {
		Gfx::Particle::spawnParticle( this, m_basePattern );
	}
	ImGui::SameLine();
	if( ImGui::Button( "Stop" ) ) {
		//
	}
	ImGui::SameLine();
	if( ImGui::Button( "Kill" ) ) {
		//
	}

	ImGui::SetWindowPos( ImVec2( System::getSystemInfo().width - ImGui::GetWindowSize().x, System::getSystemInfo().height - ImGui::GetWindowSize().y ) );
	ImGui::End();

	// Properties
	ImGui::Begin( "Properties", NULL, 
				  ImGuiWindowFlags_NoTitleBar | 
				  ImGuiWindowFlags_NoResize | 
				  ImGuiWindowFlags_NoMove | 
				  ImGuiWindowFlags_NoCollapse );

	ImGui::SetWindowSize( ImVec2( 300.0f, float( System::getSystemInfo().height ) ) );
	ImGui::SetWindowPos( ImVec2( 0.0f, 0.0f ) );

	ImGui::SetWindowFontScale( 1.2f );
	ImGui::Text( "PROPERTIES" );
	ImGui::SetWindowFontScale( 1.0f );

	drawDoubleSpacer();

	renderPattern( m_basePattern );

	ImGui::End();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPattern( Gfx::Particle::Pattern& pattern ) {

	if( ImGui::TreeNodeEx( "Lifetime", ImGuiTreeNodeFlags_Framed ) ) {
		drawValueSet( pattern.initial.lifetime, "Lifetime" );

		ImGui::TreePop();
	}

	drawDoubleSpacer();

	if( ImGui::TreeNodeEx( "Number", ImGuiTreeNodeFlags_Framed ) ) {
		drawValueSet( pattern.initial.number, "Number" );

		ImGui::TreePop();
	}

	drawDoubleSpacer();

	if( ImGui::TreeNodeEx( "Direction", ImGuiTreeNodeFlags_Framed ) ) {
		drawValueSet( pattern.initial.direction, "Direction" );

		ImGui::TreePop();
	}

	drawDoubleSpacer();

	if( ImGui::TreeNodeEx( "Velocity", ImGuiTreeNodeFlags_Framed ) ) {
		drawValueSet( pattern.initial.velocity, "Velocity" );

		drawSpacer();

		if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {

			ImGui::Checkbox( "X", &pattern.fade.velocity.x );
			ImGui::SameLine();
			ImGui::Checkbox( "Y", &pattern.fade.velocity.y );

			ImGui::Text( "Start" );
			drawValueSet( pattern.fade.velocity.start, "FadeVelocityStart" );
			ImGui::Text( "End" );
			drawValueSet( pattern.fade.velocity.end, "FadeVelocityEnd" );

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	drawDoubleSpacer();

	if( ImGui::TreeNodeEx( "Acceleration", ImGuiTreeNodeFlags_Framed ) ) {
		drawValueSet( pattern.initial.acceleration, "Acceleration" );

		drawSpacer();

		if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {

			ImGui::Checkbox( "X", &pattern.fade.acceleration.x );
			ImGui::SameLine();
			ImGui::Checkbox( "Y", &pattern.fade.acceleration.y );

			ImGui::Text( "Start" );
			drawValueSet( pattern.fade.acceleration.start, "FadeAccelerationStart" );
			ImGui::Text( "End" );
			drawValueSet( pattern.fade.acceleration.end, "FadeAccelerationEnd" );

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	drawDoubleSpacer();

	if( ImGui::TreeNodeEx( "Size", ImGuiTreeNodeFlags_Framed ) ) {
		drawValueSet( pattern.initial.size, "Size" );

		drawSpacer();

		if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {

			ImGui::Checkbox( "Fade", &pattern.fade.size.active );

			ImGui::Text( "Start" );
			drawValueSet( pattern.fade.size.start, "FadeSizeStart" );
			ImGui::Text( "End" );
			drawValueSet( pattern.fade.size.end, "FadeSizeEnd" );

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	drawDoubleSpacer();

	if( ImGui::TreeNodeEx( "Color", ImGuiTreeNodeFlags_Framed ) ) {
		drawValueSet( pattern.initial.color, "Color" );

		drawSpacer();

		if( ImGui::TreeNodeEx( "Fade", ImGuiTreeNodeFlags_Framed ) ) {
			ImGui::Checkbox( "R", &pattern.fade.color.r );
			ImGui::SameLine();
			ImGui::Checkbox( "G", &pattern.fade.color.g );
			ImGui::SameLine();
			ImGui::Checkbox( "B", &pattern.fade.color.b );
			ImGui::SameLine();
			ImGui::Checkbox( "A", &pattern.fade.color.a );

			ImGui::Text( "Target" );
			drawValueSet( pattern.fade.color.target, "ColorTarget" );
			
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	drawDoubleSpacer();
}

//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------

}

//================================================================================