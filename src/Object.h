//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "Collision.h"

//================================================================================

class Object : public std::enable_shared_from_this< Object > {
public:
	Object() : m_name( "" ), m_parent( nullptr ) {};
	Object( string name ) : m_name( name ), m_parent( nullptr ) {};

	// System
public:
	inline void destroy() { 
		destroyObject( shared_from_this() ); 
		vector< shared_ptr< Object > > objects = getObjects( shared_from_this(), "" );
		for( shared_ptr< Object > object : objects )
			object->destroy();
	}

	// Events
public:
	virtual inline void onEvent( sf::Event e ) {} // Called for SFML events
	virtual inline void onSpawnChildren() {} // Make all child objects for this object
	virtual inline void onStart() {} // Called after all objects are spawned
	virtual inline void onUpdate( sf::Time deltaTime ) {} // First update phase - For input, physics etc.
	virtual inline void onProcessCollisions() {} // Second update phase - For collision processing
	virtual inline void onCollision( Collision::CollisionResult collision, shared_ptr< Object > target ) {}  // Called when the object collides with another
	virtual inline void onPostUpdate( sf::Time deltaTime ) {} // Third update phase - For resolving physics and cleaning up
	virtual inline void onRender() {} // Called after all update phases, for rendering gfx
	virtual inline void onPostRender() {} // For things that need to be done after all renders are complete
	virtual inline void onExit() {} // When the game exits
	virtual inline void onMessage( string message ) {} // When the object is sent a generic event message
	virtual void onDestroy() {} // When the object is marked for deletion

// Collision
public:
	virtual inline Collision::CollisionResult isColliding( shared_ptr< Object > target ) { return Collision::CollisionResult(); } // The collision calculation function for this object
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

	virtual inline Math::Vec2 getPosition() const { return Math::Vec2(); }
	virtual inline void setPosition( Math::Vec2 position ) { m_position = position; }

	inline CollisionType getCollisionType() const { return m_collisionType; }
	inline void setCollisionType( CollisionType type ) { m_collisionType = type; }

	inline bool getVisibility() const { return m_visibility; }
	inline void setVisibility( bool visible ) { m_visibility = visible; }

	// Variables
protected:
	Object* m_parent{ nullptr };
	string m_name{ "" };
	CollisionType m_collisionType{ CollisionType::None };
	Math::Vec2 m_position;
	bool m_visibility{ true };


	/* Static */

	// Functions
public:
	// Create an object and store it in the global objects array for event processing
	template< class T >
	inline static shared_ptr< T > makeObject( Object* parent ) {
		shared_ptr< T > ptr = std::make_shared< T >( T() );

		shared_ptr< Object > ptrObj = std::dynamic_pointer_cast< Object >( ptr );
		if( ptrObj != nullptr ) {
			ptrObj->setParent( parent );
			ptrObj->onSpawnChildren();

			s_objects.push_back( ptrObj );
		}

		return ptr;
	}

	//--------------------------------------------------------------------------------

	// Mark an objects to be destroyed
	inline static void destroyObject( shared_ptr< Object > object ) {
		s_markedForDeletion.push_back( object );
	}

	//--------------------------------------------------------------------------------

	// Clean up deleted objects
	inline static void cleanupObjects() {
		for( shared_ptr< Object > marked : s_markedForDeletion ) {
			vector< shared_ptr< Object > >::iterator it = find( s_objects.begin(), s_objects.end(), marked );
			if( it != s_objects.end() ) {
				( *it )->onDestroy();
				s_objects.erase( it );
			}
		}

		s_markedForDeletion = vector< shared_ptr< Object > >();
	}

	//--------------------------------------------------------------------------------

	// Return all objects made using Object::makeObject
	inline static vector< shared_ptr< Object > > getObjects( shared_ptr< Object > parent = nullptr,
												  string name = "",
												  bool inclusive = false )
	{
		// If there's no name or parent, we just want everything
		if( parent == nullptr && name == "" )
			return s_objects;

		vector< shared_ptr< Object > > out;

		for( const shared_ptr< Object > object : s_objects ) {
			// Check name first cause it's the fastest
			if( name != "" )
				if( object->getName() != name )
					continue;

			// Check the parent
			if( parent != nullptr ) {
				if( object->getParent() != parent.get() )
					continue;
			}

			out.push_back( object );
		}

		// Add the parent if we need it
		if( inclusive )
			out.insert( out.begin(), parent );

		return out;
	}

	//--------------------------------------------------------------------------------

	// Dynamic casts to template type and returns all that aren't nullptr
	template< class T >
	inline static vector< shared_ptr< T > > getObjects( shared_ptr< Object > parent = nullptr,
														string name = "",
														bool inclusive = false )
	{
		vector< shared_ptr< T > > out;

		for( const shared_ptr< Object > object : s_objects ) {
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

		if( inclusive ) {
			shared_ptr< T > ptr = std::dynamic_pointer_cast< T >( parent );
			if( ptr != nullptr )
				out.insert( out.begin(), ptr );
		}

		return out;
	}

	// Variables
private:
	static vector< shared_ptr< Object > > s_objects;
	static vector< shared_ptr< Object > > s_markedForDeletion;
};

//================================================================================
