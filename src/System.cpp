//================================================================================

#include "System.h"

//--------------------------------------------------------------------------------

// Systems
#include "Random.h"
#include "Timer.h"
#include "Debug.h"

// Worlds
#include "SFMLWorld.h"

//================================================================================

namespace System
{

//================================================================================

void update();

//--------------------------------------------------------------------------------

shared_ptr< Worlds::World > world;
sf::Time deltaTime;

//--------------------------------------------------------------------------------

SystemInfo systemInfo;
bool shouldExit{ false };

//--------------------------------------------------------------------------------

sf::Font font;

//--------------------------------------------------------------------------------

sf::RenderWindow window;
sf::Clock clock;

//================================================================================

// PUBLIC

//================================================================================

bool init( int argc, char** argv ) {

	//window.create( sf::VideoMode::getFullscreenModes()[0], "Project Bullet", sf::Style::Fullscreen  );
	window.create( sf::VideoMode( 1920u, 1080u ), "Project Bullet" );
	window.setVerticalSyncEnabled( true );

	systemInfo.width = window.getSize().x;
	systemInfo.height = window.getSize().y;

	// Init timers
	Timers::init();

	return true;
}

//--------------------------------------------------------------------------------

int start() {
	Debug::startTimer( "System::Make World" );
	world = Object::makeObject< Worlds::SFMLWorld >( nullptr );
	// world = Object::makeObject< CollisionWorld >( nullptr );
	Debug::stopTimer( "System::Make World" );

	world->loadWorld( "Dash Precision World" );
	// world->loadWorld( "Juggle Time Tower" );
	
	vector< shared_ptr< Object > > objects = Object::getObjects( getWorld(), "", true );
	
	for( shared_ptr< Object > object : objects )
		object->onStart();

	font.loadFromFile( Folders::Fonts + "arial.ttf" );

	while( window.isOpen() )
		update();

	return 0;
}

//--------------------------------------------------------------------------------

void exit() {
	shouldExit = true;
}

//--------------------------------------------------------------------------------

SystemInfo getSystemInfo() {
	return systemInfo;
}

//--------------------------------------------------------------------------------

shared_ptr< Worlds::World > getWorld() {
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

// LOCAL

//================================================================================

void update() {
	// Get delta time
	sf::Time time = clock.restart();

	// Limit the deltaTime if the frame took too long.
	// So we can stop the game during breakpoints.
	if( time.asMilliseconds() == 100 )
		time = sf::Time( sf::milliseconds( 100 ) );

	deltaTime = time;

	// Update timers
	Timers::update();

	// Update the current world
	Debug::startTimer( "System::Get World Objects" );
	vector< shared_ptr< Object > > objects = Object::getObjects( getWorld(), "", true );
	Debug::stopTimer( "System::Get World Objects" );

	Debug::startTimer( "System::Event Handling" );
	// Event handling
	sf::Event e;
	while( window.pollEvent( e ) ) {
		Debug::startTimer( "System::Event Notify" );
		for( shared_ptr< Object > object : objects ) {
			object->onEvent( e );

			if( e.type == sf::Event::KeyPressed )
				if( e.key.code == sf::Keyboard::Escape )
					window.close();
			if( e.type == sf::Event::Resized ) {
				systemInfo.width = e.size.width;
				systemInfo.height = e.size.height;
				world->getCamera().calculate();
			}
			if( e.type == sf::Event::Closed )
				window.close();
		}
		Debug::stopTimer( "System::Event Notify" );
	}

	Debug::stopTimer( "System::Event Handling" );

	Debug::startTimer( "System::Update" );
	// Update physics
	for( shared_ptr< Object > object : objects )
		object->onUpdate( time );
	Debug::stopTimer( "System::Update" );
	Debug::startTimer( "System::Process Collisions" );
	for( shared_ptr< Object > object : objects )
		object->onProcessCollisions();
	Debug::stopTimer( "System::Process Collisions" );
	Debug::startTimer( "System::Post Update" );
	for( shared_ptr< Object > object : objects )
		object->onPostUpdate( time );
	Debug::stopTimer( "System::Post Update" );

	Debug::startTimer( "System::Cleanup" );
	// Cleanup
	Object::cleanupObjects();
	Debug::stopTimer( "System::Cleanup" );

	// Render
	window.clear( world->getBackgroundColor().sf() );

	Debug::startTimer( "System::Render" );
	for( shared_ptr< Object > object : objects )
		object->onRender();
	Debug::stopTimer( "System::Render" );
	Debug::startTimer( "System::Post Render" );
	for( shared_ptr< Object > object : objects )
		object->onPostRender();
	Debug::stopTimer( "System::Post Render" );

	Debug::startTimer( "System::Display" );
	window.display();
	Debug::stopTimer( "System::Display" );
}

//================================================================================

} // System