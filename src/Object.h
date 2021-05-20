//================================================================================

#pragma once

//================================================================================

#include "Observers.h"
#include "MathTypes.h"

//================================================================================

enum class CollisionType
{
	None,
	Notify,
	StaticBlocking,
	DynamicBlocking
};

//================================================================================

class Object
{
public:
	Object() : m_name( "" ), m_parent( nullptr ) {};
	Object( string name ) : m_name( name ), m_parent( nullptr ) {};

	// Events
public:
	// System
	virtual inline void onSpawnChildren() {} // For spawning all child objects
	virtual inline void onCreateObservers() {} // For binding observers
	virtual inline void onStart() {} // For setting properties of object and children
	virtual inline void onUpdate( double deltaTime ) {} // Called first each frame
	virtual inline void onRender() {} // Called last each frame
	virtual inline void onDestroy() {} // Called when the object is destroyed
	virtual inline void onExit() {} // Called when the game closes

	// Collision
	virtual inline bool isColliding( shared_ptr< Object > target, Vec2& normal ) { return false; } // The collision calculation function for this object
	virtual inline void onCollision( shared_ptr< Object > object, Vec2 normal ) {}  // Called when the object collides with another

// Get/Set
public:
	inline Object* getParent() { return m_parent; }
	inline void setParent( Object* parent ) { m_parent = parent; }

	inline string getName() { return m_name; }
	inline void setName( string name ) { m_name = name; }

	inline CollisionType getCollisionType() { return m_collisionType; }
	inline void setCollisionType( CollisionType type ) { m_collisionType = type; }

// Variables
protected:
	// System
	Object* m_parent;
	string m_name;
	CollisionType m_collisionType;


/* Static */

// Functions
public:
	// Create an object and store it in the global objects array for event processing
	template< class T >
	inline static shared_ptr< T > makeObject( string name, Object* parent )
	{
		shared_ptr< T > ptr = std::make_shared< T >( T() );

		shared_ptr< Object > ptrObj = std::dynamic_pointer_cast< Object >( ptr );
		if( ptrObj != nullptr )
		{
			ptrObj->setParent( parent );
			ptrObj->onCreateObservers();
			ptrObj->onSpawnChildren();

			s_objects.push_back( ptrObj );
		}

		return ptr;
	}

	//--------------------------------------------------------------------------------

	template< class T >
	inline static shared_ptr< T > makeObject( const T& object, Object* parent )
	{
		shared_ptr< T > ptr = make_shared< T >( object );

		shared_ptr< Object > ptrObj = std::dynamic_pointer_cast< Object >( ptr );
		if( ptrObj != nullptr )
		{
			ptrObj->setParent( parent );
			ptrObj->onCreateObservers();
			ptrObj->onSpawnChildren();

			s_objects.push_back( ptrObj );
		}

		return ptr;
	}

	//--------------------------------------------------------------------------------

	// Mark an objects to be destroyed
	inline static void destroyObject( shared_ptr< Object > object )
	{
		s_markedForDeletion.push_back( object );
	}

	//--------------------------------------------------------------------------------

	// Clean up deleted objects
	inline static void cleanupObjects()
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

	//--------------------------------------------------------------------------------

	// Return all objects made using Object::makeObject
	inline static vector< shared_ptr< Object > > getObjects() { return s_objects; }

	//--------------------------------------------------------------------------------

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

	//--------------------------------------------------------------------------------

	// Returns all objects with given parent
	inline static vector< shared_ptr< Object > > getObjectsByParent( shared_ptr< Object > parent, bool recursive = false )
	{
		vector< shared_ptr< Object > > out;
		for( shared_ptr< Object > object : s_objects )
		{
			if( object->getParent() == parent.get() )
			{
				out.push_back( object );
			}
			else if( recursive )
			{
				Object* temp = object->getParent();
				while( temp != nullptr )
				{
					temp = temp->getParent();
					if( temp == parent.get() )
					{
						out.push_back( object );
						break;
					}
				}
			}
		}

		return out;
	}

	//--------------------------------------------------------------------------------

	static void checkCollisions( vector< shared_ptr< Object > > objects );

// Variables
private:
	static vector< shared_ptr< Object > > s_objects;
	static vector< shared_ptr< Object > > s_markedForDeletion;
};

//================================================================================
