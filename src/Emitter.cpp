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

	m_duration -= milliseconds( deltaTime.asMilliseconds() );
	if( m_duration <= 0ms ) {
		destroy();
		return;
	}

	if( !( m_endTime == m_startTime ) )
		m_alpha = 1.0f - m_duration / ( m_endTime - m_startTime );

	if( m_pattern.activation.type == Pattern::Activation::Type::OnDestruction )
		return;

	m_spawnTime -= milliseconds( deltaTime.asMilliseconds() );

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
		m_pattern.activation.type == Pattern::Activation::Type::OnDestruction ) {

		for( const Particle::Pattern& pattern : m_pattern.patterns )
			spawnPattern( pattern );

		for( Particle::PatternSet& set : m_pattern.sets )
			spawnPattern( set.inc() );
	}

	m_dead = true;

	for( size_t i = 0; m_children.size() != 0; )
		m_children.at( 0 )->setParent( getWorld() );
}

//--------------------------------------------------------------------------------

void Emitter::init( const Pattern& pattern, milliseconds lifetime ) {
	m_pattern = pattern;
	setVisibility( true );

	processSet( m_pattern.activation.start );
	processSet( m_pattern.activation.end );
	processSet( m_pattern.rate.start );
	processSet( m_pattern.rate.end );

	m_endTime = milliseconds( std::numeric_limits< long long >::max() );

	if( pattern.activation.type == Pattern::Activation::Type::OnSpawn ) {
		m_active = true;
		calculateSpawnTime();
	}

	else if( pattern.activation.type == Pattern::Activation::Type::Alpha ) {
		m_startTime = milliseconds( int( lifetime.count() * m_pattern.activation.start.value ) );
		m_endTime = milliseconds( int( lifetime.count() * m_pattern.activation.end.value ) );
	}

	if( lifetime == -1ms )
		m_duration = milliseconds( std::numeric_limits< long long >::max() );
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
	if( m_pattern.rate.fade )
		rate = Math::mix( m_pattern.rate.start.value, m_pattern.rate.end.value, m_alpha );
	else
		rate = m_pattern.rate.start.value;

	if( rate == 0.0f )
		m_spawnTime = milliseconds( std::numeric_limits< long long >::max() );
	else
		m_spawnTime = milliseconds( int( 1000.0f / m_pattern.rate.start.value ) );
}

//--------------------------------------------------------------------------------

void Emitter::spawnPattern( const Particle::Pattern& pattern ) {
	processSet( m_pattern.position );
	processSet( m_pattern.multipliers.lifetime );
	processSet( m_pattern.multipliers.velocity );
	processSet( m_pattern.multipliers.acceleration );
	processSet( m_pattern.multipliers.size );
	processSet( m_pattern.multipliers.alpha );
	processSet( m_pattern.multipliers.number );

	Particle::Pattern p = pattern;

	p.initial.position.min += m_pattern.position.value;
	p.initial.position.max += m_pattern.position.value;

	p.initial.lifetime.min *= m_pattern.multipliers.lifetime.value;
	p.initial.lifetime.max *= m_pattern.multipliers.lifetime.value;

	p.initial.velocity.min *= m_pattern.multipliers.velocity.value;
	p.initial.velocity.max *= m_pattern.multipliers.velocity.value;

	p.initial.acceleration.min *= m_pattern.multipliers.acceleration.value;
	p.initial.acceleration.max *= m_pattern.multipliers.acceleration.value;

	p.shape.size.min *= m_pattern.multipliers.size.value;
	p.shape.size.max *= m_pattern.multipliers.size.value;

	p.shape.color.min.a *= m_pattern.multipliers.alpha.value;
	p.shape.color.max.a *= m_pattern.multipliers.alpha.value;

	p.initial.number.min *= m_pattern.multipliers.number.value;
	p.initial.number.max *= m_pattern.multipliers.number.value;

	Particle::Particle* parent = dynamic_cast< Particle::Particle* >( m_parent );
	if( parent == nullptr )
		return spawn( this, p );

	if( p.inheritance.position ) {
		p.initial.position.min += parent->getWorldPosition();
		p.initial.position.max += parent->getWorldPosition();
	}

	if( p.inheritance.velocity ) {
		p.initial.velocity.min += parent->getVelocity().length();
		p.initial.velocity.max += parent->getVelocity().length();
	}

	if( p.inheritance.alpha ) {
		p.shape.color.min.a *= Math::Color( parent->getShape()->getFillColor() ).a;
		p.shape.color.max.a *= Math::Color( parent->getShape()->getFillColor() ).a;
	}

	if( p.inheritance.size ) {
		p.shape.size.min *= parent->getSize().x;
		p.shape.size.max *= parent->getSize().x;
	}

	Particle::spawn( this, p );
}

//================================================================================

shared_ptr< Emitter > spawn( Object* parent, Pattern pattern, Math::Vec2 position, milliseconds lifetime ) {
	pattern.position.min += position;
	pattern.position.max += position;

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