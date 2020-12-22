#include "Observers.h"

#include "System.h"

//================================================================================

namespace Observer
{

//================================================================================

struct BasicObserver : public ObserverID
{
	Callback callback;
};

//--------------------------------------------------------------------------------

struct KeyObserver : public ObserverID
{
	KeyCallback callback;
};

//--------------------------------------------------------------------------------

struct AxisObserver : public ObserverID
{
	AxisCallback callback;
};

//--------------------------------------------------------------------------------

using ObserverVariant = std::variant< BasicObserver, KeyObserver, AxisObserver >;

//================================================================================

void cleanupObservers();
vector< ObserverVariant >::iterator findObserver( const ObserverID& ID );
bool isMarkedForRemoval( const ObserverVariant& observer );

//--------------------------------------------------------------------------------

void keyPressCallback( unsigned char key, int x, int y );
void keyReleaseCallback( unsigned char key, int x, int y );
void mouseCallback( int button, int state, int x, int y );
void mouseMoveCallback( int x, int y );

//--------------------------------------------------------------------------------

unordered_map< ObserverType, vector< ObserverVariant > > observers;
vector< ObserverID > markedForRemoval;

//--------------------------------------------------------------------------------

stack< int > freeIDs;
int highestID{ 0 };

//================================================================================

// PUBLIC

//================================================================================

void initObservers()
{
	glutKeyboardFunc( keyPressCallback );
	glutKeyboardUpFunc( keyReleaseCallback );
	glutMouseFunc( mouseCallback );
	glutMotionFunc( mouseMoveCallback );
	glutPassiveMotionFunc( mouseMoveCallback );
}

//--------------------------------------------------------------------------------

ObserverID addObserver( ObserverType type, Callback callback )
{
	if( type == ObserverType::None )
	{
		return ObserverID();
	}

	if( callback == nullptr )
	{
		return ObserverID();
	}

	int ID = -1;

	if( freeIDs.empty() )
	{
		ID = highestID++;
	}
	else
	{
		ID = freeIDs.top();
		freeIDs.pop();
	}

	BasicObserver observer;
	observer.ID = ID;
	observer.timeStamp = high_resolution_clock::now();
	observer.callback = callback;
	observer.type = type;

	observers[ type ].push_back( observer );

	return ObserverID( observer );
}

//--------------------------------------------------------------------------------

ObserverID addObserver( ObserverType type, KeyCallback callback )
{
	if( type == ObserverType::None )
	{
		return ObserverID();
	}

	if( callback == nullptr )
	{
		return ObserverID();
	}

	int ID = -1;

	if( freeIDs.empty() )
	{
		ID = highestID++;
	}
	else
	{
		ID = freeIDs.top();
		freeIDs.pop();
	}

	KeyObserver observer;
	observer.ID = ID;
	observer.timeStamp = high_resolution_clock::now();
	observer.callback = callback;
	observer.type = type;

	observers[ type ].push_back( observer );

	// User doesn't need the callback
	return ObserverID( observer );
}

//--------------------------------------------------------------------------------

ObserverID addObserver( ObserverType type, AxisCallback callback )
{
	if( type == ObserverType::None )
	{
		return ObserverID();
	}

	if( callback == nullptr )
	{
		return ObserverID();
	}

	int ID = -1;

	if( freeIDs.empty() )
	{
		ID = highestID++;
	}
	else
	{
		ID = freeIDs.top();
		freeIDs.pop();
	}

	AxisObserver observer;
	observer.ID = ID;
	observer.timeStamp = high_resolution_clock::now();
	observer.callback = callback;
	observer.type = type;

	observers[ type ].push_back( observer );

	return ObserverID( observer );
}

//--------------------------------------------------------------------------------

void removeObserver( ObserverID ID )
{
	markedForRemoval.push_back( ID );
}

//================================================================================

// LOCAL

//================================================================================

void cleanupObservers()
{
	for( const ObserverID marked : markedForRemoval )
	{
		const auto it = findObserver( marked );
		if( it != observers[ marked.type ].end() )
		{
			observers[ marked.type ].erase( it );
		}
	}

	markedForRemoval = vector< ObserverID >();
}

//--------------------------------------------------------------------------------

vector< ObserverVariant >::iterator findObserver( const ObserverID& ID )
{
	const auto func = [ ID ]( const auto& var ) {
		return var.ID == ID.ID && var.timeStamp == ID.timeStamp;
	};

	const auto it = std::find_if( observers[ ID.type ].begin(), observers[ ID.type ].end(), [ func ]( const ObserverVariant& var ) {
		return std::visit( func, var );
	} );

	return it;
}

//--------------------------------------------------------------------------------

bool isMarkedForRemoval( const ObserverVariant& observer )
{
	for( ObserverID marked : markedForRemoval )
	{
		const auto func = [ marked ]( const auto& observer ) {
			return observer.ID == marked.ID && observer.timeStamp == marked.timeStamp;
		};

		if( std::visit( func, observer ) )
		{
			return true;
		}
	}

	return false;
}

//================================================================================

// LOCAL - Callbacks

//================================================================================

void keyPressCallback( unsigned char key, int x, int y )
{
	for( ObserverVariant observer : observers[ ObserverType::KeyPress ] )
	{
		if( isMarkedForRemoval( observer ) )
		{
			continue;
		}

		KeyObserver* keyObserver = std::get_if< KeyObserver >( &observer );
		if( keyObserver != nullptr )
		{
			keyObserver->callback( key );
		}
	}
}

//--------------------------------------------------------------------------------

void keyReleaseCallback( unsigned char key, int x, int y )
{
	for( ObserverVariant observer : observers[ ObserverType::KeyRelease ] )
	{
		if( isMarkedForRemoval( observer ) )
		{
			continue;
		}

		KeyObserver* keyObserver = std::get_if< KeyObserver >( &observer );
		if( keyObserver != nullptr )
		{
			keyObserver->callback( key );
		}
	}
}

//--------------------------------------------------------------------------------

void mouseCallback( int button, int state, int x, int y )
{
	ObserverType type = state == GLUT_DOWN ? ObserverType::MousePress : ObserverType::MouseRelease;

	for( ObserverVariant observer : observers[ type ] )
	{
		if( isMarkedForRemoval( observer ) )
		{
			continue;
		}

		KeyObserver* keyObserver = std::get_if< KeyObserver >( &observer );
		if( keyObserver != nullptr )
		{
			keyObserver->callback( button );
		}
	}
}

//--------------------------------------------------------------------------------

void mouseMoveCallback( int x, int y )
{
	for( ObserverVariant observer : observers[ ObserverType::MouseMove ] )
	{
		if( isMarkedForRemoval( observer ) )
		{
			continue;
		}

		System::SystemInfo systemInfo = System::getSystemInfo();
		float outX, outY;
		outX = float( x ) / float( systemInfo.width );
		outX = outX * 2.0f - 1.0f;
		outY = float( y ) / float( systemInfo.height );
		outY = 1.0f - outY;
		outY = outY * 2.0f - 1.0f;

		AxisObserver* axisObserver = std::get_if< AxisObserver >( &observer );
		if( axisObserver != nullptr )
		{
			axisObserver->callback( outX, outY );
		}
	}
}

//================================================================================

} // Observers