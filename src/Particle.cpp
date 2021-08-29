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
	m_shape.setPosition( getWorldPosition().sf() );
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
}

//--------------------------------------------------------------------------------

void Particle::init( const Pattern& pattern ) {
	setName( "Bullet" );
	setVisibility( true );
	m_shape.setOrigin( 0.5f, 0.5f );

	// Lifetime
	if( pattern.lifetime.type == ValueType::Static ) {
		m_lifetime = milliseconds( pattern.lifetime.min );
	}
	else if( pattern.lifetime.type == ValueType::Random ) {
		m_lifetime = milliseconds( Random::getRandomIntInRange( pattern.lifetime.min, pattern.lifetime.max ) );
	}

	// Position
	if( pattern.position.type == ValueType::Static ) {
		setPosition( Math::Vec2( pattern.position.min ) );
	}
	else if( pattern.position.type == ValueType::Random ) {
		setPosition( Random::getRandomVec2InRange( Math::Vec2( pattern.position.min ), Math::Vec2( pattern.position.max ) ) );
	}

	// Direction
	if( pattern.direction.type == ValueType::Static ) {
		m_direction.first = m_direction.second = Math::Vec2( pattern.direction.startMin ).normalize();
	}
	else if( pattern.direction.type == ValueType::Random ) {
		m_direction.first = m_direction.second = Random::getRandomVec2InRange( Math::Vec2( pattern.direction.startMin ), Math::Vec2( pattern.direction.startMax ) ).normalize();
	}
	else if( pattern.direction.type == ValueType::Fade ) {
		if( pattern.direction.startType == ValueType::Static ) {
			m_direction.first = Math::Vec2( pattern.direction.startMin ).normalize();
		}
		else if( pattern.direction.startType == ValueType::Random ) {
			m_direction.first = Random::getRandomVec2InRange( Math::Vec2( pattern.direction.startMin ), Math::Vec2( pattern.direction.startMax ) ).normalize();
		}

		if( pattern.direction.endType == ValueType::Static ) {
			m_direction.second = Math::Vec2( pattern.direction.endMin ).normalize();
		}
		else if( pattern.direction.endType == ValueType::Random ) {
			m_direction.second = Random::getRandomVec2InRange( Math::Vec2( pattern.direction.endMin ), Math::Vec2( pattern.direction.endMax ) ).normalize();
		}
	}

	// Velocity
	if( pattern.velocity.type == ValueType::Static ) {
		m_power.first = m_power.second = pattern.velocity.startMin;
	}
	else if( pattern.velocity.type == ValueType::Random ) {
		m_power.first = m_power.second = Random::getRandomFloatInRange( pattern.velocity.startMin, pattern.velocity.startMax );
	}
	else if( pattern.velocity.type == ValueType::Fade ) {
		if( pattern.velocity.startType == ValueType::Static ) {
			m_power.first = pattern.velocity.startMin;
		}
		else if( pattern.velocity.startType == ValueType::Random ) {
			m_power.first = Random::getRandomFloatInRange( pattern.velocity.startMin, pattern.velocity.startMax );
		}

		if( pattern.velocity.endType == ValueType::Static ) {
			m_power.second = pattern.velocity.endMin;
		}
		else if( pattern.velocity.endType == ValueType::Random ) {
			m_power.first = Random::getRandomFloatInRange( pattern.velocity.endMin, pattern.velocity.endMax );
		}
	}

	setVelocity( m_direction.first * m_power.first );

	// Size
	if( pattern.size.type == ValueType::Static ) {
		m_size.first = m_size.second = pattern.size.startMin;
	}
	else if( pattern.size.type == ValueType::Random ) {
		m_size.first = m_size.second = Random::getRandomFloatInRange( pattern.size.startMin, pattern.size.startMax );
	}
	else if( pattern.size.type == ValueType::Fade ) {
		if( pattern.size.startType == ValueType::Static ) {
			m_size.first = pattern.size.startMin;
		}
		else if( pattern.size.startType == ValueType::Random ) {
			m_size.first = Random::getRandomFloatInRange( pattern.size.startMin, pattern.size.startMax );
		}

		if( pattern.size.endType == ValueType::Static ) {
			m_size.second = pattern.velocity.endMin;
		}
		else if( pattern.size.endType == ValueType::Random ) {
			m_size.first = Random::getRandomFloatInRange( pattern.size.endMin, pattern.size.endMax );
		}
	}

	setSize( m_size.first );

	// Color
	if( pattern.color.type == ValueType::Static ) {
		m_color.first = m_color.second = Math::Color( pattern.color.startMin );
	}
	else if( pattern.color.type == ValueType::Random ) {
		m_color.first = m_color.second = Random::getRandomColorInRange( Math::Color( pattern.color.startMin ), Math::Color( pattern.color.startMax ), pattern.color.randomType );
	}
	else if( pattern.color.type == ValueType::Fade ) {
		if( pattern.color.startType == ValueType::Static ) {
			m_color.first = Math::Color( pattern.color.startMin );
		}
		else if( pattern.color.startType == ValueType::Random ) {
			m_color.first = Random::getRandomColorInRange( Math::Color( pattern.color.startMin ), Math::Color( pattern.color.startMax ), pattern.color.startRandomType );
		}

		if( pattern.color.endType == ValueType::Static ) {
			m_color.second = Math::Color( pattern.color.endMin );
		}
		else if( pattern.color.endType == ValueType::Random ) {
			m_color.second = Random::getRandomColorInRange( Math::Color( pattern.color.endMin ), Math::Color( pattern.color.endMax ), pattern.color.endRandomType );
		}
	}

	m_shape.setFillColor( m_color.first.sf() );

	m_timer = Timers::addTimer( m_lifetime, 
								[this, pattern]( float alpha ) {
									// Direction
									Math::Vec2 direction = m_direction.first;
									if( pattern.direction.type == ValueType::Fade ) {
										if( pattern.direction.fade.first ) {
											direction.x = m_direction.first.x + ( m_direction.second.x - m_direction.first.x ) * alpha;
										}
										if( pattern.direction.fade.second ) {
											direction.y = m_direction.first.y + ( m_direction.second.y - m_direction.first.y ) * alpha;
										}
									}

									// Velocity
									float power = m_power.first;
									if( pattern.velocity.type == ValueType::Fade ) {
										power = m_power.first + ( m_power.second - m_power.first ) * alpha;
									}

									Math::Vec2 velocity = getVelocity();
									Math::Vec2 newVelocity = direction.normalize() * power;
									if( pattern.velocity.fade.first ) {
										velocity.x = newVelocity.x;
									}
									if( pattern.velocity.fade.second ) {
										velocity.y = newVelocity.y;
									}

									if( velocity == getVelocity() ) {
										velocity = direction.normalize() * velocity.length();
									}

									setVelocity( velocity );

									// Size
									if( pattern.size.type == ValueType::Fade ) {
										setSize( m_size.first + ( m_size.second - m_size.first ) * alpha );
									}

									// Color
									if( pattern.color.type == ValueType::Fade ) {
										Math::Color color = Math::Color( m_shape.getFillColor() );

										if( pattern.color.fadeR ) {
											color.r = m_color.first.r + ( m_color.second.r - m_color.first.r ) * alpha;
										}
										if( pattern.color.fadeG ) {
											color.g = m_color.first.g + ( m_color.second.g - m_color.first.g ) * alpha;
										}
										if( pattern.color.fadeB ) {
											color.b = m_color.first.b + ( m_color.second.b - m_color.first.b ) * alpha;
										}
										if( pattern.color.fadeA ) {
											color.a = m_color.first.a + ( m_color.second.a - m_color.first.a ) * alpha;
										}

										m_shape.setFillColor( color.sf() );
									}
								},
								std::bind( &Particle::destroy, this ),
								false );
}

//================================================================================

void spawnParticle( Object* parent, Pattern pattern ) {
	uint16_t count;
	if( pattern.number.type == ValueType::Static ) {
		count = uint16_t( pattern.number.min );
	}
	else if( pattern.number.type == ValueType::Random ) {
		count = uint16_t( Random::getRandomIntInRange( pattern.number.min, pattern.number.max ) );
	}

	for( uint16_t i = 0u; i < count; ++i ) {
		shared_ptr< Particle > particle = Object::makeObject< Particle >( parent );
		particle->init( pattern );
	}
}

//--------------------------------------------------------------------------------

void spawnParticle( Object* parent, Pattern pattern, Math::Vec2 position ) {
	pattern.position.min[ 0 ] += position.x;
	pattern.position.min[ 1 ] += position.y;
	pattern.position.max[ 0 ] += position.x;
	pattern.position.max[ 1 ] += position.y;

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