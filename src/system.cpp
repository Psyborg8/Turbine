//================================================================================

#include "system.h"

//--------------------------------------------------------------------------------

// Systems
#include "random.h"
#include "timer.h"
#include "debug.h"
#include "input.h"

// Worlds
#include "particle-editor.h"

//================================================================================

namespace System
{

//================================================================================

void update();

//--------------------------------------------------------------------------------

shared_ptr< World > world;
sf::Time deltaTime;

//--------------------------------------------------------------------------------

SystemInfo systemInfo;
bool shouldExit{ false };

//--------------------------------------------------------------------------------

sf::RenderWindow window;
sf::Clock clock;

//================================================================================

bool init( int argc, char** argv ) {

	//window.create( sf::VideoMode::getFullscreenModes()[0], "Project Bullet", sf::Style::Fullscreen  );
	window.create( sf::VideoMode( systemInfo.width, systemInfo.height ), "Project Bullet" );
	window.setVerticalSyncEnabled( false );
	window.setKeyRepeatEnabled( false );

	systemInfo.width = window.getSize().x;
	systemInfo.height = window.getSize().y;

	// Init ImGui
	ImGui::SFML::Init( window, true, &Debug::incDrawCall );

	// Init timers
	Timers::init();

	// Load World
	Debug::startTimer( "System::Make World" );
	//world = Object::makeObject< Worlds::SFMLWorld >( nullptr );
	world = Object::makeObject< Editor::BulletPatternEditor >( nullptr );
	Debug::stopTimer( "System::Make World" );

	// Init debug handler
	Debug::init( world.get() );

	// Init Particle System
	Gfx::Particle::init( world.get() );

	Debug::addSetCommand( "draw_debug", systemInfo.drawDebug );
	Debug::addSetCommand( "system_width", systemInfo.width );
	Debug::addSetCommand( "system_height", systemInfo.height );


	return true;
}

//--------------------------------------------------------------------------------

int start() {
	Input::start( world.get() );
	
	Debug::startTimer( "System::Start" );
	world->start();
	Debug::stopTimer( "System::Start" );

	while( window.isOpen() )
		update();

	return 0;
}

//--------------------------------------------------------------------------------

void exit() {
	shouldExit = true;
}

//--------------------------------------------------------------------------------

SystemInfo& getSystemInfo() {
	return systemInfo;
}

//--------------------------------------------------------------------------------

shared_ptr< World > getWorld() {
	return world;
}

//--------------------------------------------------------------------------------

sf::RenderWindow* getWindow() {
	return &window;
}

//--------------------------------------------------------------------------------

sf::Time getDeltaTime() {
	return deltaTime;
}                                                         

//================================================================================

void update() {
	// Get delta time
	sf::Time time = clock.restart();

	// Limit the deltaTime if the frame took too long.
	// So we can stop the game during breakpoints.
#ifdef _DEBUG
	if( time.asMilliseconds() > 100 )
		time = sf::Time( sf::milliseconds( 17 ) );
#endif

	deltaTime = time;

	// Update the current world
	// Event handling
	Debug::startTimer( "System::Event Handling" );
	sf::Event e;
	while( window.pollEvent( e ) ) {
		ImGui::SFML::ProcessEvent( e );
		// Filter out the most expensive events.
		// Use sf::Mouse::getPosition or sf::Joystick::getAxisPosition instead
		if( e.type == sf::Event::MouseMoved )
			break;
		if( e.type == sf::Event::JoystickMoved )
			break;

		world->event( e );

		if( e.type == sf::Event::KeyPressed )
			if( e.key.code == sf::Keyboard::Escape ) {
				window.close();
				ImGui::SFML::Shutdown();
				return;
			}
		if( e.type == sf::Event::Resized ) {
			systemInfo.width = e.size.width;
			systemInfo.height = e.size.height;
			window.setSize( sf::Vector2u( systemInfo.width, systemInfo.height ) );
			world->getCamera().calculate();
		}
		if( e.type == sf::Event::Closed )
			window.close();
	}
	Debug::stopTimer( "System::Event Handling" );

	// Update physics
	Debug::startTimer( "System::Update" );
	ImGui::SFML::Update( window, deltaTime );
	world->update( time );
	Debug::stopTimer( "System::Update" );
	Debug::startTimer( "System::Process Collisions" );
	world->processCollisions();
	Debug::stopTimer( "System::Process Collisions" );
	Debug::startTimer( "System::Post Update" );
	world->postUpdate( time );
	Debug::stopTimer( "System::Post Update" );

	// Cleanup
	Debug::startTimer( "System::Cleanup" );
	Gfx::Emitter::cleanup();
	Gfx::Particle::cleanup();
	Object::cleanupObjects();
	Debug::stopTimer( "System::Cleanup" );

	// Render
	window.clear( world->getBackgroundColor().sf() );

	Debug::startTimer( "System::Render" );
	window.resetGLStates();
	world->render();
	window.resetGLStates();
	ImGui::SFML::Render();
	window.resetGLStates();

	Debug::stopTimer( "System::Render" );
	Debug::startTimer( "System::Post Render" );
	world->postRender();
	Debug::stopTimer( "System::Post Render" );

	Debug::startTimer( "System::Display" );
	window.display();
	Debug::stopTimer( "System::Display" );
}

//================================================================================

} // System