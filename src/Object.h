//================================================================================

#pragma once

//================================================================================

#include "Observers.h"

//================================================================================

class Object
{
public:
	Object();
	Object( string name );

// System
public:
	void createObservers();

// Events
public:
	// System
	virtual inline void onSpawnChildren() {} // For spawning all child objects
	virtual inline void onStart() {} // For setting properties of object and children
	virtual inline void onUpdate( float deltaTime ) {} // Called first each frame
	virtual inline void onRender() {} // Called last each frame
	virtual inline void onDestroy(); // Called when the object is destroyed
	virtual inline void onExit() {} // Called when the game closes

	// Input
	virtual inline void onMouseMove( float x, float y ) {}
	virtual inline void onMousePress( int button ) {}
	virtual inline void onMouseRelease( int button ) {}

	virtual inline void onKeyboardPress( int key ) {}
	virtual inline void onKeyboardRelease( int key ) {}

// Get/Set
public:
	inline Object* getParent() { return m_parent; }
	inline void setParent( Object* parent ) { m_parent = parent; }

	inline string getName() { return m_name; }
	inline void setName( string name ) { m_name = name; }

// Variables
protected:
	// System
	Object* m_parent;
	string m_name;

	// Input
	ObserverID m_mouseMoveObserver;
	ObserverID m_mousePressObserver;
	ObserverID m_mouseReleaseObserver;

	ObserverID m_keyboardPressObserver;
	ObserverID m_keyboardReleaseObserver;


/* Static */

// Functions
public:
	// Create an object and store it in the global objects array for event processing
	template< class T >
	static shared_ptr< T > makeObject( string name, Object* parent )
	{
		shared_ptr< T > ptr = std::make_shared< T >( T() );

		shared_ptr< Object > ptrObj = std::dynamic_pointer_cast< Object >( ptr );
		if( ptrObj != nullptr )
		{
			ptrObj->setParent( parent );
			ptrObj->createObservers();
			ptrObj->onSpawnChildren();

			s_objects.push_back( ptrObj );
		}

		return ptr;
	}

	template< class T >
	static shared_ptr< T > makeObject( const T& object, Object* parent )
	{
		shared_ptr< T > ptr = make_shared< T >( object );

		shared_ptr< Object > ptrObj = std::dynamic_pointer_cast< Object >( ptr );
		if( ptrObj != nullptr )
		{
			ptrObj->setParent( parent );
			ptrObj->createObservers();
			ptrObj->onSpawnChildren();

			s_objects.push_back( ptrObj );
		}

		return ptr;
	}

	// Mark an objects to be destroyed
	static void destroyObject( shared_ptr< Object > object )
	{
		s_markedForDeletion.push_back( object );
	}

	// Clean up deleted objects
	static void cleanupObjects()
	{
		for( shared_ptr< Object > marked : s_markedForDeletion )
		{
			vector< shared_ptr< Object > >::iterator it = find( s_objects.begin(), s_objects.end(), marked );
			if( it != s_objects.end() )
			{
				(*it)->onDestroy();
				s_objects.erase( it );
			}
		}

		s_markedForDeletion = vector< shared_ptr< Object > >();
	}

	// Return all objects made using Object::makeObject
	static vector< shared_ptr< Object > > getObjects() { return s_objects; }

	// Dynamic casts to template type and returns all that aren't nullptr
	template< class T >
	inline static vector< shared_ptr< T > > getObjectsByType()
	{
		vector< shared_ptr< T > > out;

		for( const shared_ptr< Object > object : s_objects )
		{
			shared_ptr< T > ptr = std::dynamic_pointer_cast< T >( object );
			if( ptr != nullptr )
			{
				out.push_back( ptr );
			}
		}

		return out;
	}

	// Returns all objects with given parent
	static const vector< shared_ptr< Object > >& getObjectByParent( shared_ptr< Object > parent )
	{
		vector< shared_ptr< Object > > out;
		for( shared_ptr< Object > object : s_objects )
		{
			if( object->getParent() == parent.get() )
			{
				out.push_back( object );
			}
		}

		return out;
	}

// Variables
private:
	static vector< shared_ptr< Object > > s_objects;
	static vector< shared_ptr< Object > > s_markedForDeletion;
};

//================================================================================
