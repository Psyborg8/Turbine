//================================================================================

#include "Bullet.h"

//--------------------------------------------------------------------------------

#include "World.h"
#include "Timer.h"
#include "System.h"
#include "Random.h"

//================================================================================

namespace Game {
namespace Bullet {

//================================================================================

class Bullet : public Object {
public:
	Bullet() = default;

	// Events
public:
	inline void onUpdate( sf::Time deltaTime ) override {

	}

	inline void onPostUpdate( sf::Time deltaTime ) override {
		m_shape.setPosition( getPosition().sf() );
	}

	inline void onRender() override {
		System::getWindow()->draw( m_shape );
	}

	inline void onDestroy() override {
		for( Timers::TimerID timer : m_timers )
			Timers::removeTimer( timer );
		m_timers.clear();

		for( shared_ptr< Bullet > bullet : m_bullets )
			bullet->destroy();
	}

	// Get-Set
public:
	inline void setSize( Math::Vec2 size ) override { m_shape.setRadius( size.x ); }
	inline Math::Vec2 getSize() const override { return Math::Vec2( m_shape.getRadius(), m_shape.getRadius() ); }

	inline void setRadius( float radius ) { m_shape.setRadius( radius ); }
	inline float getRadius() const { m_shape.getRadius(); }

	inline void setEmitter( Bullet* emitter ) { m_emitter = emitter; }
	inline Bullet* getEmitter() const { return m_emitter; }

	// Bullet
public:
	inline void constructBullet( Pattern pattern ) {
		// In case this bullet is being reconstructed.
		for( Timers::TimerID timer : m_timers )
			Timers::removeTimer( timer );
		m_timers.clear();

		Math::Vec2 position;
		if( pattern.getPosition().first == pattern.getPosition().second )
			position = pattern.getPosition().first;
		else {
			const float x = Random::getRandomFloatInRange( pattern.getPosition().first.x, pattern.getPosition().second.x );
			const float y = Random::getRandomFloatInRange( pattern.getPosition().first.y, pattern.getPosition().second.y );
			position = Math::Vec2( x, y );
		}
		setPosition( position );

		float radius;
		if( pattern.getSize().first == pattern.getSize().second )
			radius = pattern.getSize().first;
		else
			radius = Random::getRandomFloatInRange( pattern.getSize().first, pattern.getSize().second );
		setRadius( radius );

		Math::Vec2 velocity;
		if( pattern.getVelocity().first == pattern.getVelocity().second )
			velocity = pattern.getVelocity().first;
		else {
			const float x = Random::getRandomFloatInRange( pattern.getVelocity().first.x, pattern.getVelocity().second.x );
			const float y = Random::getRandomFloatInRange( pattern.getVelocity().first.y, pattern.getVelocity().second.y );
			velocity = Math::Vec2( x, y );
		}
		setVelocity( velocity );

		milliseconds lifetime;
		if( pattern.getLifetime().first == pattern.getLifetime().second )
			lifetime = pattern.getLifetime().first;
		else
			lifetime = milliseconds( Random::getRandomIntInRange( int( pattern.getLifetime().first.count() ), int( pattern.getLifetime().second.count() ) ) );

		m_shape.setFillColor( pattern.getColor().sf() );

		for( Pattern sequence : pattern.getSequence() ) {
			uint16_t count;
			if( pattern.getNumber().first == pattern.getNumber().second )
				count = pattern.getNumber().first;
			else
				count = uint16_t( Random::getRandomIntInRange( int( pattern.getNumber().first ), int( pattern.getNumber().second ) ) );

			for( uint16_t i = 0u; i < count; ++i ) {
				float time;
				if( sequence.getTime().first == sequence.getTime().second )
					time = sequence.getTime().first;
				else
					time = Random::getRandomFloatInRange( sequence.getTime().first, sequence.getTime().second );

				m_timers.push_back( Timers::addTimer( int( lifetime.count() * time ), nullptr,
													  [sequence, this] {
														  shared_ptr< Bullet > bullet = makeObject< Bullet >( m_parent );
														  Pattern newPattern = sequence;
														  bullet->setEmitter( getEmitter() );
														  newPattern.setPosition( newPattern.getPosition().first + getWorldPosition(), newPattern.getPosition().second + getWorldPosition() );
														  bullet->constructBullet( newPattern );
													  }, false ) );
			}
		}

		if( pattern.getColor().a > 0u )
			setVisibility( true );

		function< void() > onFinished;
		if( pattern.getLoop() && m_emitter != nullptr )
			onFinished = std::bind( &Bullet::constructBullet, m_emitter, pattern );
		else
			onFinished = std::bind( &Bullet::destroy, this );

		m_timers.push_back( Timers::addTimer( lifetime, 
											  [this, pattern, position, velocity, radius]( float alpha ) {
												  const auto positionFunc = pattern.getPositionFunction();
												  if( positionFunc != nullptr )
													  setPosition( positionFunc( alpha, position ) );
												  
												  const auto velocityFunc = pattern.getVelocityFunction();
												  if( velocityFunc != nullptr )
													  setVelocity( velocityFunc( alpha, velocity ) );

												  const auto sizeFunc = pattern.getSizeFunction();
												  if( sizeFunc != nullptr )
													  setRadius( sizeFunc( alpha, radius ) );

												  const auto colorFunc = pattern.getColorFunction();
												  if( colorFunc != nullptr )
													  m_shape.setFillColor( colorFunc( alpha, pattern.getColor() ).sf() );

											  }, onFinished, false ) );
	}

protected:
	sf::CircleShape m_shape;
	Bullet* m_emitter;
	vector< shared_ptr< Bullet > > m_bullets;
	vector< Timers::TimerID > m_timers;
	bool m_relative;
};

//================================================================================

void spawnBullet( Object* world, Pattern pattern ) {
	uint16_t count;
	if( pattern.getNumber().first == pattern.getNumber().second )
		count = pattern.getNumber().first;
	else
		count = uint16_t( Random::getRandomIntInRange( int( pattern.getNumber().first ), int( pattern.getNumber().second ) ) );

	for( uint16_t i = 0u; i < count; ++i ) {
		shared_ptr< Bullet > bullet = Object::makeObject< Bullet >( world );
		bullet->constructBullet( pattern );
	}
}

//--------------------------------------------------------------------------------

}
}

//================================================================================