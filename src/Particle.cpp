//================================================================================

#include "Particle.h"

//--------------------------------------------------------------------------------

#include "World.h"
#include "Timer.h"
#include "System.h"
#include "Random.h"

//================================================================================

namespace Gfx {
namespace Particle {

//================================================================================

vector< shared_ptr< Particle > > particles;

//================================================================================

void Particle::onUpdate( sf::Time deltaTime ) {
	// Gravity
	if( m_gravity.length() > 0.0f ) {
		if( m_gravityType == Pattern::Gravity::Type::Direction ) {
			m_velocity += m_gravity.normalize() * m_gravityPower * deltaTime.asSeconds();
		}
		else if( m_gravityType == Pattern::Gravity::Type::Point ) {
			Math::Vec2 direction = m_gravity - m_position;
			direction = direction.normalize();
			if( m_gravityMultiplier != 0.0f )
				direction *= m_gravityMultiplier;
			direction *= m_gravityPower * deltaTime.asSeconds();

			m_velocity += direction;
		}
	}
}

//--------------------------------------------------------------------------------

void Particle::onProcessCollisions() {
	for( const Pattern::Collider& collider : m_colliders ) {
		Collision::CollisionResult result = isColliding( collider.get() );

		if( result.success ) {
			collider.get()->onCollision( result, shared_from_this() );

			for( const Pattern& pattern : collider.getPatterns() ) {
				shared_ptr< Particle > particle = Object::makeObject< Particle >( m_parent );
				particle->setEmitter( this );
				particle->constructParticle( pattern );
			}

			if( collider.getKill() )
				destroy();
		}
	}
}

//--------------------------------------------------------------------------------

Collision::CollisionResult Particle::isColliding( shared_ptr< Object > target ) {
	shared_ptr< Game::RigidRect > ptr = std::dynamic_pointer_cast< Game::RigidRect >( target );
	if( ptr == nullptr )
		return Collision::CollisionResult();

	Collision::Ray ray;
	ray.start = getPosition();
	ray.end = getPosition() + getVelocity();

	return Collision::collision( ray, ptr );
}

//--------------------------------------------------------------------------------

void Particle::onPostUpdate( sf::Time deltaTime ) {
	m_shape.setPosition( ( getPosition() - Math::Vec2( m_shape.getRadius() / 2.0f, m_shape.getRadius() / 2.0f ) ).sf() );

	// Emitters
	for( Emitter &emitter : m_emitters ) {
		if( !emitter.active )
			continue;

		if( emitter.clock.getElapsedTime().asMilliseconds() > emitter.delay.count() ) {
			uint16_t count = uint16_t( Random::getRandomIntInRange( int( emitter.pattern.getNumber().get().first ), int( emitter.pattern.getNumber().get().second ) ) );

			for( uint16_t i = 0u; i < count; ++i ) {
				shared_ptr< Particle > particle = Object::makeObject< Particle >( m_parent );
				particle->setEmitter( this );
				particle->constructParticle( emitter.pattern );
				particle->onPostUpdate( deltaTime );
			}

			float rate = Random::getRandomFloatInRange( emitter.rate.first, emitter.rate.second );
			if( rate == 0 ) {
				emitter.active = false;
				continue;
			}

			emitter.delay = milliseconds( int( 1000.0f / rate ) );
			emitter.clock.restart();
		}
	}
}

//--------------------------------------------------------------------------------

void Particle::onRender() {
	System::getWindow()->draw( m_shape );
}

//--------------------------------------------------------------------------------

void Particle::onDestroy() {
	Timers::removeTimer( m_timer );
}

//--------------------------------------------------------------------------------

void Particle::constructParticle( Pattern pattern ) {
	setName( "Bullet" );

	// In case this bullet is being reconstructed.
	Timers::removeTimer( m_timer );

	// Position
	Math::Vec2 position = Random::getRandomVec2InRange( pattern.getPosition().get().first, pattern.getPosition().get().second );
	if( pattern.getPosition().getRelative() && m_emitter != nullptr )
		position += m_emitter->getPosition();
	setWorldPosition( position );

	// Radius
	float radius = Random::getRandomFloatInRange( pattern.getSize().get().first, pattern.getSize().get().second );
	if( pattern.getSize().getRelative() && m_emitter != nullptr )
		radius *= m_emitter->getRadius();
	setRadius( radius );

	// Velocity
	Math::Vec2 velocity = Random::getRandomVec2InRange( pattern.getVelocity().get().first, pattern.getVelocity().get().second );
	if( pattern.getVelocity().getRelative() && m_emitter != nullptr )
		velocity += m_emitter->getVelocity();
	setVelocity( velocity );

	// Color
	Math::Color color = Random::getRandomColorInRange( pattern.getColor().get().first, pattern.getColor().get().second, pattern.getColor().getType() );
	if( pattern.getColor().getRelative() && m_emitter != nullptr )
		color.a *= m_emitter->getShape().getFillColor().a;
	m_shape.setFillColor( color.sf() );
	setVisibility( true );

	// Gravity
	Math::Vec2 gravity = Random::getRandomVec2InRange( pattern.getGravity().get().first, pattern.getGravity().get().second );
	float gravityPower = Random::getRandomFloatInRange( pattern.getGravity().getPower().first, pattern.getGravity().getPower().second );
	float gravityMultiplier = Random::getRandomFloatInRange( pattern.getGravity().getDistanceMultiplier().first, pattern.getGravity().getDistanceMultiplier().second );
	setGravity( gravity, gravityPower, gravityMultiplier, pattern.getGravity().getType() );

	// Emitter
	m_emitters.clear();
	for( Pattern::Emitter emitter : pattern.getEmitters() ) {
		for( Pattern pattern : emitter.get() ) {
			milliseconds delay = milliseconds( Random::getRandomIntInRange( int( emitter.getDelay().first.count() ), int( emitter.getDelay().second.count() ) ) );
			addEmitter( pattern, emitter.getSpawnRate(), delay );
		}
	}

	// Colliders
	m_colliders = pattern.getColliders();

	// Lifetime
	milliseconds lifetime = milliseconds( Random::getRandomIntInRange( int( pattern.getLifetime().get().first.count() ), int( pattern.getLifetime().get().second.count() ) ) );
	if( lifetime.count() > 0 ) {
		m_timer = Timers::addTimer( lifetime,
									[this, pattern, position, velocity, radius, color, gravity, gravityPower, gravityMultiplier]( float alpha ) {
										if( pattern.getVelocity().getFade().first || pattern.getVelocity().getFade().second ) {
											Math::Vec2 v;
											v.x = pattern.getVelocity().getFade().first ? velocity.x * ( 1.0f - alpha ) : getVelocity().x;
											v.y = pattern.getVelocity().getFade().second ? velocity.y * ( 1.0f - alpha ) : getVelocity().y;
											setVelocity( v );
										}

										if( pattern.getSize().getFade() )
											setRadius( radius * ( 1.0f - alpha ) );

										if( pattern.getColor().getFade() ) {
											Math::Color c = color;
											c.a = color.a * ( 1.0f - alpha );
											m_shape.setFillColor( c.sf() );
										}

									}, std::bind( &Particle::destroy, this ), false );
	}
}

//================================================================================

void spawnParticle( Object* parent, Pattern pattern ) {
	uint16_t count = uint16_t( Random::getRandomIntInRange( int( pattern.getNumber().get().first ), int( pattern.getNumber().get().second ) ) );

	for( uint16_t i = 0u; i < count; ++i ) {
		shared_ptr< Particle > particle = Object::makeObject< Particle >( parent );
		particle->constructParticle( pattern );
	}
}

//--------------------------------------------------------------------------------

}
}

//================================================================================