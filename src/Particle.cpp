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
	if( set.random )
		set.value = Random::getRandomIntInRange( set.min, set.max );
	else
		set.value = set.min;
}

//--------------------------------------------------------------------------------

void processSet( ValueSet< float >& set ) {
	if( set.random )
		set.value = Random::getRandomFloatInRange( set.min, set.max );
	else
		set.value = set.min;
}

//--------------------------------------------------------------------------------

void processSet( ValueSet< Math::Vec2 >& set ) {
	if( set.random )
		set.value = Random::getRandomVec2InRange( set.min, set.max );
	else
		set.value = set.min;
}

//--------------------------------------------------------------------------------

void processSet( ValueSet< Math::Color >& set ) {
	if( set.random )
		set.value = Random::getRandomColorInRange( set.min, set.max, Random::RandomColorType::MixRGB );
	else
		set.value = set.min;
}

//--------------------------------------------------------------------------------

void processPattern( Pattern& pattern, Pattern::Emitter& emitter ) {
	processSet( emitter.position );
	processSet( emitter.lifetime );
	processSet( emitter.velocity );
	processSet( emitter.acceleration );
	processSet( emitter.size );
	processSet( emitter.alpha );
	processSet( emitter.number );
	
	pattern.initial.position.min += emitter.position.value;
	pattern.initial.position.max += emitter.position.value;

	pattern.initial.lifetime.min *= emitter.lifetime.value;
	pattern.initial.lifetime.max *= emitter.lifetime.value;

	pattern.initial.velocity.min *= emitter.velocity.value;
	pattern.initial.velocity.max *= emitter.velocity.value;

	pattern.initial.acceleration.min *= emitter.acceleration.value;
	pattern.initial.acceleration.max *= emitter.acceleration.value;

	pattern.initial.size.min *= emitter.size.value;
	pattern.initial.size.max *= emitter.size.value;

	pattern.initial.color.min.a *= emitter.alpha.value;
	pattern.initial.color.max.a *= emitter.alpha.value;

	pattern.initial.number.min *= emitter.number.value;
	pattern.initial.number.max *= emitter.number.value;
}

//================================================================================

void Particle::onUpdate( sf::Time deltaTime ) {
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

	// Emitters
	for( auto& emitter : m_pattern.emitters ) {
		if( !emitter.active )
			continue;
		if( emitter.startTime.value == 1.0f )
			continue;
		if( emitter.startTime.value > m_alpha )
			continue;

		if( !Timers::timerStillActive( emitter.timer ) && ( emitter.rate.start.value > 0.0f || emitter.rate.end.value > 0.0f ) ) {
			for( Pattern pattern : emitter.patterns ) {
				processPattern( pattern, emitter );
				spawnParticle( m_parent, pattern, getPosition() );
			}

			processSet( emitter.rate.start );
			processSet( emitter.rate.end );

			float spawnRate = Math::mix( emitter.rate.start.value, emitter.rate.end.value, m_alpha );
			if( spawnRate == 0.0f )
				emitter.active = false;

			milliseconds spawnTime = milliseconds( int( 1000.f / spawnRate ) );
			emitter.timer = Timers::addTimer( spawnTime, nullptr, nullptr, false );
		}
	}
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
}

//--------------------------------------------------------------------------------

void Particle::onDestroy() {
	Timers::removeTimer( m_timer );

	for( auto& emitter : m_pattern.emitters ) {
		Timers::removeTimer( emitter.timer );

		if( !m_active )
			continue;

		if( emitter.startTime.value == 1.0f )
			for( Pattern pattern : emitter.patterns ) {
				processPattern( pattern, emitter );
				spawnParticle( m_parent, pattern, getPosition() );
			}
	}

	m_active = false;
}

//--------------------------------------------------------------------------------

void Particle::init( const Pattern& pattern ) {
	setName( "Bullet" );
	setVisibility( true );

	m_pattern = pattern;

	// Lifetime
	processSet( m_pattern.initial.lifetime );

	milliseconds lifetime = milliseconds( m_pattern.initial.lifetime.value );
	if( lifetime > 0ms ) {
		m_timer = Timers::addTimer( lifetime,
									[this]( float alpha ) { m_alpha = alpha; },
									std::bind( &Particle::destroy, this ),
									false );
	}

	// Position
	processSet( m_pattern.initial.position );
	setPosition( m_pattern.initial.position.value );

	// Direction
	processSet( m_pattern.initial.direction );

	// Velocity
	processSet( m_pattern.initial.velocity );

	setVelocity( m_pattern.initial.direction.value.normalize() * m_pattern.initial.velocity.value );

	// Acceleration
	processSet( m_pattern.initial.acceleration );

	// Size
	processSet( m_pattern.initial.size );
	setSize( m_pattern.initial.size.value );

	// Color
	processSet( m_pattern.initial.color );
	m_shape.setFillColor( m_pattern.initial.color.value.sf() );

	// Emitters
	for( auto& emitter : m_pattern.emitters ) {
		processSet( emitter.rate.start );
		processSet( emitter.rate.end );
		processSet( emitter.startTime );

		if( emitter.startTime.value == .0f ) {
			if( emitter.rate.start.value == .0f )
				for( const Pattern& pattern : emitter.patterns )
					spawnParticle( m_parent, pattern, getPosition() );

			else {
				milliseconds spawnTime = milliseconds( int( 1000.f / emitter.rate.start.value ) );
				emitter.timer = Timers::addTimer( spawnTime, nullptr, nullptr, false );
			}
		}
	}

	// Fades
	processSet( m_pattern.fade.velocity.start );
	processSet( m_pattern.fade.velocity.end );
	processSet( m_pattern.fade.acceleration.start );
	processSet( m_pattern.fade.acceleration.end );
	processSet( m_pattern.fade.size.start );
	processSet( m_pattern.fade.size.end );
	processSet( m_pattern.fade.color.target );
}

//================================================================================

void spawnParticle( Object* parent, Pattern pattern ) {
	processSet( pattern.initial.number );

	for( uint16_t i = 0u; i < pattern.initial.number.value; ++i ) {
		shared_ptr< Particle > particle = Object::makeObject< Particle >( parent );
		particle->init( pattern );
		particles.push_back( particle );
	}
}

//--------------------------------------------------------------------------------

void spawnParticle( Object* parent, Pattern pattern, Math::Vec2 position ) {
	pattern.initial.position.min += position;
	pattern.initial.position.max += position;

	spawnParticle( parent, pattern );
}

//--------------------------------------------------------------------------------

void killParticles() {
	for( shared_ptr< Particle > particle : particles )
		if( particle != nullptr ) {
			particle->deactivate();
			particle->destroy();
		}
}

//--------------------------------------------------------------------------------

void cleanup() {
	for( size_t i = 0u; i < particles.size(); ++i ) {
		if( particles.at( i )->isActive() )
			continue;

		particles.erase( particles.begin() + i );
		--i;
	}
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Particle > > getParticles() {
	return particles;
}

//================================================================================

// Pattern Loader

//================================================================================

rapidjson::MemoryPoolAllocator < rapidjson::CrtAllocator >* allocator;

//--------------------------------------------------------------------------------

// Saving
template< class T >
rapidjson::Value getValue( ValueSet< T > value );
template< class T >
rapidjson::Value getValue( const vector< T >& value );
rapidjson::Value getValue( Math::Vec2 value );
rapidjson::Value getValue( Math::Color value );
rapidjson::Value getValue( int value );
rapidjson::Value getValue( float value );
rapidjson::Value getValue( milliseconds value );
rapidjson::Value getValue( bool value );
rapidjson::Value getValue( string value );

rapidjson::Value getValue( const Pattern& value );

rapidjson::Value getValue( const Pattern::Initial& value );
rapidjson::Value getValue( const Pattern::Emitter& value );
rapidjson::Value getValue( const Pattern::Emitter::SpawnRate& value );
rapidjson::Value getValue( const Pattern::Fade& value );
rapidjson::Value getValue( const Pattern::Fade::Velocity& value );
rapidjson::Value getValue( const Pattern::Fade::Acceleration& value );
rapidjson::Value getValue( const Pattern::Fade::Size& value );
rapidjson::Value getValue( const Pattern::Fade::Color& value );

//--------------------------------------------------------------------------------

// Loading
template< class T >
void getValue( const rapidjson::Value& value, ValueSet< T >& out );
template< class T >
void getValue( const rapidjson::Value& value, vector< T >& out );
void getValue( const rapidjson::Value& value, bool& out );
void getValue( const rapidjson::Value& value, float& out );
void getValue( const rapidjson::Value& value, int& out );
void getValue( const rapidjson::Value& value, milliseconds& out );
void getValue( const rapidjson::Value& value, Math::Vec2& out );
void getValue( const rapidjson::Value& value, Math::Color& out );
void getValue( const rapidjson::Value& value, string& out );

void getValue( const rapidjson::Value& value, Pattern& out );

void getValue( const rapidjson::Value& value, Pattern::Initial& out );
void getValue( const rapidjson::Value& value, Pattern::Emitter& out );
void getValue( const rapidjson::Value& value, Pattern::Emitter::SpawnRate& out );
void getValue( const rapidjson::Value& value, Pattern::Fade& out );
void getValue( const rapidjson::Value& value, Pattern::Fade::Velocity& out );
void getValue( const rapidjson::Value& value, Pattern::Fade::Acceleration& out );
void getValue( const rapidjson::Value& value, Pattern::Fade::Size& out );
void getValue( const rapidjson::Value& value, Pattern::Fade::Color& out );

//================================================================================

// SAVING

//================================================================================

void savePattern( Pattern pattern, string filepath ) {
	using namespace rapidjson;

	Document document;
	document.SetObject();

	allocator = &document.GetAllocator();
	document.CopyFrom( getValue( pattern ), *allocator );

	string path = Folders::Bullets + filepath + ".json";

	FILE* file;
	if( fopen_s( &file, path.c_str(), "wb" ) )
		return;	

	char* writeBuffer = new char[ 65536 ];
	FileWriteStream stream( file, writeBuffer, sizeof( writeBuffer ) );
	PrettyWriter< FileWriteStream > writer( stream );
	writer.SetMaxDecimalPlaces( 2 );
	writer.SetFormatOptions( PrettyFormatOptions::kFormatSingleLineArray );

	document.Accept( writer );

	if( file != NULL )
		fclose( file );

	delete[] writeBuffer;
}

//================================================================================

template< class T >
rapidjson::Value getValue( ValueSet< T > value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "min", getValue( value.min ), *allocator );
	out.AddMember( "max", getValue( value.max ), *allocator );
	out.AddMember( "random", getValue( value.random ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

template< class T >
rapidjson::Value getValue( const vector< T >& value ) {
	rapidjson::Value out;
	out.SetArray();

	for( const T& member : value )
		out.PushBack( getValue( member ), *allocator );

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
	out.SetString( value.c_str(), rapidjson::SizeType( value.length() ), *allocator );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "name", getValue( value.name ), *allocator );
	out.AddMember( "initial", getValue( value.initial ), *allocator );
	out.AddMember( "fade", getValue( value.fade ), *allocator );
	out.AddMember( "emitters", getValue( value.emitters ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Initial& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "lifetime", getValue( value.lifetime ), *allocator );
	out.AddMember( "position", getValue( value.position ), *allocator );
	out.AddMember( "direction", getValue( value.direction ), *allocator );
	out.AddMember( "velocity", getValue( value.velocity ), *allocator );
	out.AddMember( "acceleration", getValue( value.acceleration ), *allocator );
	out.AddMember( "size", getValue( value.size ), *allocator );
	out.AddMember( "number", getValue( value.number ), *allocator );
	out.AddMember( "color", getValue( value.color ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Fade& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "velocity", getValue( value.velocity ), *allocator );
	out.AddMember( "acceleration", getValue( value.acceleration ), *allocator );
	out.AddMember( "size", getValue( value.size ), *allocator );
	out.AddMember( "color", getValue( value.color ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Fade::Velocity& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "x", getValue( value.x ), *allocator );
	out.AddMember( "y", getValue( value.y ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Fade::Acceleration& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "x", getValue( value.x ), *allocator );
	out.AddMember( "y", getValue( value.y ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Fade::Size& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "active", getValue( value.active ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Fade::Color& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "target", getValue( value.target ), *allocator );
	out.AddMember( "r", getValue( value.r ), *allocator );
	out.AddMember( "g", getValue( value.g ), *allocator );
	out.AddMember( "b", getValue( value.b ), *allocator );
	out.AddMember( "a", getValue( value.a ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Emitter& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "lifetime", getValue( value.lifetime ), *allocator );
	out.AddMember( "velocity", getValue( value.velocity ), *allocator );
	out.AddMember( "acceleration", getValue( value.acceleration ), *allocator );
	out.AddMember( "size", getValue( value.size ), *allocator );
	out.AddMember( "alpha", getValue( value.alpha ), *allocator );
	out.AddMember( "number", getValue( value.number ), *allocator );

	out.AddMember( "position", getValue( value.position ), *allocator );
	out.AddMember( "starttime", getValue( value.startTime ), *allocator );
	out.AddMember( "spawnrate", getValue( value.rate ), *allocator );

	out.AddMember( "patterns", getValue( value.patterns ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Pattern::Emitter::SpawnRate& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "fade", getValue( value.fade ), *allocator );

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

	char* buffer = new char[ 65536 ];
	FileReadStream is( pFile, buffer, sizeof( buffer ) );
	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>( is );

	if( pFile != NULL )
		fclose( pFile );

	if( document.HasParseError() ) {
		Debug::addMessage( "Map \"" + name + "\" has JSON parse error", Debug::DebugType::Error );
		return out;
	}

	getValue( document, out );

	delete[] buffer;

	return out;
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern& out ) {

	if( value.HasMember( "name" ) )
		getValue( value[ "name" ], out.name );

	if( value.HasMember( "initial" ) )
		getValue( value[ "initial" ], out.initial );

	if( value.HasMember( "fade" ) )
		getValue( value[ "fade" ], out.fade );

	if( value.HasMember( "emitters" ) )
		getValue( value[ "emitters" ], out.emitters );
}

//--------------------------------------------------------------------------------

template< class T >
void getValue( const rapidjson::Value& value, ValueSet< T >& out ) {
	if( value.HasMember( "min" ) )
		getValue( value[ "min" ], out.min );
	if( value.HasMember( "max" ) )
		getValue( value[ "max" ], out.max );
	if( value.HasMember( "random" ) )
		getValue( value[ "random" ], out.random );
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

void getValue( const rapidjson::Value& value, string& out ) {
	if( value.IsString() )
		out = value.GetString();
	else if( value.IsInt() )
		out = std::to_string( value.GetInt() );
	else if( value.IsFloat() )
		out = std::to_string( value.GetFloat() );
	else if( value.IsBool() )
		if( value.GetBool() )
			out = "true";
		else
			out = "false";
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Initial& out ) {
	if( value.HasMember( "lifetime" ) )
		getValue( value[ "lifetime" ], out.lifetime );
	if( value.HasMember( "position" ) )
		getValue( value[ "position" ], out.position );
	if( value.HasMember( "direction" ) )
		getValue( value[ "direction" ], out.direction );
	if( value.HasMember( "velocity" ) )
		getValue( value[ "velocity" ], out.velocity );
	if( value.HasMember( "acceleration" ) )
		getValue( value[ "acceleration" ], out.acceleration );
	if( value.HasMember( "size" ) )
		getValue( value[ "size" ], out.size );
	if( value.HasMember( "number" ) )
		getValue( value[ "number" ], out.number );
	if( value.HasMember( "color" ) )
		getValue( value[ "color" ], out.color );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Emitter& out ) {
	if( value.HasMember( "name" ) )
		getValue( value[ "name" ], out.name );
	if( value.HasMember( "position" ) )
		getValue( value[ "position" ], out.position );
	if( value.HasMember( "starttime" ) )
		getValue( value[ "starttime" ], out.startTime );
	if( value.HasMember( "spawnrate" ) )
		getValue( value[ "spawnrate" ], out.rate );
	if( value.HasMember( "lifetime" ) )
		getValue( value[ "lifetime" ], out.lifetime );
	if( value.HasMember( "velocity" ) )
		getValue( value[ "velocity" ], out.velocity );
	if( value.HasMember( "acceleration" ) )
		getValue( value[ "acceleration" ], out.acceleration );
	if( value.HasMember( "size" ) )
		getValue( value[ "size" ], out.size );
	if( value.HasMember( "alpha" ) )
		getValue( value[ "alpha" ], out.alpha );
	if( value.HasMember( "number" ) )
		getValue( value[ "number" ], out.number );
	if( value.HasMember( "patterns" ) )
		getValue( value[ "patterns" ], out.patterns );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Emitter::SpawnRate& out ) {
	if( value.HasMember( "start" ) )
		getValue( value[ "start" ], out.start );
	if( value.HasMember( "end" ) )
		getValue( value[ "end" ], out.end );
	if( value.HasMember( "fade" ) )
		getValue( value[ "fade" ], out.fade );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Fade& out ) {
	if( value.HasMember( "velocity" ) )
		getValue( value[ "velocity" ], out.velocity );
	if( value.HasMember( "acceleration" ) )
		getValue( value[ "acceleration" ], out.acceleration );
	if( value.HasMember( "size" ) )
		getValue( value[ "size" ], out.size );
	if( value.HasMember( "color" ) )
		getValue( value[ "color" ], out.color );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Fade::Velocity& out ) {
	if( value.HasMember( "start" ) )
		getValue( value[ "start" ], out.start );
	if( value.HasMember( "end" ) )
		getValue( value[ "end" ], out.end );
	if( value.HasMember( "x" ) )
		getValue( value[ "x" ], out.x );
	if( value.HasMember( "y" ) )
		getValue( value[ "y" ], out.y );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Fade::Acceleration& out ) {
	if( value.HasMember( "start" ) )
		getValue( value[ "start" ], out.start );
	if( value.HasMember( "end" ) )
		getValue( value[ "end" ], out.end );
	if( value.HasMember( "x" ) )
		getValue( value[ "x" ], out.x );
	if( value.HasMember( "y" ) )
		getValue( value[ "y" ], out.y );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Fade::Size& out ) {
	if( value.HasMember( "start" ) )
		getValue( value[ "start" ], out.start );
	if( value.HasMember( "end" ) )
		getValue( value[ "end" ], out.end );
	if( value.HasMember( "active" ) )
		getValue( value[ "active" ], out.active );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Pattern::Fade::Color& out ) {
	if( value.HasMember( "target" ) )
		getValue( value[ "target" ], out.target );
	if( value.HasMember( "r" ) )
		getValue( value[ "r" ], out.r );
	if( value.HasMember( "g" ) )
		getValue( value[ "g" ], out.g );
	if( value.HasMember( "b" ) )
		getValue( value[ "b" ], out.b );
	if( value.HasMember( "a" ) )
		getValue( value[ "a" ], out.a );
}

//--------------------------------------------------------------------------------

}
}

//================================================================================