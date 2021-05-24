//================================================================================

#include "System.h"

//--------------------------------------------------------------------------------

// Systems
#include "Random.h"
#include "Observers.h"
#include "Timer.h"

// Worlds
#include "CollisionWorld.h"
#include "GridWorld.h"

// Objects
#include "Object.h"
#include "RigidRect.h"

//================================================================================

namespace System
{

//================================================================================

void onKeyPress( int key );
void onKeyRelease( int key );
void onWindowResize( int width, int height );
void onMouseMove( float x, float y );

void update();

//--------------------------------------------------------------------------------

shared_ptr< Worlds::World > world;

std::unordered_map< KeyCode, bool > keyMap;
Math::Vec2 mousePosition;
double deltaTime;

//--------------------------------------------------------------------------------

SystemInfo systemInfo;

chronoClockPoint lastFrameTime;
chronoClockPoint lastRenderTime;
chronoClockPoint startTime;

bool shouldExit{ false };

//================================================================================

// PUBLIC

//================================================================================

bool init( int argc, char** argv ) {
	// Init glut
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( systemInfo.width, systemInfo.height );
	glutInitWindowPosition( 100, 100 );
	glutCreateWindow( "Project Bullet" );

	// Set glut functions
	glutIdleFunc( update );
	glutDisplayFunc( update );

	// Init glew
	if( GLEW_OK != glewInit() )
		return false;

	// Flush errors
	while( GL_NO_ERROR != glGetError() );

	// Set clear color
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	// Init timers
	Timers::init();

	// Init observers
	Observers::initObservers();

	// Add System observers
	Observers::addObserver( Observers::ObserverType::KeyPress, KeyCallback( onKeyPress ) );
	Observers::addObserver( Observers::ObserverType::KeyRelease, KeyCallback( onKeyRelease ) );
	Observers::addObserver( Observers::ObserverType::WindowResize, Vec2Callback( onWindowResize ) );
	Observers::addObserver( Observers::ObserverType::MouseMove, AxisCallback( onMouseMove ) );

	// Init random number generator
	startTime = high_resolution_clock::now();
	long long seed = startTime.time_since_epoch().count();
	Random::setSeed( uint16_t( seed ) );

	return true;
}

//--------------------------------------------------------------------------------

int start() {
	// world = Object::makeObject< CollisionWorld >( nullptr );
	world = Game::Object::makeObject< Worlds::GridWorld >( nullptr );
	
	vector< shared_ptr< Game::Object > > objects = Game::Object::getObjects( getWorld(), "", true, true );
	
	for( shared_ptr< Game::Object > object : objects )
		object->onStart();

	lastFrameTime = high_resolution_clock::now();
	lastRenderTime = high_resolution_clock::now();

	glutMainLoop();

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

bool getKeyState( KeyCode key ) {
	keyMap.try_emplace( key, false ); // Default to false if the value doesn't exist yet.
	return keyMap[ key ];
}

//--------------------------------------------------------------------------------

Math::Vec2 getMousePosition() {
	return mousePosition;
}

//--------------------------------------------------------------------------------

double getDeltaTime() {
	return deltaTime;
}

//================================================================================

// LOCAL

//================================================================================

void update() {
	// Get delta time
	chronoClockPoint now = high_resolution_clock::now();
	milliseconds msDeltaTime = duration_cast< milliseconds >( now - lastFrameTime );
	double dt = msDeltaTime.count() / 1000.0;

	// Limit the deltaTime if the frame took too long.
	// So we can stop the game during breakpoints.
	dt = std::min( dt, 0.05 );

	// Store the delta time
	deltaTime = dt;

	// Update timers
	Timers::update();

	// Update the current world
	vector< shared_ptr< Game::Object > > objects = Game::Object::getObjects( getWorld(), "", true, true );

	for( shared_ptr< Game::Object > object : objects )
		object->onUpdate( dt );
	for( shared_ptr< Game::Object > object : objects )
		object->onProcessCollisions();
	for( shared_ptr< Game::Object > object : objects )
		object->onPostUpdate( dt );

	// Cleanup
	Game::Object::cleanupObjects();

	// Store new frame time
	lastFrameTime = now;

	// Render
	glClearColor( 0.05f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	for( shared_ptr< Game::Object > object : objects )
		object->onRender();
	for( shared_ptr< Game::Object > object : objects )
		object->onPostRender();

	glutSwapBuffers();
	glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void onKeyPress( int key ) {
	const KeyCode keyCode = static_cast< KeyCode >( key );
	keyMap[ keyCode ] = true;

	if( keyCode == KeyCode::Escape )
		exit();
}

//--------------------------------------------------------------------------------

void onKeyRelease( int key ) {
	const KeyCode keyCode = static_cast< KeyCode >( key );
	keyMap[ keyCode ] = false;
}

//--------------------------------------------------------------------------------

void onWindowResize( int width, int height ) {
	systemInfo.height = height;
	systemInfo.width = width;

	glViewport( 0, 0, width, height );

	world->getCamera()->calculate();
}

//--------------------------------------------------------------------------------

void onMouseMove( float x, float y ) {
	mousePosition = Math::Vec2( static_cast< double >( x ), static_cast< double >( y ) );
}

//================================================================================

} // System