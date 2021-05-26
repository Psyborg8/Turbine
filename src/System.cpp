//================================================================================

#include "System.h"

//--------------------------------------------------------------------------------

// Systems
#include "Random.h"
#include "Timer.h"

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

sf::RenderWindow window{ sf::VideoMode( systemInfo.width, systemInfo.height ), "Project Bullet" };
sf::Clock clock;

//================================================================================

// PUBLIC

//================================================================================

bool init( int argc, char** argv ) {

	// Init timers
	Timers::init();

	return true;
}

//--------------------------------------------------------------------------------

int start() {
	world = Object::makeObject< Worlds::SFMLWorld >( nullptr );
	// world = Object::makeObject< CollisionWorld >( nullptr );

	world->loadWorld( "Dash Precision World" );
	// world->loadWorld( "Juggle Time Tower" );
	
	vector< shared_ptr< Object > > objects = Object::getObjects( getWorld(), "", true, true );
	
	for( shared_ptr< Object > object : objects )
		object->onStart();

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
	if( time.asSeconds() == 0.05f )
		time = sf::Time( sf::seconds( 0.05f ) );

	deltaTime = time;

	// Update timers
	Timers::update();

	// Update the current world
	vector< shared_ptr< Object > > objects = Object::getObjects( getWorld(), "", true, true );

	// Event handling
	sf::Event e;
	while( window.pollEvent( e ) ) {
		for( shared_ptr< Object > object : objects )
			object->onEvent( e );
	}

	// Update physics
	for( shared_ptr< Object > object : objects )
		object->onUpdate( time );
	for( shared_ptr< Object > object : objects )
		object->onProcessCollisions();
	for( shared_ptr< Object > object : objects )
		object->onPostUpdate( time );

	// Cleanup
	Object::cleanupObjects();

	// Render
	window.clear( world->getBackgroundColor().sf() );

	for( shared_ptr< Object > object : objects )
		object->onRender();
	for( shared_ptr< Object > object : objects )
		object->onPostRender();

	window.display();
}

//================================================================================

} // System