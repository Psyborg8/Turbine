//================================================================================

#include "object.h"

//--------------------------------------------------------------------------------

#include "debug.h"
#include "world.h"

//================================================================================

void Object::event( sf::Event e ) {
	if( isMarkedForRemoval() )
		return;

	onEvent( e );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->event( e );
}

//--------------------------------------------------------------------------------

void Object::spawnChildren() {
	if( isMarkedForRemoval() )
		return;

	onSpawnChildren();

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->spawnChildren();
}

//--------------------------------------------------------------------------------

void Object::start() {
	if( isMarkedForRemoval() )
		return;

	onStart();
	
	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->start();
}

//--------------------------------------------------------------------------------

void Object::update( sf::Time dt ) {
	if( isMarkedForRemoval() )
		return;

	onUpdate( dt );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->update( dt );
}

//--------------------------------------------------------------------------------

void Object::processCollisions() {
	if( isMarkedForRemoval() )
		return;

	onProcessCollisions();

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->processCollisions();
}

//--------------------------------------------------------------------------------

void Object::postUpdate( sf::Time dt ) {
	if( isMarkedForRemoval() )
		return;

	onPostUpdate( dt );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->postUpdate( dt );
}

//--------------------------------------------------------------------------------

void Object::render( sf::RenderTarget* target ) {
	if( isMarkedForRemoval() || !m_visibility )
		return;

	onRender( target );

	vector< shared_ptr< Object > > children = m_children;
	std::sort( children.begin(), children.end(),
			   []( shared_ptr< Object > a, shared_ptr< Object > b ) {
				   return a->getPriority() < b->getPriority();
			   } );

	for( shared_ptr< Object > child : children )
		child->render( target );
}

//--------------------------------------------------------------------------------

void Object::postRender( sf::RenderTarget* target ) {
	if( isMarkedForRemoval() )
		return;

	onPostRender( target );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->postRender( target );
}

//--------------------------------------------------------------------------------

void Object::exit() {
	if( isMarkedForRemoval() )
		return;

	onExit();

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->exit();
}

//--------------------------------------------------------------------------------

void Object::message( string message ) {
	if( isMarkedForRemoval() )
		return;

	onMessage( message );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->message( message );
}

//--------------------------------------------------------------------------------

void Object::destroy() {
	if( isMarkedForRemoval() )
		return;

	s_markedForDeletion.push_back( shared_from_this() );
	onDestroy();

	const size_t size = m_children.size();
	for( size_t i = 0u ; i < size; ++i )
		m_children.at( i )->destroy();
}

//--------------------------------------------------------------------------------

void Object::processCollisions( vector< shared_ptr< Object > > targets ) {
	for( shared_ptr< Object > target : targets ) {
		const Collision::CollisionResult result = isColliding( target );
		if( result.success ) {
			onCollision( result, target );
			target->onCollision( result, shared_from_this() );
		}
	}
}

//--------------------------------------------------------------------------------

void Object::resolveCollisions( vector< shared_ptr< Object > > targets, bool notify ) {
	using collisionPair = pair< shared_ptr< Object >, Collision::CollisionResult >;

	// Broad Phase
	vector < collisionPair > results;
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = isColliding( target );
		if( result.success )
			results.push_back( make_pair( target, result ) );
	}

	// Sort by distance
	std::sort( results.begin(), results.end(),
			   []( const collisionPair& a, const collisionPair& b ) {
				   return a.second.distance < b.second.distance;
			   } );

	// Narrow Phase
	for( pair< shared_ptr< Object >, Collision::CollisionResult > collision : results ) {
		// Check again, in case a previous resolution means we aren't colliding anymore
		const Collision::CollisionResult result = isColliding( collision.first );
		if( result.success ) {
			// Resolve the collision
			Collision::resolveCollision( result );

			if( notify ) {
				// Notify both targets
				onCollision( result, collision.first );
				collision.first->onCollision( result, shared_from_this() );
			}
		}
	}
}

//--------------------------------------------------------------------------------

void Object::setParent( Object* parent ) {
	if( m_parent != nullptr )
		m_parent->removeChild( shared_from_this() );

	parent->addChild( shared_from_this() );
	m_parent = parent;
}

//--------------------------------------------------------------------------------

Math::Vec2 Object::getWorldPosition() const {
	if( m_parent == nullptr )
		return getPosition();

	return getPosition() + m_parent->getWorldPosition();
}

//--------------------------------------------------------------------------------

void Object::setWorldPosition( Math::Vec2 position ) {
	if( m_parent != nullptr )
		position -= m_parent->getWorldPosition();

	setPosition( position );
}

//--------------------------------------------------------------------------------

World* Object::getWorld() const {
	if( m_parent == nullptr )
		return nullptr;

	World* world;
	world = dynamic_cast< World* >( m_parent );

	if( world == nullptr )
		return m_parent->getWorld();
	else
		return world;
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Object > > Object::getChildren( bool recursive ) const {
	if( !recursive )
		return m_children;

	vector< shared_ptr< Object > > out = m_children;
	for( shared_ptr< Object > child : m_children ) {
		vector< shared_ptr< Object > > children = child->getChildren( true );
		out.insert( out.end(), children.begin(), children.end() );
	}

	return out;
}

//================================================================================

vector< shared_ptr< Object > > Object::s_objects;
vector< shared_ptr< Object > > Object::s_markedForDeletion;

//================================================================================