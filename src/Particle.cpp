//================================================================================

#include "Particle.h"

//--------------------------------------------------------------------------------

#include "Debug.h"
#include "World.h"
#include "Timer.h"
#include "System.h"
#include "Random.h"
#include "Emitter.h"

//================================================================================

namespace Gfx {
namespace Particle {

//================================================================================

vector< shared_ptr< Particle > > particles;
vector< Pattern > patterns;

//================================================================================

void Particle::onUpdate( sf::Time deltaTime ) {
	if( m_duration >= milliseconds( std::numeric_limits< int32_t >::min() ) ) {
		m_duration -= milliseconds( deltaTime.asMilliseconds() );
		if( m_duration <= 0ms )
			return destroy();

		m_alpha = 1.0f - float( m_duration.count() ) / float( m_pattern.initial.lifetime.value );
	}

	// Fade Acceleration
	Math::Vec2 acceleration = m_pattern.initial.acceleration.value;
	if( m_pattern.fade.acceleration.x )
		acceleration.x *= Math::mix( m_pattern.fade.acceleration.start.value, m_pattern.fade.acceleration.end.value, m_alpha );

	if( m_pattern.fade.acceleration.y )
		acceleration.y *= Math::mix( m_pattern.fade.acceleration.start.value, m_pattern.fade.acceleration.end.value, m_alpha );

	setVelocity( getVelocity() + acceleration * deltaTime.asSeconds() );

	// Fade Size
	if( m_pattern.fade.size.active )
		setSize( m_pattern.initial.size.value * Math::mix( m_pattern.fade.size.start.value, m_pattern.fade.size.end.value, m_alpha ) );

	// Fade Colour
	Math::Color color = m_pattern.initial.color.value;
	if( m_pattern.fade.color.r )
		color.r = Math::mix( m_pattern.initial.color.value.r, m_pattern.fade.color.target.value.r, m_alpha );
	if( m_pattern.fade.color.g )
		color.g = Math::mix( m_pattern.initial.color.value.g, m_pattern.fade.color.target.value.g, m_alpha );
	if( m_pattern.fade.color.b )
		color.b = Math::mix( m_pattern.initial.color.value.b, m_pattern.fade.color.target.value.b, m_alpha );
	if( m_pattern.fade.color.a )
		color.a = Math::mix( m_pattern.initial.color.value.a, m_pattern.fade.color.target.value.a, m_alpha );

	m_shape.setFillColor( color.sf() );
}

//--------------------------------------------------------------------------------

void Particle::onPostUpdate( sf::Time deltaTime ) {

	// Reset standard Post-Update
	setPosition( getPosition() - getVelocity() * deltaTime.asSeconds() );

	Math::Vec2 velocity = getVelocity();

	if( m_pattern.fade.velocity.x )
		velocity.x *= Math::mix( m_pattern.fade.velocity.start.value, m_pattern.fade.velocity.end.value, m_alpha );

	if( m_pattern.fade.velocity.y )
		velocity.y *= Math::mix< float >( m_pattern.fade.velocity.start.value, m_pattern.fade.velocity.end.value, m_alpha );

	setPosition( getPosition() + velocity * deltaTime.asSeconds() );
}

//--------------------------------------------------------------------------------

void Particle::onRender() {
	m_shape.setPosition( getPosition().sf() - getSize().sf() );
	System::getWindow()->draw( m_shape );
	Debug::incDrawCall();
}

//--------------------------------------------------------------------------------

void Particle::onDestroy() {
	m_dead = true;
}

//--------------------------------------------------------------------------------

void Particle::init( const Pattern& pattern ) {
	setName( "Bullet" );
	setVisibility( true );

	m_pattern = pattern;

	// Fades
	Math::processSet( m_pattern.fade.velocity.start );
	Math::processSet( m_pattern.fade.velocity.end );
	Math::processSet( m_pattern.fade.acceleration.start );
	Math::processSet( m_pattern.fade.acceleration.end );
	Math::processSet( m_pattern.fade.size.start );
	Math::processSet( m_pattern.fade.size.end );
	Math::processSet( m_pattern.fade.color.target );

	// Lifetime
	Math::processSet( m_pattern.initial.lifetime );

	milliseconds lifetime = milliseconds( m_pattern.initial.lifetime.value );
	if( lifetime > 0ms )
		m_duration = lifetime;

	// Position
	Math::processSet( m_pattern.initial.position );
	setPosition( m_pattern.initial.position.value );

	// Direction
	Math::processSet( m_pattern.initial.direction );

	// Velocity
	Math::processSet( m_pattern.initial.velocity );
	Math::Vec2 velocity = m_pattern.initial.direction.value.normalize() * m_pattern.initial.velocity.value;
	if( m_pattern.fade.velocity.x )
		velocity.x *= m_pattern.fade.velocity.start.value;
	else
		velocity.y *= m_pattern.fade.velocity.start.value;

	setVelocity( velocity );

	// Acceleration
	Math::processSet( m_pattern.initial.acceleration );

	// Size
	Math::processSet( m_pattern.initial.size );
	float size = m_pattern.initial.size.value;
	if( m_pattern.fade.size.active )
		size *= m_pattern.fade.size.start.value;

	setSize( size );

	// Color
	Math::processSet( m_pattern.initial.color );
	Math::Color color = m_pattern.initial.color.value;
	m_shape.setFillColor( m_pattern.initial.color.value.sf() );

	// Emitters
	for( Emitter::Pattern emitter : m_pattern.emitters )
		m_emitters.push_back( Emitter::spawn( this, emitter, getWorldPosition(), lifetime ) );
}

//================================================================================

void spawn( Object* parent, Pattern pattern, Math::Vec2 position ) {
	Math::processSet( pattern.initial.number );

	for( uint16_t i = 0u; i < pattern.initial.number.value; ++i ) {
		shared_ptr< Particle > particle = Object::makeObject< Particle >( parent );
		particle->init( pattern );
		particles.push_back( particle );
	}
}

//--------------------------------------------------------------------------------

void killAll() {
	for( shared_ptr< Particle > particle : particles )
		if( particle != nullptr ) {
			particle->kill();
			particle->destroy();
		}
}

//--------------------------------------------------------------------------------

void cleanup() {
	for( size_t i = 0u; i < particles.size(); ++i ) {
		if( !particles.at( i )->isDead() )
			continue;

		particles.erase( particles.begin() + i );
		--i;
	}
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Particle > > getAll() {
	return particles;
}

//--------------------------------------------------------------------------------

}
}

//================================================================================
