//================================================================================

#include "System.h"

//--------------------------------------------------------------------------------

// Systems
#include "Random.h"
#include "Observers.h"
#include "Timer.h"

// Worlds
#include "GridWorld.h"

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

shared_ptr< World > world;

std::unordered_map< KeyCode, bool > keyMap;

//--------------------------------------------------------------------------------

SystemInfo systemInfo;

chronoClockPoint lastFrameTime;
chronoClockPoint lastRenderTime;
chronoClockPoint startTime;

unique_ptr< Random > randomGen;

bool shouldExit{ false };

//================================================================================

// PUBLIC

//================================================================================

bool init( int argc, char** argv )
{
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
	{
		return false;
	}

	// Flush errors
	while( GL_NO_ERROR != glGetError() );

	// Set clear color
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	// Init timers
	Timer::init();

	// Init observers
	Observer::initObservers();

	// Add System observers
	Observer::addObserver( ObserverType::KeyPress, KeyCallback( onKeyPress ) );
	Observer::addObserver( ObserverType::KeyRelease, KeyCallback( onKeyRelease ) );
	Observer::addObserver( ObserverType::WindowResize, Vec2Callback( onWindowResize ) );
	Observer::addObserver( ObserverType::MouseMove, AxisCallback( onMouseMove ) );

	// Init random number generator
	startTime = high_resolution_clock::now();
	long long seed = startTime.time_since_epoch().count();
	randomGen = make_unique< Random >( static_cast< unsigned int >( seed ) );

	return true;
}

//--------------------------------------------------------------------------------

int start()
{
	world = Object::makeObject< GridWorld >( "World", nullptr );
	
	world->onStart();
	vector< shared_ptr< Object > > objects = Object::getObjectsByParent( getWorld(), true );
	for( shared_ptr< Object > object : objects )
	{
		object->onStart();
	}

	lastFrameTime = high_resolution_clock::now();
	lastRenderTime = high_resolution_clock::now();

	glutMainLoop();

	return 0;
}

//--------------------------------------------------------------------------------

void exit()
{
	shouldExit = true;
}

//--------------------------------------------------------------------------------

SystemInfo getSystemInfo()
{
	return systemInfo;
}

//--------------------------------------------------------------------------------

shared_ptr< World > getWorld()
{
	return world;
}

//--------------------------------------------------------------------------------

bool getKeyState( KeyCode key )
{
	keyMap.try_emplace( key, false ); // Default to false if the value doesn't exist yet.
	return keyMap[ key ];
}

//================================================================================

// LOCAL

//================================================================================

void update()
{
	// Get delta time
	chronoClockPoint now = high_resolution_clock::now();
	milliseconds msDeltaTime = duration_cast< milliseconds >( now - lastFrameTime );
	double deltaTime = msDeltaTime.count() / 1000.0;

	// Limit the deltaTime if the frame took too long.
	// So we can stop the game during breakpoints.
	deltaTime = min( deltaTime, 0.05 );

	// Do physics and collision before limiting the framerate for accuracy
	const vector< shared_ptr< Object > > objects = Object::getObjectsByParent( getWorld(), true );

	world->onUpdate( deltaTime );
	for( shared_ptr< Object > object : objects )
	{
		object->onUpdate( deltaTime );
	}

	// Update timers
	Timer::update();

	// Check for collisions
	Object::checkCollisions( objects );

	// Store new frame time
	lastFrameTime = now;

	glClearColor( 0.05f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	// Render images
	world->onRender();
	for( shared_ptr< Object > object : objects )
	{
		object->onRender();
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void onKeyPress( int key )
{
	const KeyCode keyCode = static_cast< KeyCode >( key );
	keyMap[ keyCode ] = true;

	if( keyCode == KeyCode::Escape )
	{
		exit();
	}
}

//--------------------------------------------------------------------------------

void onKeyRelease( int key )
{
	const KeyCode keyCode = static_cast< KeyCode >( key );
	keyMap[ keyCode ] = false;
}

//--------------------------------------------------------------------------------

void onWindowResize( int width, int height )
{
	systemInfo.height = height;
	systemInfo.width = width;

	glViewport( 0, 0, width, height );

	world->getCamera()->calculate();
}

//--------------------------------------------------------------------------------

void onMouseMove( float x, float y )
{
	std::cout << x << " " << y << std::endl;
}

//================================================================================

} // System