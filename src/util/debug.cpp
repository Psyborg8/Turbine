//================================================================================

#include "debug.h"

//--------------------------------------------------------------------------------

#include "input.h"
#include "particle-manager.h"
#include "string-utils.h"
#include "system.h"
#include "utils.h"

#include <filesystem>

//================================================================================

namespace Debug {

//--------------------------------------------------------------------------------

struct Timer {
	string name;
	sf::Clock clock;
	Counter< float, 500u > times;
	sf::Time current_time;
};

//--------------------------------------------------------------------------------

struct DebugMessage {
	string text;
	DebugType type;
	float alpha;
};

//--------------------------------------------------------------------------------

vector< DebugMessage > messages;
vector< Timer > timers;

//================================================================================

// Console

//================================================================================

struct Command {
	string command;
	size_t args;
	function< void( vector< string > args ) > func;
};

struct SimpleCommand {
	string name;
	string helper;
	function< void( string arg ) > func;
};

//--------------------------------------------------------------------------------

void callCommand( string command );

//--------------------------------------------------------------------------------

vector< Command > commands;
vector< SimpleCommand > setCommands;
vector< SimpleCommand > showCommands;
vector< SimpleCommand > hideCommands;

//--------------------------------------------------------------------------------

void addSetCommand( string name, bool& value, string helper ) {
	setCommands.push_back( SimpleCommand{ name, helper, [&value]( const string& arg ) {
											 if( arg == "true" )
												 value = true;
											 else if( arg == "false" )
												 value = false;
											 else
												 addMessage( "true/false value expected.",
															 DebugType::Error );
										 } } );
}

//--------------------------------------------------------------------------------

void addSetCommand( string name, float& value, string helper ) {
	setCommands.push_back( SimpleCommand{
		name, helper, [&value]( const string& arg ) { value = std::stof( arg ); } } );
}

//--------------------------------------------------------------------------------

void addSetCommand( string name, int& value, string helper ) {
	setCommands.push_back( SimpleCommand{
		name, helper, [&value]( const string& arg ) { value = std::stoi( arg ); } } );
}

//--------------------------------------------------------------------------------

void addSetCommand( string name, unsigned int& value, string helper ) {
	setCommands.push_back( SimpleCommand{
		name, helper, [&value]( const string& arg ) { value = std::stoi( arg ); } } );
}

//--------------------------------------------------------------------------------

void addSetCommand( string name, size_t& value, string helper ) {
	setCommands.push_back( SimpleCommand{
		name, helper, [&value]( const string& arg ) { value = std::stoi( arg ); } } );
}

//--------------------------------------------------------------------------------

void addSetCommand( string name, Math::Vec2& value, string helper ) {}

//--------------------------------------------------------------------------------

void addSetCommand( string name, milliseconds& value, string helper ) {
	setCommands.push_back( SimpleCommand{ name, helper, [&value]( const string& arg ) {
											 value = milliseconds( std::stoi( arg ) );
										 } } );
}

//--------------------------------------------------------------------------------

void addShowCommand( string name, function< void() > callback, string helper ) {
	showCommands.push_back( SimpleCommand{
		name, helper, [callback]( const string& arg ) { callback(); } } );
}

//--------------------------------------------------------------------------------

void addHideCommand( string name, function< void() > callback, string helper ) {
	hideCommands.push_back( SimpleCommand{
		name, helper, [callback]( const string& arg ) { callback(); } } );
}

//--------------------------------------------------------------------------------

void addCommand( string command,
				 size_t args,
				 function< void( vector< string > args ) > func,
				 string helper ) {
	commands.push_back( Command{ command, args, func } );
}

//--------------------------------------------------------------------------------

void addCommand( string command, function< void() > func, string helper ) {
	commands.push_back(
		Command{ command, 0u, [func]( const vector< string >& args ) { func(); } } );
}

//--------------------------------------------------------------------------------

void callCommand( string command ) {
	vector< string > args = Utils::tokenize( command, " " );

	if( args.empty() )
		return;

	// Show
	if( args.at( 0 ) == "show" ) {
		if( args.size() <= 1u )
			goto error_argument;

		const auto it = std::find_if(
			showCommands.begin(), showCommands.end(), [args]( const SimpleCommand& command ) {
				return command.name == args.at( 1 );
			} );

		if( it == showCommands.end() )
			goto error_argument_missing;

		it->func( args.at( 1 ) );
		return;
	}

	// Hide
	if( args.at( 0 ) == "hide" ) {
		if( args.size() <= 1u )
			goto error_argument;

		const auto it = std::find_if(
			hideCommands.begin(), hideCommands.end(), [args]( const SimpleCommand& command ) {
				return command.name == args.at( 1 );
			} );

		if( it == hideCommands.end() )
			goto error_argument_missing;

		it->func( args.at( 1 ) );
		return;
	}

	// Set
	if( args.at( 0 ) == "set" ) {
		if( args.size() <= 1u )
			goto error_argument;
		if( args.size() <= 2u )
			goto error_argument;

		const auto it = std::find_if(
			setCommands.begin(), setCommands.end(), [args]( const SimpleCommand& command ) {
				return command.name == args.at( 1 );
			} );

		if( it == setCommands.end() )
			goto error_argument_missing;

		it->func( args.at( 2 ) );
		return;
	}

	// Commands
	{
		const auto it = std::find_if(
			commands.begin(), commands.end(), [args]( const Command& command ) {
				return command.command == args.at( 0 );
			} );

		if( it == commands.end() )
			goto error_missing;

		if( args.size() <= it->args )
			goto error_argument;

		it->func( args );
		return;
	}

	// Errors
error_missing:
	addMessage( "Command \"" + args.at( 0 ) + "\" not recognized.", DebugType::Error );
	return;

error_argument:
	addMessage( "Command \"" + command + "\" expects an argument.", DebugType::Error );
	return;

error_argument_missing:
	addMessage( "Argument \"" + args.at( 1 ) + "\" not valid.", DebugType::Error );
	return;
}

//================================================================================

// Handler

//================================================================================

class DebugHandler : public Object {
private:
public:
	DebugHandler()			= default;
	virtual ~DebugHandler() = default;

public:
	void onSpawnChildren() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender( sf::RenderTarget* target ) override;
	void onEvent( sf::Event e ) override;

public:
	void show() { menu.open = true; }
	void hide() { menu.open = false; }
	void incDrawCall() { performance.draws++; }
	void addPerformancePage( string name, function< string() > func );

private:
	struct {
		bool open{ false };

	} menu;

	struct {
		bool open{ false };
		std::array< char, 1024u > buffer;
	} console;


	struct {
		bool open{ false };

		Counter< float, 500u > frame_rates_counter;
		Counter< float, 500u > draw_calls_counter;
		Counter< float, 500u > particles_counter;

		int draws{ 0 };

		vector< pair< string, function< string() > > > pages;
	} performance;
};

//--------------------------------------------------------------------------------

shared_ptr< DebugHandler > handler;

//--------------------------------------------------------------------------------

void DebugHandler::onSpawnChildren() {
	setVisibility( true );
	console.buffer.fill( '\0' );

	addShowCommand( "debug_menu", std::bind( &DebugHandler::show, this ) );
	addShowCommand( "debug_console", [this] { console.open = true; } );
	addShowCommand( "debug_performance", [this] { performance.open = true; } );

	addHideCommand( "debug_menu", std::bind( &DebugHandler::hide, this ) );
	addHideCommand( "debug_console", [this] { console.open = false; } );
	addHideCommand( "debug_performance", [this] { performance.open = false; } );
}

//--------------------------------------------------------------------------------

void DebugHandler::onUpdate( sf::Time _delta_time ) {
	performance.frame_rates_counter.push( 1.f / _delta_time.asSeconds() );

	performance.draw_calls_counter.push( performance.draws );
	performance.draws = 0u;

	performance.particles_counter.push( Gfx::Particle::Manager::getParticleCount() );
	for( Timer& timer : timers ) {
		if( timer.current_time == sf::Time::Zero )
			continue;

		timer.times.push( timer.current_time.asSeconds() );
		timer.current_time = sf::Time::Zero;
	}
}

//--------------------------------------------------------------------------------

void DebugHandler::onRender( sf::RenderTarget* target ) {
	ImGui::PushID( "DebugHandler" );

	if( menu.open ) {
		ImGui::PushID( "Menu" );
		ImGui::Begin( "Debug Menu",
					  &menu.open,
					  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
						  | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove );

		if( ImGui::Button( "Console" ) )
			console.open = !console.open;
		if( ImGui::Button( "Performance" ) )
			performance.open = !performance.open;
		if( ImGui::Button( "Draw Debug" ) )
			System::getSystemInfo().drawDebug = !System::getSystemInfo().drawDebug;

		ImGui::SetWindowPos( ImVec2(
			System::getSystemInfo().width - ImGui::GetWindowSize().x, 400.f ) );

		ImGui::End();
		ImGui::PopID();
	}

	if( console.open ) {
		ImGui::PushID( "Console" );
		ImGui::Begin( "Console",
					  &console.open,
					  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
						  | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove );

		ImGui::SetWindowSize( ImVec2( 1200.f, 537.f ) );
		ImGui::SetWindowPos( ImVec2(
			( System::getSystemInfo().width / 2.f ) - ( ImGui::GetWindowSize().x / 2.f ), .0f ) );
		ImGui::SetWindowFocus();

		ImGui::BeginChild( "Message Box",
						   ImVec2( 0.f, 500.f ),
						   true,
						   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
							   | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove );

		for( DebugMessage message : messages ) {
			ImColor color;
			if( message.type == DebugType::None )
				color = ImColor( 150, 150, 150, int( message.alpha * 255.0f ) );
			else if( message.type == DebugType::Info )
				color = ImColor( 100, 200, 200, int( message.alpha * 255.0f ) );
			else if( message.type == DebugType::Warning )
				color = ImColor( 200, 150, 50, int( message.alpha * 255.0f ) );
			else if( message.type == DebugType::Error )
				color = ImColor( 200, 50, 50, int( message.alpha * 255.0f ) );
			else if( message.type == DebugType::Input )
				color = ImColor( 150, 100, 200, int( message.alpha * 255.0f ) );
			else if( message.type == DebugType::Performance )
				color = ImColor( 225, 180, 50, int( message.alpha * 255.0f ) );

			ImGui::TextColored( color, "%s", message.text.c_str() );
		}

		ImGui::EndChild();

		if( ImGui::IsWindowAppearing() )
			ImGui::SetKeyboardFocusHere();

		ImGui::PushItemWidth( 1183 );
		if( ImGui::InputText( "##Input",
							  console.buffer.data(),
							  std::numeric_limits< uint16_t >::max(),
							  ImGuiInputTextFlags_EnterReturnsTrue ) ) {
			callCommand( string( console.buffer.data() ) );
			console.buffer.fill( '\0' );
		}
		ImGui::PopItemWidth();

		ImGui::End();
		ImGui::PopID();
	}

	if( performance.open ) {
		ImGui::PushID( "Performance" );
		ImGui::Begin( "Performance##Debug",
					  &performance.open,
					  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar
						  | ImGuiWindowFlags_NoCollapse );

		ImGui::SetWindowFontScale( 1.1f );
		// ImGui::SetWindowPos( ImVec2( 0.f, 0.f ) );

		ImGui::BeginTabBar( "Performance Tab Bar" );

		if( ImGui::BeginTabItem( "FPS" ) ) {
			const int fps = performance.frame_rates_counter.average();
			string text	  = Utils::format( "FPS: %i", fps );
			ImGui::Text( "%s", text.c_str() );

			const auto data = performance.frame_rates_counter.data();
			const auto size = data.size();
			const auto max	= performance.frame_rates_counter.max();
			ImGui::PlotLines(
				"", data.data(), size, 0, ( const char* ) 0, 0.f, max, ImVec2( 0.f, 100.f ) );

			if( performance.draw_calls_counter.average() > 0u ) {
				ImGui::Spacing();
				ImGui::Spacing();
				text = Utils::format(
					"Draw Calls: %i", int( performance.draw_calls_counter.average() ) );
				ImGui::Text( "%s", text.c_str() );

				const auto data = performance.draw_calls_counter.data();
				const auto size = data.size();
				const auto max	= performance.draw_calls_counter.max();
				ImGui::PlotHistogram(
					"", data.data(), size, 0, ( const char* ) 0, 0.f, max, ImVec2( 0.f, 100.f ) );
			}

			ImGui::EndTabItem();
		}

		if( ImGui::BeginTabItem( "Timers" ) ) {
			string format;
			for( Timer timer : timers )
				format += Utils::format( "\n%s: %.3fms",
										 timer.name.c_str(),
										 getAverageTime( timer.name ) * 1000.f );

			ImGui::Text( "%s", format.c_str() );
			ImGui::EndTabItem();
		}

		for( const pair< string, function< string() > >& page : performance.pages ) {
			if( ImGui::BeginTabItem( page.first.c_str() ) ) {
				ImGui::Text( "%s", page.second().c_str() );
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
		ImGui::End();
		ImGui::PopID();
	}

	ImGui::PopID();
}

//--------------------------------------------------------------------------------

void DebugHandler::onEvent( sf::Event e ) {
	if( e.type == sf::Event::KeyPressed ) {
		if( e.key.shift && e.key.code == sf::Keyboard::Z ) {
			menu.open = !menu.open;
			return;
		}
		if( e.key.code == sf::Keyboard::F11 ) {
			console.open = !console.open;
			return;
		}
		if( e.key.code == sf::Keyboard::F12 ) {
			performance.open = !performance.open;
			return;
		}
	}
}

//--------------------------------------------------------------------------------

void DebugHandler::addPerformancePage( string name, function< string() > func ) {
	performance.pages.push_back( make_pair( name, func ) );
}

//--------------------------------------------------------------------------------

void incDrawCall() {
	handler->incDrawCall();
}

//--------------------------------------------------------------------------------

void addPerformancePage( string name, function< string() > func ) {
	handler->addPerformancePage( name, func );
}
//================================================================================

// UI

//================================================================================

void init( Object* world ) {
	handler = Object::makeObject< DebugHandler >( world );

	Input::bindButton( "Show Debug Window",
					   ControllerButton::None,
					   sf::Keyboard::Key::D,
					   []( bool ) { show(); } );
}

//--------------------------------------------------------------------------------

void show() {
	handler->show();
}

//--------------------------------------------------------------------------------

void hide() {
	handler->hide();
}

//================================================================================

// Timers

//================================================================================

void startTimer( string name ) {
	const auto it
		= std::find_if( timers.begin(), timers.end(), [name]( const Timer& timer ) {
			  return timer.name == name;
		  } );

	if( it != timers.end() ) {
		it->clock.restart();
		return;
	}

	Timer timer;
	timer.name = name;
	timer.clock.restart();
	timers.push_back( timer );

	std::sort( timers.begin(), timers.end(), []( const Timer& a, const Timer& b ) {
		return a.name < b.name;
	} );
}

//--------------------------------------------------------------------------------

void stopTimer( string name ) {
	const auto it
		= std::find_if( timers.begin(), timers.end(), [name]( const Timer& timer ) {
			  return timer.name == name;
		  } );

	if( it == timers.end() )
		return;


	sf::Time time = it->clock.restart();
	it->current_time += time;
}

//--------------------------------------------------------------------------------

float getAverageTime( string name ) {
	const auto it
		= std::find_if( timers.begin(), timers.end(), [name]( const Timer& timer ) {
			  return timer.name == name;
		  } );

	if( it == timers.end() )
		return 0.0f;

	return it->times.average();
}

//================================================================================

// Messages

//================================================================================

void addMessage( string message, DebugType type ) {
	DebugMessage out;
	out.text  = message;
	out.alpha = 1.0;
	out.type  = type;

	messages.push_back( out );
}

//--------------------------------------------------------------------------------

string getLastMessage() {
	if( messages.empty() )
		return string();
	return messages.rbegin()->text;
}

//--------------------------------------------------------------------------------

}	 // namespace Debug

//================================================================================
