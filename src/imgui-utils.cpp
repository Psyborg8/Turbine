//================================================================================

#include "imgui-utils.h"

//--------------------------------------------------------------------------------

#include <filesystem>

#include "system.h"
#include "string-utils.h"

//================================================================================

namespace ImGui {

//--------------------------------------------------------------------------------

bool render( Math::ValueSet< Math::Vec2 >& value, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	if( id[ 0 ] != '#' ) {
		ImGui::Text( id );
		ImGui::Spacing();
	}

	out |= Checkbox( "Random", &value.random );
	if( value.random ) {
		ImGui::SameLine();
		out |= Checkbox( "Lock", &value.lock );
	}
	ImGui::SameLine();
	out |= Checkbox( "Inverse", &value.inverse );

	if( value.random ) {
		out |= ImGui::InputFloat2( "Min", &value.min.x, 3 );
		out |= ImGui::InputFloat2( "Max", &value.max.x, 3 );
	}
	else
		out |= ImGui::InputFloat2( "Value", &value.min.x, 3 );

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool render( Math::ValueSet< Math::Color >& value, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	if( id[ 0 ] != '#' ) {
		ImGui::Text( id );
		ImGui::Spacing();
	}

	out |= Checkbox( "Random", &value.random );
	if( value.random ) {
		ImGui::SameLine();
		out |= Checkbox( "Lock", &value.lock );
		ImGui::SameLine();
		out |= Checkbox( "HSV", &value.hsv );
	}

	if( value.random ) {
		out |= ImGui::ColorEdit4( "Min", &value.min.r );
		out |= ImGui::ColorEdit4( "Max", &value.max.r );
	}
	else
		out |= ImGui::ColorEdit4( "Value", &value.min.r );

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool render( Math::ValueSet< float >& value, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	if( id[ 0 ] != '#' ) {
		ImGui::Text( id );
		ImGui::Spacing();
	}

	out |= Checkbox( "Random", &value.random );
	ImGui::SameLine();
	out |= Checkbox( "Inverse", &value.inverse );

	if( value.random ) {
		out |= ImGui::InputFloat( "Min", &value.min, 3 );
		out |= ImGui::InputFloat( "Max", &value.max, 3 );
	}
	else
		out |= ImGui::InputFloat( "Value", &value.min, 3 );

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool render( Math::ValueSet< int >& value, const char* id ) {
	bool out = false;

	ImGui::PushID( id );

	if( id[ 0 ] != '#' ) {
		ImGui::Text( id );
		ImGui::Spacing();
	}

	out |= Checkbox( "Random", &value.random );
	ImGui::SameLine();
	out |= Checkbox( "Inverse", &value.inverse );

	if( value.random ) {
		out |= ImGui::InputInt( "Min", &value.min );
		out |= ImGui::InputInt( "Max", &value.max );
	}
	else
		out |= ImGui::InputInt( "Value", &value.min );

	ImGui::PopID();

	return out;
}

//--------------------------------------------------------------------------------

bool patternSelectorOpen{ false };
function< void( string ) > patternSelectorCallback{ nullptr };

//--------------------------------------------------------------------------------

void openPatternSelector( function< void( string ) > callback ) {
	if( callback == nullptr )
		return;

	patternSelectorCallback = callback;
	patternSelectorOpen = true;
}

//--------------------------------------------------------------------------------

void renderPatternSelector() {
	if( !patternSelectorOpen )
		return;

	ImGui::OpenPopup( "Pattern Selector" );
	ImGui::BeginPopupModal( "Pattern Selector", &patternSelectorOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );

	ImGui::SetWindowSize( ImVec2( 600, 600 ) );
	ImGui::SetWindowPos( ImVec2( System::getSystemInfo().width / 2.f - ImGui::GetWindowWidth() / 2.f,
								 System::getSystemInfo().height / 2.f - ImGui::GetWindowHeight() / 2.f ) );

	using namespace std::filesystem;
	
	function< bool( path directory ) > iterate;
	iterate = [&iterate]( path directory ) {
		bool out = false;
		directory_iterator iterator{ directory };

		for( directory_entry element : iterator ) {
			if( !element.is_directory() )
				continue;

			string name = element.path().filename().string();
			string dir = directory.string();
			string id = Utils::format( "%s##%s", name.c_str(), dir.c_str() );
			if( ImGui::TreeNodeEx( id.c_str(), ImGuiTreeNodeFlags_Framed ) ) {
				out |= iterate( element.path() );
				ImGui::TreePop();
			}
			ImGui::Spacing();
		}

		iterator = directory_iterator( directory );
		for( directory_entry element : iterator ) {
			if( !element.is_regular_file() || !( element.path().extension() == ".bullet" ) )
				continue;

			if( ImGui::Selectable( Utils::format( "%s##%s", element.path().filename().string().c_str(), directory.string().c_str() ).c_str() ) ) {
				string str = element.path().string();
				std::replace( str.begin(), str.end(), '\\', '/' );
				patternSelectorCallback( str );
				out = true;
			}
		}

		return out;
	};

	bool done = iterate( path( Folders::Bullets ) );

	if( patternSelectorOpen )
		ImGui::EndPopup();

	patternSelectorOpen &= !done;
}

//--------------------------------------------------------------------------------

bool renameWindowOpen{ false };
function< void( string ) > renameWindowCallback{ nullptr };
string renameWindowBuffer;

//--------------------------------------------------------------------------------

void openRenamePopup( function< void( string ) > callback, string current ) {
	if( callback == nullptr )
		return;

	renameWindowBuffer = current;
	renameWindowBuffer.resize( 256u );
	renameWindowOpen = true;
	renameWindowCallback = callback;
}

//--------------------------------------------------------------------------------

void renderRenamePopup() {
	if( !renameWindowOpen )
		return;

	ImGui::OpenPopup( "Rename" );
	ImGui::BeginPopupModal( "Rename", &renameWindowOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar );
	ImGui::SetWindowSize( ImVec2( 200, 80 ) );
	ImGui::SetWindowPos( ImVec2( System::getSystemInfo().width / 2.f - ImGui::GetWindowWidth() / 2.f,
								 System::getSystemInfo().height / 2.f - ImGui::GetWindowHeight() / 2.f ) );

	bool done = false;

	if( ImGui::IsWindowAppearing() )
		ImGui::SetKeyboardFocusHere();

	ImGui::PushItemWidth( 183 );
	if( ImGui::InputText( "##RenameText", renameWindowBuffer.data(), 256u,
						  ImGuiInputTextFlags_EnterReturnsTrue
						  | ImGuiInputTextFlags_AutoSelectAll ) ) {

		renameWindowCallback( renameWindowBuffer );
		done = true;
	}
	ImGui::PopItemWidth();

	if( ImGui::Button( "Apply" ) ) {
		renameWindowCallback( renameWindowBuffer );
		done = true;
	}

	if( renameWindowOpen )
		ImGui::EndPopup();

	renameWindowOpen &= !done;
}

//--------------------------------------------------------------------------------

}

//================================================================================