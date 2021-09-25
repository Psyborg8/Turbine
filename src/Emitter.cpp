//================================================================================

#include "Emitter.h"

//--------------------------------------------------------------------------------

#include "Particle.h"
#include "World.h"

//================================================================================

namespace Gfx {
namespace Emitter {

//--------------------------------------------------------------------------------

vector< shared_ptr< Emitter > > emitters;

//================================================================================

void Emitter::onUpdate( sf::Time deltaTime ) {
	if( !m_active )
		return;

	m_duration -= microseconds( deltaTime.asMicroseconds() );
	if( m_duration <= 0ms ) {
		destroy();
		return;
	}

	if( !( m_endTime == m_startTime ) )
		m_alpha = 1.0f - m_duration / ( m_endTime - m_startTime );

	if( m_pattern.properties.activation.type == Pattern::Properties::Activation::Type::OnDestruction )
		return;

	m_spawnTime -= microseconds( deltaTime.asMicroseconds() );

	if( m_spawnTime <= 0ms ) {
		calculateSpawnTime();

		for( const Particle::Pattern& pattern : m_pattern.patterns )
			spawnPattern( pattern );

		for( Particle::PatternSet& set : m_pattern.sets )
			spawnPattern( set.inc() );
	}
}

//--------------------------------------------------------------------------------

void Emitter::onDestroy() {
	if( m_active &&
		m_pattern.properties.activation.type == Pattern::Properties::Activation::Type::OnDestruction ) {

		for( const Particle::Pattern& pattern : m_pattern.patterns )
			spawnPattern( pattern );

		for( Particle::PatternSet& set : m_pattern.sets )
			spawnPattern( set.inc() );
	}

	m_dead = true;
	m_active = false;

	for( size_t i = 0; m_children.size() != 0; )
		m_children.at( 0 )->setParent( getWorld() );
}

//--------------------------------------------------------------------------------

void Emitter::init( const Pattern& pattern, milliseconds lifetime ) {
	m_pattern = pattern;
	setVisibility( true );

	processSet( m_pattern.properties.position );
	processSet( m_pattern.properties.activation.start );
	processSet( m_pattern.properties.activation.end );
	processSet( m_pattern.properties.rate.start );
	processSet( m_pattern.properties.rate.end );

	m_endTime = microseconds( std::numeric_limits< long long >::max() );

	if( pattern.properties.activation.type == Pattern::Properties::Activation::Type::OnSpawn ) {
		m_active = true;
		calculateSpawnTime();

		for( const Particle::Pattern& pattern : m_pattern.patterns )
			spawnPattern( pattern );
		for( Particle::PatternSet& set : m_pattern.sets )
			spawnPattern( set.inc() );

		if( m_spawnTime == microseconds( std::numeric_limits< long long >::max() ) )
			return destroy();
	}

	else if( pattern.properties.activation.type == Pattern::Properties::Activation::Type::Alpha ) {
		m_startTime = microseconds( int( duration_cast< microseconds >( lifetime ).count() * m_pattern.properties.activation.start.value ) );
		m_endTime = microseconds( int( duration_cast< microseconds >( lifetime ).count() * m_pattern.properties.activation.end.value ) );
	}

	if( lifetime == -1ms )
		m_duration = microseconds( std::numeric_limits< long long >::max() );
	else
		m_duration = m_endTime - m_startTime;
}

//--------------------------------------------------------------------------------

void Emitter::start() {
	m_active = true;
}

//--------------------------------------------------------------------------------

void Emitter::stop() {
	m_active = false;
}

//--------------------------------------------------------------------------------

void Emitter::kill() {
	for( shared_ptr< Object > child : m_children )
		child->destroy();
}

//--------------------------------------------------------------------------------

void Emitter::calculateSpawnTime() {
	float rate;
	if( m_pattern.properties.rate.fade )
		rate = Math::mix( m_pattern.properties.rate.start.value, m_pattern.properties.rate.end.value, m_alpha );
	else
		rate = m_pattern.properties.rate.start.value;

	if( rate == 0.0f )
		m_spawnTime = microseconds( std::numeric_limits< long long >::max() );
	else
		m_spawnTime = microseconds( int( 1000000.f / m_pattern.properties.rate.start.value ) );
}

//--------------------------------------------------------------------------------

void Emitter::spawnPattern( const Particle::Pattern& pattern ) {
	processSet( m_pattern.multipliers.lifetime );
	processSet( m_pattern.multipliers.velocity );
	processSet( m_pattern.multipliers.acceleration );
	processSet( m_pattern.multipliers.size );
	processSet( m_pattern.multipliers.alpha );
	processSet( m_pattern.multipliers.number );

	Particle::Pattern p = pattern;

	p.physics.position.position.min += m_pattern.properties.position.value;
	p.physics.position.position.min += m_pattern.properties.position.value;

	p.properties.lifetime.min *= m_pattern.multipliers.lifetime.value;
	p.properties.lifetime.max *= m_pattern.multipliers.lifetime.value;

	p.physics.velocity.direction.power.min *= m_pattern.multipliers.velocity.value;
	p.physics.velocity.direction.power.max *= m_pattern.multipliers.velocity.value;
	p.physics.velocity.point.power.min *= m_pattern.multipliers.velocity.value;
	p.physics.velocity.point.power.max *= m_pattern.multipliers.velocity.value;
	p.physics.velocity.value.value.min *= m_pattern.multipliers.velocity.value;
	p.physics.velocity.value.value.max *= m_pattern.multipliers.velocity.value;

	p.physics.acceleration.direction.power.min *= m_pattern.multipliers.velocity.value;
	p.physics.acceleration.direction.power.max *= m_pattern.multipliers.velocity.value;
	p.physics.acceleration.point.power.min *= m_pattern.multipliers.velocity.value;
	p.physics.acceleration.point.power.max *= m_pattern.multipliers.velocity.value;
	p.physics.acceleration.value.value.min *= m_pattern.multipliers.velocity.value;
	p.physics.acceleration.value.value.max *= m_pattern.multipliers.velocity.value;

	p.shape.size.size.min *= m_pattern.multipliers.size.value;
	p.shape.size.size.max *= m_pattern.multipliers.size.value;

	p.shape.color.color.min.a *= m_pattern.multipliers.alpha.value;
	p.shape.color.color.max.a *= m_pattern.multipliers.alpha.value;
	p.shape.outline.color.color.min.a *= m_pattern.multipliers.alpha.value;
	p.shape.outline.color.color.max.a *= m_pattern.multipliers.alpha.value;

	p.properties.number.min *= m_pattern.multipliers.number.value;
	p.properties.number.max *= m_pattern.multipliers.number.value;

	if( p.inheritance.position ) {
		p.physics.position.position.min += getPosition();
		p.physics.position.position.max += getPosition();
	}

	Particle::Particle* parent = dynamic_cast< Particle::Particle* >( m_parent );
	if( parent == nullptr )
		return spawn( this, p );

	if( p.inheritance.velocity ) {
		p.physics.velocity.direction.power.min *= parent->getVelocity().length();
		p.physics.velocity.direction.power.max *= parent->getVelocity().length();
		p.physics.velocity.point.power.min *= m_parent->getVelocity().length();
		p.physics.velocity.point.power.max *= m_parent->getVelocity().length();
		p.physics.velocity.value.value.min *= m_parent->getVelocity().length();
		p.physics.velocity.value.value.max *= m_parent->getVelocity().length();
	}

	if( p.inheritance.alpha ) {
		p.shape.color.color.min.a *= parent->getColor().a;
		p.shape.color.color.max.a *= parent->getColor().a;
		p.shape.outline.color.color.min.a *= parent->getOutlineColor().a;
		p.shape.outline.color.color.max.a *= parent->getOutlineColor().a;
	}

	if( p.inheritance.size ) {
		p.shape.size.size.min *= parent->getSize();
		p.shape.size.size.max *= parent->getSize();
	}

	// Alpha & Size

	Particle::spawn( this, p );
}

//================================================================================

shared_ptr< Emitter > spawn( Object* parent, Pattern pattern, Math::Vec2 position, milliseconds lifetime ) {
	pattern.properties.position.min += position;
	pattern.properties.position.max += position;

	shared_ptr< Emitter > emitter = Object::makeObject< Emitter >( parent );
	emitter->init( pattern, lifetime );
	emitters.push_back( emitter );
	return emitter;
}

//--------------------------------------------------------------------------------

void stopAll() {
	for( shared_ptr< Emitter > emitter : emitters )
		emitter->stop();
}

//--------------------------------------------------------------------------------

void killAll() {
	for( shared_ptr< Emitter > emitter : emitters )
		emitter->kill();
}

//--------------------------------------------------------------------------------

void destroyAll() {
	for( shared_ptr< Emitter > emitter : emitters ) {
		emitter->stop();
		emitter->destroy();
	}

	emitters.clear();
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Emitter > > getAll() {
	return emitters;
}

//--------------------------------------------------------------------------------

void cleanup() {
	for( size_t i = 0; i < emitters.size(); ++i ) {
		if( !emitters.at( i )->isDead() )
			continue;

		emitters.erase( emitters.begin() + i );
		--i;
	}
}

//--------------------------------------------------------------------------------

}
}

//================================================================================