//================================================================================

#include "system.h"

//--------------------------------------------------------------------------------

// Systems
#include "app.h"
#include "debug.h"
#include "input.h"
#include "particle-affector.h"
#include "particle-manager.h"
#include "random.h"
#include "timer.h"

//================================================================================

namespace System {

//================================================================================

void update();

//--------------------------------------------------------------------------------

shared_ptr< App > app;
sf::Time deltaTime;

//--------------------------------------------------------------------------------

SystemInfo systemInfo;
bool shouldExit{ false };

//--------------------------------------------------------------------------------

sf::RenderWindow window;
sf::Clock clock;

//================================================================================

bool init( shared_ptr< App > _app ) {

	// window.create( sf::VideoMode::getFullscreenModes()[0], "Project Bullet",
	// sf::Style::Fullscreen  );
	window.create( sf::VideoMode( systemInfo.width, systemInfo.height ), "Project Bullet" );
	window.setVerticalSyncEnabled( false );
	window.setKeyRepeatEnabled( false );

	systemInfo.width  = window.getSize().x;
	systemInfo.height = window.getSize().y;

	// Init ImGui
	if( !ImGui::SFML::Init( window, true ) )
		return false;

	// Init timers
	Timers::init();

	// Load World
	app = _app;

	// Init debug handler
	Debug::init( app.get() );

	// Init particles
	Gfx::Particle::Manager::init();
	Gfx::Particle::Affector::init();

	Debug::addSetCommand( "draw_debug", systemInfo.drawDebug );
	Debug::addSetCommand( "system_width", systemInfo.width );
	Debug::addSetCommand( "system_height", systemInfo.height );

	return true;
}

//--------------------------------------------------------------------------------

int start() {
	Input::start( app.get() );

	app->start();

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

shared_ptr< App > getApp() {
	return app;
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
	Debug::startTimer( "Tick" );

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
	Debug::startTimer( "System - Event Handling" );
	sf::Event e;
	while( window.pollEvent( e ) ) {
		ImGui::SFML::ProcessEvent( e );
		// Filter out the most expensive events.
		// Use sf::Mouse::getPosition or sf::Joystick::getAxisPosition instead
		if( e.type == sf::Event::MouseMoved )
			break;
		if( e.type == sf::Event::JoystickMoved )
			break;

		app->event( e );

		if( e.type == sf::Event::KeyPressed )
			if( e.key.code == sf::Keyboard::Escape ) {
				window.close();
				ImGui::SFML::Shutdown();
				return;
			}
		if( e.type == sf::Event::Resized ) {
			systemInfo.width  = e.size.width;
			systemInfo.height = e.size.height;
			window.setSize( sf::Vector2u( systemInfo.width, systemInfo.height ) );
		}
		if( e.type == sf::Event::Closed )
			window.close();
	}
	Debug::stopTimer( "System - Event Handling" );

	Debug::startTimer( "System - Update" );
	// Update physics
	Gfx::Particle::Manager::update( deltaTime );
	ImGui::SFML::Update( window, deltaTime );
	app->update( deltaTime );
	Debug::stopTimer( "System - Update" );

	Debug::startTimer( "System - Process Collisions" );
	app->processCollisions();
	Debug::stopTimer( "System - Process Collisions" );

	Debug::startTimer( "System - Post Update" );
	Gfx::Particle::Manager::postUpdate( deltaTime );
	app->postUpdate( deltaTime );
	Debug::stopTimer( "System - Post Update" );

	Debug::startTimer( "System - Cleanup" );
	// Cleanup
	Object::cleanupObjects();
	Debug::stopTimer( "System - Cleanup" );

	Debug::startTimer( "System - Render" );
	// Render
	window.clear( app->getBackgroundColor().sf() );

	window.resetGLStates();
	Gfx::Particle::Manager::render( &window );
	app->render( &window );
	window.resetGLStates();
	ImGui::SFML::Render();
	window.resetGLStates();
	Debug::stopTimer( "System - Render" );

	Debug::startTimer( "System - Post Render" );
	app->postRender( &window );
	Debug::stopTimer( "System - Post Render" );

	Debug::startTimer( "System - Display" );
	window.display();
	Debug::stopTimer( "System - Display" );

	Debug::stopTimer( "Tick" );
}

//--------------------------------------------------------------------------------

}	 // namespace System

//================================================================================
