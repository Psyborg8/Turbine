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

void processSet( ValueSet< int >& set ) {
	set.value = Random::getRandomIntInRange( set.min, set.max );
}

//--------------------------------------------------------------------------------

void processSet( ValueSet< float >& set ) {
	set.value = Random::getRandomFloatInRange( set.min, set.max );
}

//--------------------------------------------------------------------------------

void processSet( ValueSet< Math::Vec2 >& set ) {
	set.value = Random::getRandomVec2InRange( set.min, set.max );
}

//--------------------------------------------------------------------------------

void processSet( ValueSet< Math::Color >& set ) {
	set.value = Random::getRandomColorInRange( set.min, set.max, Random::RandomColorType::ShuffleHSV );
}

//================================================================================

void Particle::onUpdate( sf::Time deltaTime ) {
	m_shape.setPosition( getWorldPosition().sf() );

	// Emitters
	for( pair< Emitter, float >& emitter : m_pattern.emitters ) {
		if( emitter.second == 1.0f )
			continue;

		if( emitter.first.spawnTimer && !Timers::timerStillActive( emitter.first.spawnTimer ) ) {
			for( const Pattern& pattern : emitter.first.patterns )
				spawnParticle( this, pattern );

			processSet( emitter.first.spawnRate.start );
			processSet( emitter.first.spawnRate.end );

			float spawnRate = Math::mix( emitter.first.spawnRate.start.value, emitter.first.spawnRate.end.value, m_alpha );
			milliseconds spawnTime = milliseconds( int( 1000.f / spawnRate ) );
			emitter.first.spawnTimer = Timers::addTimer( spawnTime, nullptr, nullptr, false );
		}
	}
}

//--------------------------------------------------------------------------------

void Particle::onPostUpdate( sf::Time deltaTime ) {
	//
}

//--------------------------------------------------------------------------------

void Particle::onRender() {
	System::getWindow()->draw( m_shape );
}

//--------------------------------------------------------------------------------

void Particle::onDestroy() {
	Timers::removeTimer( m_timer );

	for( const Pattern::Emitter& emitter : m_pattern.emitters ) {
		if( emitter.type == EmitterType::End )
			for( const Pattern& pattern : emitter.patterns ) {
				spawnParticle( this, pattern );
			}

		Timers::removeTimer( emitter.spawnTimer );
	}
}

//--------------------------------------------------------------------------------

void Particle::init( const Pattern& pattern ) {
	setName( "Bullet" );
	setVisibility( true );
	m_shape.setOrigin( 0.5f, 0.5f );

	m_pattern = pattern;

	// Lifetime
	processSet( m_pattern.lifetime );

	// Position
	processSet( m_pattern.position );
	setPosition( m_pattern.position.value.start );

	// Direction
	processSet( m_pattern.direction );

	// Velocity
	processSet( m_pattern.velocity );

	setVelocity( m_pattern.direction.value.start.normalize() * m_pattern.velocity.value.start );

	// Size
	processSet( m_pattern.size );
	setSize( m_pattern.size.value.start );

	// Color
	processSet( m_pattern.color, pattern.color.types );
	m_shape.setFillColor( m_pattern.color.value.start.sf() );

	// Lifetime timer
	milliseconds lifetime = milliseconds( m_pattern.lifetime.value.start );
	if( lifetime > 0ms ) {
		m_timer = Timers::addTimer( lifetime,
									[this]( float alpha ) { m_alpha = alpha; },
									std::bind( &Particle::destroy, this ),
									false );
	}

	// Start Emitters
	for( Pattern::Emitter& emitter : m_pattern.emitters ) {
		processSet( emitter.time );
		processSet( emitter.spawnRate );

		if( emitter.type == EmitterType::Start ) {
			if( emitter.spawnRate.value.start == .0f )
				for( const Pattern& pattern : emitter.patterns )
					spawnParticle( this, pattern );

			else {
				milliseconds spawnTime = milliseconds( int( 1000.f / emitter.spawnRate.value.start ) );
				emitter.spawnTimer = Timers::addTimer( spawnTime, nullptr, nullptr, false );
			}
		}
	}
}

//================================================================================

void spawnParticle( Object* parent, Pattern pattern ) {
	uint16_t count = uint16_t( pattern.number.min );
	if( pattern.number.type == ValueType::Random ) {
		count = uint16_t( Random::getRandomIntInRange( pattern.number.min, pattern.number.max ) );
	}

	for( uint16_t i = 0u; i < count; ++i ) {
		shared_ptr< Particle > particle = Object::makeObject< Particle >( parent );
		particle->init( pattern );
	}
}

//--------------------------------------------------------------------------------

void spawnParticle( Object* parent, Pattern pattern, Math::Vec2 position ) {
	pattern.position.start.min += position;
	pattern.position.start.max += position;

	spawnParticle( parent, pattern );
}

//================================================================================

// Pattern Loader

//================================================================================

/*

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

*/

}
}

//================================================================================