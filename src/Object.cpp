//================================================================================

#include "Object.h"

//--------------------------------------------------------------------------------

#include "Debug.h"

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

	setPosition( getPosition() + getVelocity() * dt.asSeconds() );
	onPostUpdate( dt );

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->postUpdate( dt );
}

//--------------------------------------------------------------------------------

void Object::render() {
	if( isMarkedForRemoval() || !m_visibility )
		return;

	onRender();

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->render();
}

//--------------------------------------------------------------------------------

void Object::postRender() {
	if( isMarkedForRemoval() )
		return;

	onPostRender();

	const size_t size = m_children.size();
	for( size_t i = 0u; i < size; ++i )
		m_children.at( i )->postRender();
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

	Debug::startTimer( "Collision::Broad Phase" );

	// Broad Phase
	vector < collisionPair > results;
	for( shared_ptr< Object > target : targets ) {
		Collision::CollisionResult result = isColliding( target );
		if( result.success )
			results.push_back( make_pair( target, result ) );
	}

	Debug::stopTimer( "Collision::Broad Phase" );
	Debug::startTimer( "Collision::Sorting" );

	// Sort by distance
	std::sort( results.begin(), results.end(),
			   []( const collisionPair& a, const collisionPair& b ) {
				   return a.second.distance < b.second.distance;
			   } );

	Debug::stopTimer( "Collision::Sorting" );
	Debug::startTimer( "Collision::Narrow Phase" );

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

	Debug::stopTimer( "Collision::Narrow Phase" );
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

//================================================================================

vector< shared_ptr< Object > > Object::s_objects;
vector< shared_ptr< Object > > Object::s_markedForDeletion;

//================================================================================