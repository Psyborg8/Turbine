//================================================================================

#include "Particle.h"

//--------------------------------------------------------------------------------

#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

#include <filesystem>

#include "Debug.h"
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

//================================================================================

// Pattern Loader

//================================================================================

rapidjson::MemoryPoolAllocator < rapidjson::CrtAllocator >* allocator;

//--------------------------------------------------------------------------------

// Saving
template< class T >
rapidjson::Value getValue( Pattern::Range< T > range );
rapidjson::Value getValue( Math::Vec2 value );
rapidjson::Value getValue( Math::Color value );
rapidjson::Value getValue( int value );
rapidjson::Value getValue( float value );
rapidjson::Value getValue( milliseconds value );
rapidjson::Value getValue( bool value );
rapidjson::Value getValue( string value );

rapidjson::Value getValue( const Pattern& value );
rapidjson::Value getValue( const Pattern::Position& value );
rapidjson::Value getValue( const Pattern::Velocity& value );
rapidjson::Value getValue( const Pattern::Lifetime& value );
rapidjson::Value getValue( const Pattern::Size& value );
rapidjson::Value getValue( const Pattern::Number& value );
rapidjson::Value getValue( const Pattern::Color& value );
rapidjson::Value getValue( const Pattern::Gravity& value );
rapidjson::Value getValue( const Pattern::Emitter& value );
rapidjson::Value getValue( const vector< Pattern::Emitter >& value );

//--------------------------------------------------------------------------------

// Loading
template< class T >
void getValue( const rapidjson::Value& value, Pattern::Range< T >& out );
template< class T >
void getValue( const rapidjson::Value& value, vector< T >& out );
void getValue( const rapidjson::Value& value, bool& out );
void getValue( const rapidjson::Value& value, float& out );
void getValue( const rapidjson::Value& value, int& out );
void getValue( const rapidjson::Value& value, milliseconds& out );
void getValue( const rapidjson::Value& value, Math::Vec2& out );
void getValue( const rapidjson::Value& value, Math::Color& out );

void getValue( const rapidjson::Value& value, Pattern& out );
void getValue( const rapidjson::Value& value, Pattern::Position& out );
void getValue( const rapidjson::Value& value, Pattern::Velocity& out );
void getValue( const rapidjson::Value& value, Pattern::Lifetime& out );
void getValue( const rapidjson::Value& value, Pattern::Size& out );
void getValue( const rapidjson::Value& value, Pattern::Number& out );
void getValue( const rapidjson::Value& value, Pattern::Color& out );
void getValue( const rapidjson::Value& value, Pattern::Gravity& out );
void getValue( const rapidjson::Value& value, Pattern::Emitter& out );

//================================================================================

// SAVING

//================================================================================

void savePattern( Pattern pattern, string filepath ) {
	using namespace rapidjson;

	Document document;
	document.SetObject();

	allocator = &document.GetAllocator();
	document.CopyFrom( getValue( pattern ), *allocator );

	FILE* file;
	fopen_s( &file, filepath.c_str(), "wb" );

	char writeBuffer[ 65536 ];
	FileWriteStream stream( file, writeBuffer, sizeof( writeBuffer ) );
	PrettyWriter< FileWriteStream > writer( stream );
	writer.SetMaxDecimalPlaces( 2 );
	writer.SetFormatOptions( PrettyFormatOptions::kFormatSingleLineArray );

	document.Accept( writer );
}

//================================================================================

template< class T >
rapidjson::Value getValue( Pattern::Range< T > range ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "min", getValue( range.first ), *allocator );
	out.AddMember( "max", getValue( range.second ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( Math::Vec2 value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "x", getValue( value.x ), *allocator );
	out.AddMember( "y", getValue( value.y ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( Math::Color value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "red", getValue( value.r ), *allocator );
	out.AddMember( "green", getValue( value.g ), *allocator );
	out.AddMember( "blue", getValue( value.b ), *allocator );
	out.AddMember( "alpha", getValue( value.a ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( int value ) {
	rapidjson::Value out;
	out.SetInt( value );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( float value ) {
	rapidjson::Value out;
	out.SetFloat( value );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( milliseconds value ) {
	rapidjson::Value out;
	out.SetInt64( value.count() );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( bool value ) {
	rapidjson::Value out;
	out.SetBool( value );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( string value ) {
	rapidjson::Value out;
	out.SetString( rapidjson::GenericStringRef< char >( value.c_str() ) );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "lifetime", getValue( value.getLifetime() ), *allocator );
	out.AddMember( "position", getValue( value.getPosition() ), *allocator );
	out.AddMember( "velocity", getValue( value.getVelocity() ), *allocator );
	out.AddMember( "size", getValue( value.getSize() ), *allocator );
	out.AddMember( "number", getValue( value.getNumber() ), *allocator );
	out.AddMember( "color", getValue( value.getColor() ), *allocator );
	out.AddMember( "gravity", getValue( value.getGravity() ), *allocator );
	out.AddMember( "emitter", getValue( value.getEmitters() ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Lifetime& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "value", getValue( value.get() ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Position& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "value", getValue( value.get() ), *allocator );
	out.AddMember( "relative", getValue( value.getRelative() ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Velocity& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "value", getValue( value.get() ), *allocator );
	out.AddMember( "relative", getValue( value.getRelative() ), *allocator );
	out.AddMember( "fadex", getValue( value.getFade().first ), *allocator );
	out.AddMember( "fadey", getValue( value.getFade().second ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Size& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "value", getValue( value.get() ), *allocator );
	out.AddMember( "relative", getValue( value.get() ), *allocator );
	out.AddMember( "fade", getValue( value.getFade() ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Number& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "value", getValue( value.get() ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Color& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "value", getValue( value.get() ), *allocator );
	out.AddMember( "relative", getValue( value.getRelative() ), *allocator );
	out.AddMember( "type", getValue( static_cast< int >( value.getType() ) ), *allocator );
	out.AddMember( "fade", getValue( value.getFade() ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Gravity& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "type", static_cast< int >( value.getType() ), *allocator );
	out.AddMember( "value", getValue( value.get() ), *allocator );
	out.AddMember( "power", getValue( value.getPower() ), *allocator );
	out.AddMember( "multiplier", getValue( value.getDistanceMultiplier() ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const vector< Pattern::Emitter >& value ) {
	rapidjson::Value out;
	out.SetArray();

	for( const Pattern::Emitter& emitter : value )
		out.PushBack( getValue( emitter ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Emitter& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "spawnrate", getValue( value.getSpawnRate() ), *allocator );
	out.AddMember( "delay", getValue( value.getDelay() ), *allocator );

	rapidjson::Value patterns;
	patterns.SetArray();

	for( const Pattern& pattern : value.get() )
		patterns.PushBack( getValue( pattern ), *allocator );

	out.AddMember( "patterns", patterns, *allocator );

	return out;
}

//================================================================================

// LOADING

//================================================================================

Pattern loadPattern( string name ) {
	using namespace rapidjson;
	using namespace Particle;

	Pattern out;

	// Parse JSON
	string path = Folders::Bullets + name + ".json";
	FILE* pFile;
	if( fopen_s( &pFile, path.c_str(), "rb" ) )
		return Pattern();
	char buffer[ 65536 ];
	FileReadStream is( pFile, buffer, sizeof( buffer ) );
	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>( is );
	fclose( pFile );

	if( document.HasParseError() ) {
		Debug::addMessage( "Map \"" + name + "\" has JSON parse error", Debug::DebugType::Error );
		return out;
	}

	getValue( document, out );

	return out;
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern& out ) {
	Pattern::Lifetime lifetime;
	if( value.HasMember( "lifetime" ) )
		getValue( value[ "lifetime" ], lifetime );
	out.setLifetime( lifetime );

	Pattern::Position position;
	if( value.HasMember( "position" ) )
		getValue( value[ "position" ], position );
	out.setPosition( position );

	Pattern::Velocity velocity;
	if( value.HasMember( "velocity" ) )
		getValue( value[ "velocity" ], velocity );
	out.setVelocity( velocity );

	Pattern::Size size;
	if( value.HasMember( "size" ) )
		getValue( value[ "size" ], size );
	out.setSize( size );

	Pattern::Number number;
	if( value.HasMember( "number" ) )
		getValue( value[ "number" ], number );
	out.setNumber( number );

	Pattern::Color color;
	if( value.HasMember( "color" ) )
		getValue( value[ "color" ], color );
	out.setColor( color );

	Pattern::Gravity gravity;
	if( value.HasMember( "gravity" ) )
		getValue( value[ "gravity" ], gravity );
	out.setGravity( gravity );

	vector< Pattern::Emitter > emitters;
	if( value.HasMember( "emitter" ) )
		getValue( value[ "emitter" ], emitters );

	for( Pattern::Emitter emitter : emitters )
		out.addEmitter( emitter );
}

//--------------------------------------------------------------------------------

template< class T >
void getValue( const rapidjson::Value& value, Pattern::Range< T >& out ) {
	if( value.HasMember( "min" ) )
		getValue( value[ "min" ], out.first );
	if( value.HasMember( "max" ) )
		getValue( value[ "max" ], out.second );
}

//--------------------------------------------------------------------------------

template< class T >
void getValue( const rapidjson::Value& value, vector< T >& out ) {
	out.clear();

	if( value.IsArray() ) {
		for( const rapidjson::Value& v : value.GetArray() ) {
			T member;
			getValue( v, member );
			out.push_back( member );
		}
	}
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, bool& out ) {
	if( value.IsBool() )
		out = value.GetBool();
	else if( value.IsInt() )
		out = bool( value.GetInt() );
	else if( value.IsFloat() )
		out = bool( value.GetFloat() );
	else if( value.IsString() )
		out = bool( value.GetString() );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, float& out ) {
	if( value.IsFloat() )
		out = value.GetFloat();
	else if( value.IsInt() )
		out = float( value.GetInt() );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, int& out ) {
	if( value.IsInt() )
		out = value.GetInt();
	else if( value.IsFloat() )
		out = int( value.GetFloat() );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, milliseconds& out ) {
	int count;
	getValue( value, count );
	out = milliseconds( count );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::Vec2& out ) {
	if( value.HasMember( "x" ) )
		getValue( value[ "x" ], out.x );
	if( value.HasMember( "y" ) )
		getValue( value[ "y" ], out.y );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::Color& out ) {
	if( value.HasMember( "red" ) )
		getValue( value[ "red" ], out.r );
	if( value.HasMember( "green" ) )
		getValue( value[ "green" ], out.g );
	if( value.HasMember( "blue" ) )
		getValue( value[ "blue" ], out.b );
	if( value.HasMember( "alpha" ) )
		getValue( value[ "alpha" ], out.a );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Position& out ) {
	if( value.HasMember( "value" ) ) {
		Pattern::Range< Math::Vec2 > range;
		getValue( value[ "value" ], range );
		out.set( range.first, range.second );
	}
	if( value.HasMember( "relative" ) ) {
		bool relative;
		getValue( value[ "relative" ], relative );
		out.setRelative( relative );
	}
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Velocity& out ) {
	if( value.HasMember( "value" ) ) {
		Pattern::Range< Math::Vec2 > range;
		getValue( value[ "value" ], range );
		out.set( range.first, range.second );
	}
	if( value.HasMember( "relative" ) ) {
		bool relative;
		getValue( value[ "relative" ], relative );
		out.setRelative( relative );
	}
	pair< bool, bool > fade{ false, false };
	if( value.HasMember( "fadex" ) )
		getValue( value[ "fadex" ], fade.first );
	if( value.HasMember( "fadey" ) )
		getValue( value[ "fadey" ], fade.second );
	out.setFade( fade.first, fade.second );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Lifetime& out ) {
	if( value.HasMember( "value" ) ) {
		Pattern::Range< milliseconds > range;
		getValue( value[ "value" ], range );
		out.set( range.first, range.second );
	}
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Size& out ) {
	if( value.HasMember( "value" ) ) {
		Pattern::Range< float > range;
		getValue( value[ "value" ], range );
		out.set( range.first, range.second );
	}
	if( value.HasMember( "relative" ) ) {
		bool relative;
		getValue( value[ "relative" ], relative );
		out.setRelative( relative );
	}
	if( value.HasMember( "fade" ) ) {
		bool fade;
		getValue( value[ "fade" ], fade );
		out.setFade( fade );
	}
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Number& out ) {
	if( value.HasMember( "value" ) ) {
		Pattern::Range< int > range;
		getValue( value[ "value" ], range );
		out.set( uint16_t( range.first ), uint16_t( range.second ) );
	}
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Color& out ) {
	Random::RandomColorType type;

	if( value.HasMember( "type" ) ) {
		int t;
		getValue( value[ "type" ], t );
		type = static_cast< Random::RandomColorType >( t );
	}

	if( value.HasMember( "value" ) ) {
		Pattern::Range< Math::Color > range;
		getValue( value[ "value" ], range );
		out.set( range.first, range.second, type );
	}
	if( value.HasMember( "relative" ) ) {
		bool relative;
		getValue( value[ "relative" ], relative );
		out.setRelative( relative );
	}
	if( value.HasMember( "fade" ) ) {
		bool fade;
		getValue( value[ "fade" ], fade );
		out.setFade( fade );
	}
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Gravity& out ) {
	Pattern::Gravity::Type type;

	if( value.HasMember( "type" ) ) {
		int t;
		getValue( value[ "type" ], t );
		type = static_cast< Pattern::Gravity::Type >( t );
	}
	if( value.HasMember( "value" ) ) {
		Pattern::Range< Math::Vec2 > range;
		getValue( value[ "value" ], range );

		if( type == Pattern::Gravity::Type::Direction )
			out.setDirection( range.first, range.second );
		else if( type == Pattern::Gravity::Type::Point )
			out.setPoint( range.first, range.second );
	}
	if( value.HasMember( "power" ) ) {
		Pattern::Range< float > range;
		getValue( value[ "power" ], range );
		out.setPower( range.first, range.second );
	}
	if( value.HasMember( "multiplier" ) ) {
		Pattern::Range< float > range;
		getValue( value[ "multiplier" ], range );
		out.setDistanceMultiplier( range.first, range.second );
	}
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Emitter& out ) {
	if( value.HasMember( "spawnrate" ) ) {
		Pattern::Range< float > range;
		getValue( value[ "spawnrate" ], range );
		out.setSpawnRate( range.first, range.second );
	}
	if( value.HasMember( "delay" ) ) {
		Pattern::Range< milliseconds > range;
		getValue( value[ "delay" ], range );
		out.setDelay( range.first, range.second );
	}
	if( value.HasMember( "patterns" ) ) {
		vector< Pattern > patterns;
		getValue( value[ "patterns" ], patterns );

		for( Pattern pattern : patterns )
			out.addPattern( pattern );
	}
}

//--------------------------------------------------------------------------------

}
}

//================================================================================