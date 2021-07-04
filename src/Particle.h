//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "Random.h"
#include "Timer.h"
#include "RigidRect.h"

//================================================================================

namespace Gfx {
namespace Particle {

//--------------------------------------------------------------------------------

class Pattern {
public:
	template < class T >
	using Range = pair< T, T >;

	template< class T >
	using Function = function< T( float alpha, T origin, T current ) >;

	//--------------------------------------------------------------------------------

	struct Emitter {
		Emitter& addPattern( Pattern pattern ) { m_patterns.push_back( pattern ); return *this; }

		Emitter& setSpawnRate( float spawnRate ) { m_spawnRate = make_pair( spawnRate, spawnRate ); return *this; }
		Emitter& setSpawnRate( float min, float max ) { m_spawnRate = make_pair( min, max ); return *this; }
		Emitter& setSpawnRateFunc( Function< float > func ) { m_spawnRateFunc = func; return *this; }

		Emitter& setDelay( milliseconds delay ) { m_delay = make_pair( delay, delay ); return *this; }
		Emitter& setDelay( milliseconds min, milliseconds max ) { m_delay = make_pair( min, max ); return *this; }
		Emitter& setDelayFunc( Function< milliseconds > func ) { m_delayFunc = func; return *this; }

		const vector< Pattern >& get() const { return m_patterns; }
		vector< Pattern >& get() { return m_patterns; }

		Range< float > getSpawnRate() const { return m_spawnRate; }
		Function< float > getSpawnRateFunc() const { return m_spawnRateFunc; }

		Range< milliseconds > getDelay() const { return m_delay; }
		Function< milliseconds > getDelayFunc() const { return m_delayFunc; }

	protected:
		vector< Pattern > m_patterns;

		Range< float > m_spawnRate;
		Function< float > m_spawnRateFunc;

		Range< milliseconds > m_delay;
		Function< milliseconds > m_delayFunc;
	};

	//--------------------------------------------------------------------------------
	
	struct Lifetime {
		Lifetime& set( milliseconds lifetime ) { m_range = make_pair( lifetime, lifetime ); return *this; }
		Lifetime& set( milliseconds min, milliseconds max ) { m_range = make_pair( min, max ); return *this; }

		Range< milliseconds > get() const { return m_range; }

	protected:
		Range< milliseconds > m_range;
	};

	//--------------------------------------------------------------------------------

	struct Position {
		Position& set( Math::Vec2 position ) { m_range = make_pair( position, position ); return *this; }
		Position& set( float x, float y ) { Math::Vec2 position{ x, y }; return set( position ); }
		Position& set( Math::Vec2 min, Math::Vec2 max ) { m_range = make_pair( min, max ); return *this; }
		Position& setRelative( bool relative ) { m_relative = relative; return *this; }

		Range< Math::Vec2 > get() const { return m_range; }
		bool getRelative() const { return m_relative; }

	protected:
		Range< Math::Vec2 > m_range;
		bool m_relative{ false };
	};

	//--------------------------------------------------------------------------------

	struct Velocity {
		Velocity& set( Math::Vec2 velocity ) { m_range = make_pair( velocity, velocity ); return *this; }
		Velocity& set( float x, float y ) { Math::Vec2 velocity{ x, y }; return set( velocity ); }
		Velocity& set( Math::Vec2 min, Math::Vec2 max ) { m_range = make_pair( min, max ); return *this; }
		Velocity& setRelative( bool relative ) { m_relative = relative; return *this; }
		Velocity& setFade( bool x, bool y ) { m_fade.first = x; m_fade.second = y; return *this; }

		Range< Math::Vec2 > get() const { return m_range; }
		bool getRelative() const { return m_relative; }
		pair< bool, bool > getFade() const { return m_fade; }

	protected:
		Range< Math::Vec2 > m_range;
		bool m_relative{ false };
		pair< bool, bool > m_fade{ false, false };
	};

	//--------------------------------------------------------------------------------

	struct Size {
		Size& set( float size ) { m_range = make_pair( size, size ); return *this; }
		Size& set( float min, float max ) { m_range = make_pair( min, max ); return *this; }
		Size& setRelative( bool relative ) { m_relative = relative; return *this; }
		Size& setFade( bool fade ) { m_fade = fade; return *this; }

		Range< float > get() const { return m_range; }
		bool getRelative() const { return m_relative; }
		bool getFade() const { return m_fade; }

	protected:
		Range< float > m_range{ 1.0f, 1.0f };
		bool m_relative{ false };
		bool m_fade{ false };
	};

	//--------------------------------------------------------------------------------

	struct Number {
		Number& set( uint16_t number ) { m_range = make_pair( number, number ); return *this; }
		Number& set( uint16_t min, uint16_t max ) { m_range = make_pair( min, max ); return *this; }

		Range< uint16_t > get() const { return m_range; }

	protected:
		Range< uint16_t > m_range{ 1u, 1u };
	};

	//--------------------------------------------------------------------------------

	struct Color {
		Color& set( Math::Color color ) { m_range = make_pair( color, color ); return *this; }
		Color& set( Math::Color min, Math::Color max, Random::RandomColorType type ) { m_range = make_pair( min, max ); m_type = type; return *this; }
		Color& setRelative( bool relative ) { m_relative = relative; return *this; }
		Color& setFade( bool fade ) { m_fade = fade; return *this; }

		Range< Math::Color > get() const { return m_range; }
		Random::RandomColorType getType() const { return m_type; }
		bool getRelative() const { return m_relative; }
		bool getFade() const { return m_fade; }

	protected:
		Range< Math::Color > m_range;
		Random::RandomColorType m_type;
		bool m_relative{ false };
		bool m_fade{ false };
	};

	//--------------------------------------------------------------------------------

	struct Gravity {
		enum class Type {
			Direction = 0,
			Point,
		};

		Gravity& setDirection( Math::Vec2 direction ) { m_range = make_pair( direction, direction ); m_type = Type::Direction; return *this; }
		Gravity& setDirection( float x, float y ) { Math::Vec2 direction{ x, y }; return setDirection( direction ); }
		Gravity& setDirection( Math::Vec2 min, Math::Vec2 max ) { m_range = make_pair( min, max ); m_type = Type::Direction; return *this; }

		Gravity& setPower( float power ) { m_power = make_pair( power, power ); return *this; }
		Gravity& setPower( float min, float max ) { m_power = make_pair( min, max ); return *this; }

		Gravity& setPoint( Math::Vec2 point ) { setDirection( point ); m_type = Type::Point; return *this; }
		Gravity& setPoint( float x, float y ) { setDirection( x, y ); m_type = Type::Point; return *this; }
		Gravity& setPoint( Math::Vec2 min, Math::Vec2 max ) { setDirection( min, max ); m_type = Type::Point; return *this; }

		Gravity& setDistanceMultiplier( float multiplier ) { m_distanceMultiplier = make_pair( multiplier, multiplier ); return *this; }
		Gravity& setDistanceMultiplier( float min, float max ) { m_distanceMultiplier = make_pair( min, max ); return *this; }

		Range< Math::Vec2 > get() const { return m_range; }
		Range< float > getPower() const { return m_power; }
		Range< float > getDistanceMultiplier() const { return m_distanceMultiplier; }
		Type getType() const { return m_type; }

	protected:
		Range< Math::Vec2 > m_range;
		Range< float > m_power{ 0.0f, 0.0f };
		Range< float > m_distanceMultiplier{ 0.0f, 0.0f };
		Type m_type;
	};

	//--------------------------------------------------------------------------------

	struct Collider {
		Collider& set( shared_ptr< Object > collider ) { m_collider = collider; return *this; }
		Collider& set( Math::Vec2 position, Math::Vec2 size ) { 
			shared_ptr< Game::RigidRect > collider = make_shared< Game::RigidRect >();
			collider->setPosition( position );
			collider->setSize( size );
			collider->setCollisionType( CollisionType::Static );
			set( collider );
		}
		shared_ptr< Object > get() const { return m_collider; }

		Collider& setKill( bool kill ) { m_kill = kill; return *this; }
		bool getKill() const { return m_kill; }

		Collider& addPattern( Pattern pattern ) { m_patterns.push_back( pattern ); return *this; }
		const vector< Pattern >& getPatterns() const { return m_patterns; }

	protected:
		shared_ptr< Object > m_collider;
		bool m_kill;
		vector< Pattern > m_patterns;
	};

	//================================================================================

public:
	Pattern() = default;

public:
	// Properties
	Pattern& setLifetime( Lifetime lifetime ) { m_lifetime = lifetime; return *this; }
	Pattern& setPosition( Position position ) { m_position = position; return *this; }
	Pattern& setVelocity( Velocity velocity ) { m_velocity = velocity; return *this; }
	Pattern& setSize( Size size ) { m_size = size; return *this; }
	Pattern& setNumber( Number number ) { m_number = number; return *this; }
	Pattern& setColor( Color color ) { m_color = color; return *this; }
	Pattern& setGravity( Gravity gravity ) { m_gravity = gravity; return *this; }

	Lifetime& getLifetime() { return m_lifetime; }
	Position& getPosition() { return m_position; }
	Velocity& getVelocity() { return m_velocity; }
	Size& getSize() { return m_size; }
	Number& getNumber() { return m_number; }
	Color& getColor() { return m_color; }
	Gravity& getGravity() { return m_gravity; }

	const Lifetime& getLifetime() const { return m_lifetime; }
	const Position& getPosition() const { return m_position; }
	const Velocity& getVelocity() const { return m_velocity; }
	const Size& getSize() const { return m_size; }
	const Number& getNumber() const { return m_number; }
	const Color& getColor() const { return m_color; }
	const Gravity& getGravity() const { return m_gravity; }

	Pattern& addEmitter( Emitter emitter ) { m_emitters.push_back( emitter ); return *this; }
	const vector< Emitter >& getEmitters() const { return m_emitters; }
	vector< Emitter >& getEmitters() { return m_emitters; }

	Pattern& addCollider( Collider collider ) { m_colliders.push_back( collider ); return *this; }
	const vector< Collider >& getColliders() const { return m_colliders; }
	vector< Collider >& getColliders() { return m_colliders; }

protected:
	vector< Emitter > m_emitters;
	vector< Collider > m_colliders;

	Lifetime m_lifetime;
	Position m_position;
	Velocity m_velocity;
	Size m_size;
	Number m_number;
	Color m_color;
	Gravity m_gravity;
};

//================================================================================

class Particle : public Object {
public:
	struct Emitter {
		bool active{ true };
		Pattern pattern;
		Pattern::Range< float > rate{ 0.0f, 0.0f };
		milliseconds delay;
		sf::Clock clock;
	};

	Particle() = default;

	// Events
public:
	void onUpdate( sf::Time deltaTime ) override;
	void onProcessCollisions() override;
	Collision::CollisionResult isColliding( shared_ptr< Object > target ) override;
	void onPostUpdate( sf::Time deltaTime ) override;
	void onRender() override;
	void onDestroy() override;

	// Get-Set
public:
	inline void setSize( Math::Vec2 size ) override { m_shape.setRadius( size.x ); }
	inline Math::Vec2 getSize() const override { return Math::Vec2( m_shape.getRadius(), m_shape.getRadius() ); }

	inline void setRadius( float radius ) { m_shape.setRadius( radius ); }
	inline float getRadius() const { return m_shape.getRadius(); }

	inline void setEmitter( Particle* emitter ) { m_emitter = emitter; }
	inline Particle* getEmitter() const { return m_emitter; }

	inline void setColliders( vector< Pattern::Collider > colliders ) { m_colliders = colliders; }
	inline const vector< Pattern::Collider >& getColliders() const { return m_colliders; }
	inline vector< Pattern::Collider >& getColliders() { return m_colliders; }

	inline void addEmitter( Pattern pattern, Pattern::Range< float > rate, milliseconds delay ) { 
		Emitter emitter; 
		emitter.pattern = pattern; 
		emitter.delay = delay; 
		emitter.rate = rate; 
		m_emitters.push_back( emitter ); }

	inline void setGravity( Math::Vec2 gravity, float power, float multiplier, Pattern::Gravity::Type type ) { m_gravity = gravity; m_gravityPower = power; m_gravityMultiplier = multiplier; m_gravityType = type; }

	inline sf::CircleShape& getShape() { return m_shape; }
	inline const sf::CircleShape& getShape() const { return m_shape; }

	// Variables
protected:
	sf::CircleShape m_shape;
	Timers::TimerID m_timer;
	vector< Emitter > m_emitters;
	Particle* m_emitter;
	vector< Pattern::Collider > m_colliders;

	Math::Vec2 m_gravity;
	Pattern::Gravity::Type m_gravityType;
	float m_gravityPower;
	float m_gravityMultiplier;

	// Bullet
public:
	void constructParticle( Pattern pattern );
};

//================================================================================

void spawnParticle( Object* parent, Pattern pattern );

//--------------------------------------------------------------------------------

}
}

//================================================================================