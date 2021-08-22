//================================================================================

#include "BulletPatternEditor.h"

//--------------------------------------------------------------------------------

#include "System.h"
#include "Debug.h"

//================================================================================

//--------------------------------------------------------------------------------

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
		Gfx::Particle::spawnParticle( this, convertPattern( m_basePattern ) );
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

	ImGui::Text( "PROPERTIES" );
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();

	renderPattern( m_basePattern );

	ImGui::End();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPattern( Pattern& pattern ) {
	renderLifetime( pattern );
	renderPosition( pattern );
	renderVelocity( pattern );
	renderColor( pattern );
	renderSize( pattern );
	renderGravity( pattern );
	renderNumber( pattern );
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderPosition( Pattern& pattern ) {
	ImGui::PushID( "Position" );
	// Label
	ImGui::Text( "Position" );
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Type
	if( pattern.position.type == ValueType::Static )
		if( ImGui::Button( "Type: Static" ) )
			ImGui::OpenPopup( "PositionType" );

	if( pattern.position.type == ValueType::Random )
		if( ImGui::Button( "Type: Random" ) )
			ImGui::OpenPopup( "PositionType" );

	// Type Popup
	if( ImGui::BeginPopup( "PositionType",
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoCollapse ) ) {
		if( ImGui::MenuItem( "Static" ) )
			pattern.position.type = ValueType::Static;
		if( ImGui::MenuItem( "Random" ) )
			pattern.position.type = ValueType::Random;

		ImGui::EndPopup();
	}

	ImGui::Spacing();

	// Value
	if( pattern.position.type == ValueType::Static ) {
		ImGui::InputFloat2( "Value", pattern.position.min.data() );
		pattern.position.max = pattern.position.min;
	}
	else if( pattern.position.type == ValueType::Random ) {
		ImGui::InputFloat2( "Min", pattern.position.min.data() );
		ImGui::InputFloat2( "Max", pattern.position.max.data() );
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderVelocity( Pattern& pattern ) {
	ImGui::PushID( "Velocity" );
	// Label
	ImGui::Text( "Velocity" );
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Type
	if( pattern.velocity.type == ValueType::Static ) {
		if( ImGui::Button( "Type: Static" ) )
			ImGui::OpenPopup( "Type" );
	}

	else if( pattern.velocity.type == ValueType::Random ) {
		if( ImGui::Button( "Type: Random" ) )
			ImGui::OpenPopup( "Type" );
	}

	else if( pattern.velocity.type == ValueType::Fade ) {
		if( ImGui::Button( "Type: Fade" ) )
			ImGui::OpenPopup( "Type" );
	}

	// Type Popup
	if( ImGui::BeginPopup( "Type",
							ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoCollapse ) ) {
		if( ImGui::MenuItem( "Static" ) )
			pattern.velocity.type = ValueType::Static;
		if( ImGui::MenuItem( "Random" ) )
			pattern.velocity.type = ValueType::Random;
		if( ImGui::MenuItem( "Fade" ) )
			pattern.velocity.type = ValueType::Fade;

		ImGui::EndPopup();
	}

	ImGui::Spacing();

	// Value
	if( pattern.velocity.type == ValueType::Static ) {
		ImGui::InputFloat2( "Value", pattern.velocity.startMin.data() );
		pattern.velocity.startMax = pattern.velocity.startMin;
		pattern.velocity.endMin = pattern.velocity.startMin;
		pattern.velocity.endMax = pattern.velocity.startMin;
	}
	else if( pattern.velocity.type == ValueType::Random ) {
		ImGui::InputFloat2( "Min", pattern.velocity.startMin.data() );
		ImGui::InputFloat2( "Max", pattern.velocity.startMax.data() );
		pattern.velocity.endMin = pattern.velocity.startMin;
		pattern.velocity.endMax = pattern.velocity.startMax;
	}
	else if( pattern.velocity.type == ValueType::Fade ) {
		// Start
		ImGui::PushID( "Start" );
		ImGui::Text( "Start" );
		if( pattern.velocity.startType == ValueType::Static ) {
			if( ImGui::Button( "Type: Static" ) )
				ImGui::OpenPopup( "Type" );
		}
		else if( pattern.velocity.startType == ValueType::Random ) {
			if( ImGui::Button( "Type: Random" ) )
				ImGui::OpenPopup( "Type" );
		}

		// Type Popup
		if( ImGui::BeginPopup( "Type",
								ImGuiWindowFlags_NoTitleBar |
								ImGuiWindowFlags_NoResize |
								ImGuiWindowFlags_NoMove |
								ImGuiWindowFlags_NoCollapse ) ) {
			if( ImGui::MenuItem( "Static" ) )
				pattern.velocity.startType = ValueType::Static;
			if( ImGui::MenuItem( "Random" ) )
				pattern.velocity.startType = ValueType::Random;

			ImGui::EndPopup();
		}

		// Value
		if( pattern.velocity.startType == ValueType::Static ) {
			ImGui::InputFloat2( "Value", pattern.velocity.startMin.data() );
			pattern.velocity.startMax = pattern.velocity.startMin;
		}
		else if( pattern.velocity.startType == ValueType::Random ) {
			ImGui::InputFloat2( "Min", pattern.velocity.startMin.data() );
			ImGui::InputFloat2( "Max", pattern.velocity.startMax.data() );
		}

		ImGui::PopID();

		ImGui::Spacing();
		ImGui::Spacing();

		// End
		ImGui::PushID( "End" );
		ImGui::Text( "End" );

		// Fade Type
		ImGui::Checkbox( "Fade X", &pattern.velocity.fadeX );
		ImGui::SameLine();
		ImGui::Checkbox( "Fade Y", &pattern.velocity.fadeY );

		if( pattern.velocity.endType == ValueType::Static )
			if( ImGui::Button( "Type: Static" ) )
				ImGui::OpenPopup( "Type" );
		if( pattern.velocity.endType == ValueType::Random )
			if( ImGui::Button( "Type: Random" ) )
				ImGui::OpenPopup( "Type" );

		// Type Popup
		if( ImGui::BeginPopup( "Type",
								ImGuiWindowFlags_NoTitleBar |
								ImGuiWindowFlags_NoResize |
								ImGuiWindowFlags_NoMove |
								ImGuiWindowFlags_NoCollapse ) ) {
			if( ImGui::MenuItem( "Static" ) )
				pattern.velocity.endType = ValueType::Static;
			if( ImGui::MenuItem( "Random" ) )
				pattern.velocity.endType = ValueType::Random;

			ImGui::EndPopup();
		}

		// Value
		if( pattern.velocity.endType == ValueType::Static ) {
			if( pattern.velocity.fadeX && pattern.velocity.fadeY ) {
				ImGui::InputFloat2( "Value", pattern.velocity.endMin.data() );
				pattern.velocity.endMax = pattern.velocity.endMin;
			}
			else if( pattern.velocity.fadeX ) {
				ImGui::InputFloat( "Value", &pattern.velocity.endMin[ 0 ] );
				pattern.velocity.endMax = pattern.velocity.endMin;
			}
			else if( pattern.velocity.fadeY ) {
				ImGui::InputFloat( "Value", &pattern.velocity.endMin[ 1 ] );
				pattern.velocity.endMax = pattern.velocity.endMin;
			}
		}
		else if( pattern.velocity.endType == ValueType::Random ) {
				ImGui::InputFloat2( "Min", pattern.velocity.endMin.data() );
				ImGui::InputFloat2( "Max", pattern.velocity.endMax.data() );
		}

		ImGui::PopID();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderGravity( Pattern& pattern ) {
	//
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderColor( Pattern& pattern ) {
	ImGui::PushID( "Color" );
	// Label
	ImGui::Text( "Color" );
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Type
	if( pattern.color.type == ValueType::Static ) {
		if( ImGui::Button( "Type: Static" ) )
			ImGui::OpenPopup( "Type" );
	}

	else if( pattern.color.type == ValueType::Random ) {
		if( ImGui::Button( "Type: Random" ) )
			ImGui::OpenPopup( "Type" );
	}

	else if( pattern.color.type == ValueType::Fade ) {
		if( ImGui::Button( "Type: Fade" ) )
			ImGui::OpenPopup( "Type" );
	}

	// Type Popup
	if( ImGui::BeginPopup( "Type",
						   ImGuiWindowFlags_NoTitleBar |
						   ImGuiWindowFlags_NoResize |
						   ImGuiWindowFlags_NoMove |
						   ImGuiWindowFlags_NoCollapse ) ) {
		if( ImGui::MenuItem( "Static" ) )
			pattern.color.type = ValueType::Static;
		if( ImGui::MenuItem( "Random" ) )
			pattern.color.type = ValueType::Random;
		if( ImGui::MenuItem( "Fade" ) )
			pattern.color.type = ValueType::Fade;

		ImGui::EndPopup();
	}

	ImGui::Spacing();

	// Value
	if( pattern.color.type == ValueType::Static ) {
		ImGui::ColorEdit4( "Value", &pattern.color.startMin.x );
		pattern.color.startMax = pattern.color.startMin;
		pattern.color.endMin = pattern.color.startMin;
		pattern.color.endMax = pattern.color.startMin;
	}
	else if( pattern.color.type == ValueType::Random ) {
		if( pattern.color.randomType == Random::RandomColorType::ShuffleRGB ) {
			if( ImGui::Button( "Random Type: Shuffle RGB" ) )
				ImGui::OpenPopup( "RandomType" );
		}
		else if( pattern.color.randomType == Random::RandomColorType::MixRGB ) {
			if( ImGui::Button( "Random Type: Mix RGB" ) )
				ImGui::OpenPopup( "RandomType" );
		}
		else if( pattern.color.randomType == Random::RandomColorType::ShuffleHSV ) {
			if( ImGui::Button( "Random Type: Shuffle HSV" ) )
				ImGui::OpenPopup( "RandomType" );
		}
		else if( pattern.color.randomType == Random::RandomColorType::MixHSV ) {
			if( ImGui::Button( "Random Type: Mix HSV" ) )
				ImGui::OpenPopup( "RandomType" );
		}
		else if( pattern.color.randomType == Random::RandomColorType::SortedHSV ) {
			if( ImGui::Button( "Random Type: Sorted HSV" ) )
				ImGui::OpenPopup( "RandomType" );
		}

		if( ImGui::BeginPopup( "RandomType", 
							   ImGuiWindowFlags_NoTitleBar |
							   ImGuiWindowFlags_NoResize |
							   ImGuiWindowFlags_NoMove |
							   ImGuiWindowFlags_NoCollapse ) ) {
			if( ImGui::MenuItem( "Shuffle RGB" ) )
				pattern.color.randomType = Random::RandomColorType::ShuffleRGB;
			if( ImGui::MenuItem( "Mix RGB" ) )
				pattern.color.randomType = Random::RandomColorType::MixRGB;
			if( ImGui::MenuItem( "Shuffle HSV" ) )
				pattern.color.randomType = Random::RandomColorType::ShuffleHSV;
			if( ImGui::MenuItem( "Mix HSV" ) )
				pattern.color.randomType = Random::RandomColorType::MixHSV;
			if( ImGui::MenuItem( "Sorted HSV" ) )
				pattern.color.randomType = Random::RandomColorType::SortedHSV;

			ImGui::EndPopup();
		}

		ImGui::ColorEdit4( "Min", &pattern.color.startMin.x );
		ImGui::ColorEdit4( "Max", &pattern.color.startMax.x );
		pattern.color.endMin = pattern.color.startMin;
		pattern.color.endMax = pattern.color.startMax;
	}
	else if( pattern.color.type == ValueType::Fade ) {
		// Start
		ImGui::PushID( "Start" );
		ImGui::Text( "Start" );
		if( pattern.color.startType == ValueType::Static ) {
			if( ImGui::Button( "Type: Static" ) )
				ImGui::OpenPopup( "Type" );
		}
		else if( pattern.color.startType == ValueType::Random ) {
			if( ImGui::Button( "Type: Random" ) )
				ImGui::OpenPopup( "Type" );
		}

		// Type Popup
		if( ImGui::BeginPopup( "Type",
							   ImGuiWindowFlags_NoTitleBar |
							   ImGuiWindowFlags_NoResize |
							   ImGuiWindowFlags_NoMove |
							   ImGuiWindowFlags_NoCollapse ) ) {
			if( ImGui::MenuItem( "Static" ) )
				pattern.color.startType = ValueType::Static;
			if( ImGui::MenuItem( "Random" ) )
				pattern.color.startType = ValueType::Random;

			ImGui::EndPopup();
		}

		// Value
		if( pattern.color.startType == ValueType::Static ) {
			ImGui::ColorEdit4( "Value", &pattern.color.startMin.x );
			pattern.color.startMax = pattern.color.startMin;
		}
		else if( pattern.color.startType == ValueType::Random ) {
			if( pattern.color.startRandomType == Random::RandomColorType::ShuffleRGB ) {
				if( ImGui::Button( "Random Type: Shuffle RGB" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.startRandomType == Random::RandomColorType::MixRGB ) {
				if( ImGui::Button( "Random Type: Mix RGB" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.startRandomType == Random::RandomColorType::ShuffleHSV ) {
				if( ImGui::Button( "Random Type: Shuffle HSV" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.startRandomType == Random::RandomColorType::MixHSV ) {
				if( ImGui::Button( "Random Type: Mix HSV" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.startRandomType == Random::RandomColorType::SortedHSV ) {
				if( ImGui::Button( "Random Type: Sorted HSV" ) )
					ImGui::OpenPopup( "RandomType" );
			}

			if( ImGui::BeginPopup( "RandomType",
								   ImGuiWindowFlags_NoTitleBar |
								   ImGuiWindowFlags_NoResize |
								   ImGuiWindowFlags_NoMove |
								   ImGuiWindowFlags_NoCollapse ) ) {
				if( ImGui::MenuItem( "Shuffle RGB" ) )
					pattern.color.startRandomType = Random::RandomColorType::ShuffleRGB;
				if( ImGui::MenuItem( "Mix RGB" ) )
					pattern.color.startRandomType = Random::RandomColorType::MixRGB;
				if( ImGui::MenuItem( "Shuffle HSV" ) )
					pattern.color.startRandomType = Random::RandomColorType::ShuffleHSV;
				if( ImGui::MenuItem( "Mix HSV" ) )
					pattern.color.startRandomType = Random::RandomColorType::MixHSV;
				if( ImGui::MenuItem( "Sorted HSV" ) )
					pattern.color.startRandomType = Random::RandomColorType::SortedHSV;

				ImGui::EndPopup();
			}

			ImGui::ColorEdit4( "Min", &pattern.color.startMin.x );
			ImGui::ColorEdit4( "Max", &pattern.color.startMax.x );
		}

		ImGui::PopID();

		ImGui::Spacing();
		ImGui::Spacing();

		// End
		ImGui::PushID( "End" );
		ImGui::Text( "End" );

		// Fade Type
		ImGui::Checkbox( "R", &pattern.color.fadeR );
		ImGui::SameLine();
		ImGui::Checkbox( "G", &pattern.color.fadeG );
		ImGui::SameLine();
		ImGui::Checkbox( "B", &pattern.color.fadeB );
		ImGui::SameLine();
		ImGui::Checkbox( "A", &pattern.color.fadeA );

		if( pattern.color.endType == ValueType::Static )
			if( ImGui::Button( "Type: Static" ) )
				ImGui::OpenPopup( "Type" );
		if( pattern.color.endType == ValueType::Random )
			if( ImGui::Button( "Type: Random" ) )
				ImGui::OpenPopup( "Type" );

		// Type Popup
		if( ImGui::BeginPopup( "Type",
							   ImGuiWindowFlags_NoTitleBar |
							   ImGuiWindowFlags_NoResize |
							   ImGuiWindowFlags_NoMove |
							   ImGuiWindowFlags_NoCollapse ) ) {
			if( ImGui::MenuItem( "Static" ) )
				pattern.color.endType = ValueType::Static;
			if( ImGui::MenuItem( "Random" ) )
				pattern.color.endType = ValueType::Random;

			ImGui::EndPopup();
		}

		// Value
		if( pattern.color.endType == ValueType::Static ) {
			ImGui::ColorEdit4( "Value", &pattern.color.endMin.x );
			pattern.color.endMax = pattern.color.endMin;
		}
		else if( pattern.color.endType == ValueType::Random ) {
			if( pattern.color.endRandomType == Random::RandomColorType::ShuffleRGB ) {
				if( ImGui::Button( "Random Type: Shuffle RGB" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.endRandomType == Random::RandomColorType::MixRGB ) {
				if( ImGui::Button( "Random Type: Mix RGB" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.endRandomType == Random::RandomColorType::ShuffleHSV ) {
				if( ImGui::Button( "Random Type: Shuffle HSV" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.endRandomType == Random::RandomColorType::MixHSV ) {
				if( ImGui::Button( "Random Type: Mix HSV" ) )
					ImGui::OpenPopup( "RandomType" );
			}
			else if( pattern.color.endRandomType == Random::RandomColorType::SortedHSV ) {
				if( ImGui::Button( "Random Type: Sorted HSV" ) )
					ImGui::OpenPopup( "RandomType" );
			}

			if( ImGui::BeginPopup( "RandomType",
								   ImGuiWindowFlags_NoTitleBar |
								   ImGuiWindowFlags_NoResize |
								   ImGuiWindowFlags_NoMove |
								   ImGuiWindowFlags_NoCollapse ) ) {
				if( ImGui::MenuItem( "Shuffle RGB" ) )
					pattern.color.endRandomType = Random::RandomColorType::ShuffleRGB;
				if( ImGui::MenuItem( "Mix RGB" ) )
					pattern.color.endRandomType = Random::RandomColorType::MixRGB;
				if( ImGui::MenuItem( "Shuffle HSV" ) )
					pattern.color.endRandomType = Random::RandomColorType::ShuffleHSV;
				if( ImGui::MenuItem( "Mix HSV" ) )
					pattern.color.endRandomType = Random::RandomColorType::MixHSV;
				if( ImGui::MenuItem( "Sorted HSV" ) )
					pattern.color.endRandomType = Random::RandomColorType::SortedHSV;

				ImGui::EndPopup();
			}

			ImGui::ColorEdit4( "Min", &pattern.color.endMin.x );
			ImGui::ColorEdit4( "Max", &pattern.color.endMax.x );
		}

		ImGui::PopID();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderNumber( Pattern& pattern ) {
	ImGui::PushID( "Number" );
	// Label
	ImGui::Text( "Number" );
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Type
	if( pattern.number.type == ValueType::Static ) {
		if( ImGui::Button( "Type: Static" ) )
			ImGui::OpenPopup( "Type" );
	}

	else if( pattern.number.type == ValueType::Random ) {
		if( ImGui::Button( "Type: Random" ) )
			ImGui::OpenPopup( "Type" );
	}

	// Type Popup
	if( ImGui::BeginPopup( "Type",
						   ImGuiWindowFlags_NoTitleBar |
						   ImGuiWindowFlags_NoResize |
						   ImGuiWindowFlags_NoMove |
						   ImGuiWindowFlags_NoCollapse ) ) {
		if( ImGui::MenuItem( "Static" ) )
			pattern.number.type = ValueType::Static;
		if( ImGui::MenuItem( "Random" ) )
			pattern.number.type = ValueType::Random;

		ImGui::EndPopup();
	}

	ImGui::Spacing();

	if( pattern.number.type == ValueType::Static ) {
		ImGui::InputInt( "Value", &pattern.number.min );
		if( pattern.number.min < 0 )
			pattern.number.min = 0;

		pattern.number.max = pattern.number.min;
	}
	else if( pattern.number.type == ValueType::Random ) {
		ImGui::InputInt( "Min", &pattern.number.min );
		ImGui::InputInt( "Max", &pattern.number.max );

		if( pattern.number.min < 0 )
			pattern.number.min = 0;
		if( pattern.number.max < 0 )
			pattern.number.max = 0;
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderSize( Pattern& pattern ) {
	ImGui::PushID( "Size" );
	// Label
	ImGui::Text( "Size" );
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Type
	if( pattern.size.type == ValueType::Static ) {
		if( ImGui::Button( "Type: Static" ) )
			ImGui::OpenPopup( "Type" );
	}

	else if( pattern.size.type == ValueType::Random ) {
		if( ImGui::Button( "Type: Random" ) )
			ImGui::OpenPopup( "Type" );
	}

	else if( pattern.size.type == ValueType::Fade ) {
		if( ImGui::Button( "Type: Fade" ) )
			ImGui::OpenPopup( "Type" );
	}

	// Type Popup
	if( ImGui::BeginPopup( "Type",
						   ImGuiWindowFlags_NoTitleBar |
						   ImGuiWindowFlags_NoResize |
						   ImGuiWindowFlags_NoMove |
						   ImGuiWindowFlags_NoCollapse ) ) {
		if( ImGui::MenuItem( "Static" ) )
			pattern.size.type = ValueType::Static;
		if( ImGui::MenuItem( "Random" ) )
			pattern.size.type = ValueType::Random;
		if( ImGui::MenuItem( "Fade" ) )
			pattern.size.type = ValueType::Fade;

		ImGui::EndPopup();
	}

	ImGui::Spacing();

	// Value
	if( pattern.size.type == ValueType::Static ) {
		ImGui::InputFloat( "Value", &pattern.size.startMin );
		pattern.size.startMax = pattern.size.startMin;
		pattern.size.endMin = pattern.size.startMin;
		pattern.size.endMax = pattern.size.startMin;
	}
	else if( pattern.size.type == ValueType::Random ) {
		ImGui::InputFloat( "Min", &pattern.size.startMin );
		ImGui::InputFloat( "Max", &pattern.size.startMax );
		pattern.size.endMin = pattern.size.startMin;
		pattern.size.endMax = pattern.size.startMax;
	}
	else if( pattern.size.type == ValueType::Fade ) {
		// Start
		ImGui::PushID( "Start" );
		ImGui::Text( "Start" );
		if( pattern.size.startType == ValueType::Static ) {
			if( ImGui::Button( "Type: Static" ) )
				ImGui::OpenPopup( "Type" );
		}
		else if( pattern.size.startType == ValueType::Random ) {
			if( ImGui::Button( "Type: Random" ) )
				ImGui::OpenPopup( "Type" );
		}

		// Type Popup
		if( ImGui::BeginPopup( "Type",
							   ImGuiWindowFlags_NoTitleBar |
							   ImGuiWindowFlags_NoResize |
							   ImGuiWindowFlags_NoMove |
							   ImGuiWindowFlags_NoCollapse ) ) {
			if( ImGui::MenuItem( "Static" ) )
				pattern.size.startType = ValueType::Static;
			if( ImGui::MenuItem( "Random" ) )
				pattern.size.startType = ValueType::Random;

			ImGui::EndPopup();
		}

		// Value
		if( pattern.size.startType == ValueType::Static ) {
			ImGui::InputFloat( "Value", &pattern.size.startMin );
			pattern.size.startMax = pattern.size.startMin;
		}
		else if( pattern.size.startType == ValueType::Random ) {
			ImGui::InputFloat( "Min", &pattern.size.startMin );
			ImGui::InputFloat( "Max", &pattern.size.startMax );
		}

		ImGui::PopID();

		ImGui::Spacing();
		ImGui::Spacing();

		// End
		ImGui::PushID( "End" );
		ImGui::Text( "End" );

		// Type
		if( pattern.size.endType == ValueType::Static )
			if( ImGui::Button( "Type: Static" ) )
				ImGui::OpenPopup( "Type" );
		if( pattern.size.endType == ValueType::Random )
			if( ImGui::Button( "Type: Random" ) )
				ImGui::OpenPopup( "Type" );

		// Type Popup
		if( ImGui::BeginPopup( "Type",
							   ImGuiWindowFlags_NoTitleBar |
							   ImGuiWindowFlags_NoResize |
							   ImGuiWindowFlags_NoMove |
							   ImGuiWindowFlags_NoCollapse ) ) {
			if( ImGui::MenuItem( "Static" ) )
				pattern.size.endType = ValueType::Static;
			if( ImGui::MenuItem( "Random" ) )
				pattern.size.endType = ValueType::Random;

			ImGui::EndPopup();
		}

		// Value
		if( pattern.size.endType == ValueType::Static ) {
			ImGui::InputFloat( "Value", &pattern.size.endMin );
			pattern.size.endMax = pattern.size.endMin;
		}
		else if( pattern.size.endType == ValueType::Random ) {
			ImGui::InputFloat( "Min", &pattern.size.endMin );
			ImGui::InputFloat( "Max", &pattern.size.endMax );
		}

		ImGui::PopID();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void BulletPatternEditor::renderLifetime( Pattern& pattern ) {
	ImGui::PushID( "Lifetime" );
	// Label
	ImGui::Text( "Lifetime" );
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Type
	if( pattern.lifetime.type == ValueType::Static )
		if( ImGui::Button( "Type: Static" ) )
			ImGui::OpenPopup( "Type" );

	if( pattern.lifetime.type == ValueType::Random )
		if( ImGui::Button( "Type: Random" ) )
			ImGui::OpenPopup( "Type" );

	// Type Popup
	if( ImGui::BeginPopup( "Type",
						   ImGuiWindowFlags_NoTitleBar |
						   ImGuiWindowFlags_NoResize |
						   ImGuiWindowFlags_NoMove |
						   ImGuiWindowFlags_NoCollapse ) ) {
		if( ImGui::MenuItem( "Static" ) )
			pattern.lifetime.type = ValueType::Static;
		if( ImGui::MenuItem( "Random" ) )
			pattern.lifetime.type = ValueType::Random;

		ImGui::EndPopup();
	}

	ImGui::Spacing();

	// Value
	if( pattern.lifetime.type == ValueType::Static ) {
		ImGui::InputInt( "Value", &pattern.lifetime.min );
		if( pattern.lifetime.min < 0 )
			pattern.lifetime.min = 0;
		pattern.lifetime.max = pattern.lifetime.min;
	}
	else if( pattern.lifetime.type == ValueType::Random ) {
		ImGui::InputInt( "Min", &pattern.lifetime.min );
		ImGui::InputInt( "Max", &pattern.lifetime.max );

		if( pattern.lifetime.min < 0 )
			pattern.lifetime.min = 0;
		if( pattern.lifetime.max < 0 )
			pattern.lifetime.max = 0;
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::PopID();
}

//--------------------------------------------------------------------------------

Gfx::Particle::Pattern BulletPatternEditor::convertPattern( Pattern& pattern ) {
	using namespace Gfx;

	Particle::Pattern out;

		out.setPosition( Particle::Pattern::Position().set( Math::Vec2( pattern.position.min[ 0 ], pattern.position.min[ 1 ] ), Math::Vec2( pattern.position.max[ 0 ], pattern.position.max[ 1 ] ) ) );
		out.setVelocity( Particle::Pattern::Velocity()
						 .setFade( pattern.velocity.fadeX, pattern.velocity.fadeY )
						 .set( Math::Vec2( pattern.velocity.startMin[ 0 ], pattern.velocity.startMin[ 1 ] ), Math::Vec2( pattern.velocity.startMax[ 0 ], pattern.velocity.startMax[ 1 ] ) ) );
		out.setSize( Particle::Pattern::Size()
					 .setFade( pattern.size.type == ValueType::Fade )
					 .set( pattern.size.startMin, pattern.size.startMax ) );
		out.setLifetime( Particle::Pattern::Lifetime()
						 .set( milliseconds( pattern.lifetime.min ), milliseconds( pattern.lifetime.max ) ) );
		out.setNumber( Particle::Pattern::Number()
					   .set( uint16_t( pattern.number.min ), uint16_t( pattern.number.max ) ) );
		out.setColor( Particle::Pattern::Color()
					  .set( Math::Color( sf::Color( pattern.color.startMin ) ) ) );

	return out;
}

//--------------------------------------------------------------------------------

}

//================================================================================