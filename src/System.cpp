//================================================================================

#include "System.h"

//--------------------------------------------------------------------------------

#include "Quad.h"
#include "Player.h"
#include "Random.h"
#include "Observers.h"
#include "DevWorld.h"

//================================================================================

namespace System
{

//================================================================================

void onKeyPress( int key );
void onWindowResize( int width, int height );

void update();

//--------------------------------------------------------------------------------

shared_ptr< World > world;

//--------------------------------------------------------------------------------

SystemInfo systemInfo;

chronoClockPoint lastFrameTime;
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
	glutReshapeFunc( onWindowResize );

	// Init glew
	if( GLEW_OK != glewInit() )
	{
		return false;
	}

	// Flush errors
	while( GL_NO_ERROR != glGetError() );

	// Set clear color
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	// Init observers
	Observer::initObservers();

	// Add System observers
	Observer::addObserver( ObserverType::KeyPress, KeyCallback( onKeyPress ) );

	// Init random number generator
	startTime = high_resolution_clock::now();
	long long seed = startTime.time_since_epoch().count();
	randomGen = make_unique< Random >( static_cast< unsigned int >( seed ) );

	return true;
}

//--------------------------------------------------------------------------------

int start()
{
	world = Object::makeObject< DevWorld >( "DevWorld", nullptr );

	lastFrameTime = high_resolution_clock::now();

	vector< shared_ptr< Object > > objects = Object::getObjects();
	for( shared_ptr< Object > object : objects )
	{
		object->onStart();
	}

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

//================================================================================

// LOCAL

//================================================================================

void update()
{
	// Get delta time
	chronoClockPoint now = high_resolution_clock::now();
	milliseconds msDeltaTime = duration_cast< milliseconds >( now - lastFrameTime );
	float deltaTime = msDeltaTime.count() / 1000.0f;

	if( deltaTime > 1.0 )
	{
		deltaTime = 1.0f / 60.0f;
	}

	// Apply frame limiter if there is one
	if( systemInfo.fpsLimit > 0u )
	{
		milliseconds frameTime = milliseconds( 1000 / systemInfo.fpsLimit );

		if( msDeltaTime < frameTime )
		{
			return;
		}
	}

	lastFrameTime = now;

	const vector< shared_ptr< Object > > objects = Object::getObjects();

	// Update objects
	for( shared_ptr< Object > object : objects )
	{
		object->onUpdate( deltaTime );
	}

	glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	for( shared_ptr< Object > object : objects )
	{
		object->onRender();
	}

	glutSwapBuffers();

	glutPostRedisplay();
}

void onKeyPress( int key )
{
	if( key == 27 ) // ESC
	{
		exit();
	}
}

//--------------------------------------------------------------------------------

void onWindowResize( int width, int height )
{
	systemInfo.height = height;
	systemInfo.width = width;

	glViewport( 0, 0, width, height );

	world->getCamera()->calculate();
}

//================================================================================

} // System