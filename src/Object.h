//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "Collision.h"

//================================================================================

class Object : public std::enable_shared_from_this< Object >
{
public:
	Object() : m_name( "" ), m_parent( nullptr ) {};
	Object( string name ) : m_name( name ), m_parent( nullptr ) {};

// Events
public:
	virtual inline void onStart() {}
	virtual inline void onCreateObservers() {}
	virtual inline void onSpawnChildren() {}
	virtual inline void onUpdate( double deltaTime ) {}
	virtual inline void onProcessCollisions( double deltaTime ) {}
	virtual inline void onPostUpdate( double deltaTime ) {}
	virtual inline void onRender() {}
	virtual inline void onPostRender() {}
	virtual inline void onExit() {}
	virtual inline void onDestroy() {}

// Collision
public:
	virtual inline Collision::CollisionResult isColliding( shared_ptr< Object > target ) { return Collision::CollisionResult(); } // The collision calculation function for this object
	virtual inline void onCollision( Collision::CollisionResult collision, shared_ptr< Object > target ) {}  // Called when the object collides with another
	virtual inline void resolveCollision( shared_ptr< Object > target ) {} // Resolves the collision by moving the dynamic object towards the collision normal
	virtual inline void resolveCollision( Collision::CollisionResult collision ) {}
	
	void processCollisions( vector< shared_ptr< Object > > targets ); // Calls onCollision for every colliding target
	void resolveCollisions( vector< shared_ptr< Object > > targets, bool notify = false ); // Resolves all collisions with the targets, in order of distance. If Notify, calls onCollision when a collision is resolved.

// Get/Set
public:
	inline Object* getParent() const { return m_parent; }
	inline void setParent( Object* parent ) { m_parent = parent; }

	inline string getName() const { return m_name; }
	inline void setName( string name ) { m_name = name; }

	virtual inline Vec2 getPosition() const { return Vec2(); }
	virtual inline void setPosition( Vec2 position ) {}

// Variables
protected:
	// System
	Object* m_parent;
	string m_name;


/* Static */

// Functions
public:
	// Create an object and store it in the global objects array for event processing
	template< class T >
	inline static shared_ptr< T > makeObject( Object* parent )
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
				out.push_back( ptr );
		}

		return out;
	}

	//--------------------------------------------------------------------------------

	inline static vector< shared_ptr< Object > > getObjectsByName( string name )
	{
		vector< shared_ptr< Object > > out;

		for( const shared_ptr< Object > object : s_objects )
			if( object->getName() == name )
				out.push_back( object );

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

// Variables
private:
	static vector< shared_ptr< Object > > s_objects;
	static vector< shared_ptr< Object > > s_markedForDeletion;
};

//================================================================================
