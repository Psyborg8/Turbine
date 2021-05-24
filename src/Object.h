//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "Collision.h"

//================================================================================

using ObjectPtr = shared_ptr< Object >;

//================================================================================

class Object : public std::enable_shared_from_this< Object >
{
public:
	Object() : m_name( "" ), m_parent( nullptr ) {};
	Object( string name ) : m_name( name ), m_parent( nullptr ) {};

// System
	inline void destroy() { destroyObject( shared_from_this() ); }

// Events
public:
	virtual inline void onCreateObservers() {} // Create all observers for object
	virtual inline void onSpawnChildren() {} // Make all child objects for this object
	virtual inline void onStart() {} // Called after all objects are spawned
	virtual inline void onUpdate( double deltaTime ) {} // First update phase - For input, physics etc.
	virtual inline void onProcessCollisions() {} // Second update phase - For collision processing
	virtual inline void onCollision( Collision::CollisionResult collision, ObjectPtr target ) {}  // Called when the object collides with another
	virtual inline void onPostUpdate( double deltaTime ) {} // Third update phase - For resolving physics and cleaning up
	virtual inline void onRender() {} // Called after all update phases, for rendering gfx
	virtual inline void onPostRender() {} // For things that need to be done after all renders are complete
	virtual inline void onExit() {} // When the game exits
	virtual void onDestroy(); // When the object is marked for deletion

// Collision
public:
	virtual inline Collision::CollisionResult isColliding( ObjectPtr target ) { return Collision::CollisionResult(); } // The collision calculation function for this object
	virtual inline void resolveCollision( ObjectPtr target ) {} // Resolves the collision by moving the dynamic object towards the collision normal
	virtual inline void resolveCollision( Collision::CollisionResult collision ) {}
	
	void processCollisions( vector< ObjectPtr > targets ); // Calls onCollision for every colliding target
	void resolveCollisions( vector< ObjectPtr > targets, bool notify = false ); // Resolves all collisions with the targets, in order of distance. If Notify, calls onCollision when a collision is resolved.

// Get/Set
public:
	inline Object* getParent() const { return m_parent; }
	inline void setParent( Object* parent ) { m_parent = parent; }

	inline string getName() const { return m_name; }
	inline void setName( string name ) { m_name = name; }

	virtual inline Vec2 getPosition() const { return Vec2(); }
	virtual inline void setPosition( Vec2 position ) {}

	inline CollisionType getCollisionType() const { return m_collisionType; }
	inline void setCollisionType( CollisionType type ) { m_collisionType = type; }

// Variables
protected:
	Object* m_parent{ nullptr };
	string m_name{ "" };
	CollisionType m_collisionType{ CollisionType::None };


/* Static */

// Functions
public:
	// Create an object and store it in the global objects array for event processing
	template< class T >
	inline static shared_ptr< T > makeObject( Object* parent )
	{
		shared_ptr< T > ptr = std::make_shared< T >( T() );

		ObjectPtr ptrObj = std::dynamic_pointer_cast< Object >( ptr );
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
	inline static void destroyObject( ObjectPtr object )
	{
		s_markedForDeletion.push_back( object );
	}

	//--------------------------------------------------------------------------------

	// Clean up deleted objects
	inline static void cleanupObjects()
	{
		for( ObjectPtr marked : s_markedForDeletion )
		{
			vector< ObjectPtr >::iterator it = find( s_objects.begin(), s_objects.end(), marked );
			if( it != s_objects.end() )
			{
				(*it)->onDestroy();
				s_objects.erase( it );
			}
		}

		s_markedForDeletion = vector< ObjectPtr >();
	}

	//--------------------------------------------------------------------------------

	// Return all objects made using Object::makeObject
	inline static vector< ObjectPtr > getObjects( ObjectPtr parent = nullptr, 
															string name = "", 
															bool recursive = false, 
															bool inclusive = false ) 
	{ 
		// If there's no name or parent, we just want everything
		if( parent == nullptr && name == "" )
			return s_objects;

		vector< ObjectPtr > out;

		for( const ObjectPtr object : s_objects )
		{
			// Check name first cause it's the fastest
			if( name != "" )
				if( object->getName() != name )
					continue;

			// Check the parent
			if( parent != nullptr )
			{
				if( object->getParent() != parent.get() )
					continue;
			}

			out.push_back( object );
		}

		// If the parent exists and is correct, look recursively for more
		if( recursive && parent != nullptr )
		{
			vector< ObjectPtr > temp = out;
			for( ObjectPtr object : temp )
			{
				vector< ObjectPtr > children = getObjects( object, name, true, false );
				out.insert( out.end(), children.begin(), children.end() );
			}
		}

		// Add the parent if we need it
		if( inclusive )
			out.insert( out.begin(), parent );

		return out;
	}

	//--------------------------------------------------------------------------------

	// Dynamic casts to template type and returns all that aren't nullptr
	template< class T >
	inline static vector< shared_ptr< T > > getObjects( ObjectPtr parent = nullptr,
															  string name = "",
															  bool recursive = false,
															  bool inclusive = false )
	{
		vector< shared_ptr< T > > out;

		for( const ObjectPtr object : s_objects )
		{
			// Check name first cause it's the fastest
			if( name != "" )
				if( object->getName() != name )
					continue;

			// Check that we can cast to the given type successfully
			shared_ptr< T > ptr = std::dynamic_pointer_cast< T >( object );
			if( ptr == nullptr )
				continue;

			// Check the parent
			if( parent != nullptr )
				if( object->getParent() != parent.get() )
					continue;

			out.push_back( ptr );
		}

		// If the parent exists and is correct, look recursively for more
		if( recursive && parent != nullptr )
		{
			vector< shared_ptr< T > > temp = out;
			for( shared_ptr< T > object : temp )
			{
				shared_ptr< Object > ptr = std::dynamic_pointer_cast< Object >( object );
				if( ptr == nullptr )
					continue;

				vector< shared_ptr< T > > children = getObjects< T >( ptr, name, true, false );
				out.insert( out.end(), children.begin(), children.end() );
			}
		}

		if( inclusive )
		{
			shared_ptr< T > ptr = std::dynamic_pointer_cast< T >( parent );
			if( ptr != nullptr )
				out.insert( out.begin(), ptr );
		}

		return out;
	}

// Variables
private:
	static vector< ObjectPtr > s_objects;
	static vector< ObjectPtr > s_markedForDeletion;
};

//================================================================================
