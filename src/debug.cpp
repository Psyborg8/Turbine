//================================================================================

#include "debug.h"

//--------------------------------------------------------------------------------

#include "particle-manager.h"
#include "player.h"
#include "string-utils.h"
#include "system.h"

#include <filesystem>

//================================================================================

namespace Debug {

//--------------------------------------------------------------------------------

struct Timer {
	string name;
	sf::Clock clock;
	list< sf::Time > times;
	sf::Time currentTime;
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
	DebugHandler() = default;

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

		vector< sf::Time > deltaTimes;
		vector< float > framerates;
		float frameRatesMax{ 0.f };

		vector< float > drawCalls;
		float drawCallsMax{ 0.f };
		int draws{ 0 };

		vector< float > renderGroups;
		float renderGroupsMax{ 0.f };
		vector< float > particles;
		float particlesMax{ 0.f };

		vector< float > emitters;
		float emittersMax{ 0.f };

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

void DebugHandler::onUpdate( sf::Time deltaTime ) {
	performance.deltaTimes.push_back( deltaTime );
	if( performance.deltaTimes.size() > 1000u )
		performance.deltaTimes.erase( performance.deltaTimes.begin() );

	float totalTime = .0f;
	for( sf::Time dt : performance.deltaTimes )
		totalTime += dt.asSeconds();

	const float averageTime = totalTime / performance.deltaTimes.size();
	const float fps			= 1.f / averageTime;

	if( fps > performance.frameRatesMax )
		performance.frameRatesMax = fps;
	performance.framerates.push_back( fps );
	if( performance.framerates.size() > 10000u )
		performance.framerates.erase( performance.framerates.begin() );

	if( performance.draws > performance.drawCallsMax )
		performance.drawCallsMax = performance.draws;
	performance.drawCalls.push_back( performance.draws );
	performance.draws = 0;
	if( performance.drawCalls.size() > 1000u )
		performance.drawCalls.erase( performance.drawCalls.begin() );

	const float particles = float( Gfx::Particle::Manager::getParticleCount() );
	performance.particles.push_back( particles );
	if( particles > performance.particlesMax )
		performance.particlesMax = particles;
	if( performance.particles.size() > 1000u )
		performance.particles.erase( performance.particles.begin() );

	for( Timer& timer : timers ) {
		if( timer.currentTime == sf::Time::Zero )
			continue;

		timer.times.push_back( timer.currentTime );
		if( timer.times.size() >= 100u )
			timer.times.erase( timer.times.begin() );

		timer.currentTime = sf::Time::Zero;
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
			float totalTime = .0f;
			for( sf::Time dt : performance.deltaTimes )
				totalTime += dt.asSeconds();

			const float averageTime = totalTime / performance.deltaTimes.size();
			const int fps			= int( 1.f / averageTime );
			string text				= Utils::format( "FPS: %i", fps );
			ImGui::Text( "%s", text.c_str() );

			ImGui::PlotLines( "",
							  performance.framerates.data(),
							  int( performance.framerates.size() ),
							  0,
							  ( const char* ) 0,
							  0.f,
							  performance.frameRatesMax,
							  ImVec2( 0.f, 100.f ) );

			if( performance.drawCalls.size() > 0u ) {
				ImGui::Spacing();
				ImGui::Spacing();
				text = Utils::format(
					"Draw Calls: %i", int( *performance.drawCalls.rbegin() ) );
				ImGui::Text( "%s", text.c_str() );
				ImGui::PlotHistogram( "",
									  performance.drawCalls.data(),
									  int( performance.drawCalls.size() ),
									  0,
									  ( const char* ) 0,
									  0.f,
									  performance.drawCallsMax,
									  ImVec2( 0.f, 100.f ) );
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
	it->currentTime += time;
}

//--------------------------------------------------------------------------------

float getAverageTime( string name ) {
	const auto it
		= std::find_if( timers.begin(), timers.end(), [name]( const Timer& timer ) {
			  return timer.name == name;
		  } );

	if( it == timers.end() )
		return 0.0f;

	const list< sf::Time >& vt = it->times;

	float total = 0.0f;
	for( const sf::Time& time : vt )
		total += time.asSeconds();

	return total / vt.size();
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
