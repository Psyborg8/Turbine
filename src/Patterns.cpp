//================================================================================

#include "Patterns.h"

//--------------------------------------------------------------------------------

#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

#include <filesystem>

#include "Debug.h"

//================================================================================

namespace Gfx {

rapidjson::MemoryPoolAllocator < rapidjson::CrtAllocator >* allocator;

//--------------------------------------------------------------------------------

// Saving
template< class T >
rapidjson::Value getValue( Math::ValueSet< T > value );
template< class T >
rapidjson::Value getValue( const vector< T >& value );
rapidjson::Value getValue( Math::Vec2 value );
rapidjson::Value getValue( Math::Color value );
rapidjson::Value getValue( int value );
rapidjson::Value getValue( float value );
rapidjson::Value getValue( milliseconds value );
rapidjson::Value getValue( bool value );
rapidjson::Value getValue( string value );

rapidjson::Value getValue( const Particle::Pattern& value );

rapidjson::Value getValue( const Particle::Pattern::Initial& value );
rapidjson::Value getValue( const Emitter::Pattern& value );
rapidjson::Value getValue( const Emitter::Pattern::SpawnRate& value );
rapidjson::Value getValue( const Particle::Pattern::Fade& value );
rapidjson::Value getValue( const Particle::Pattern::Fade::Velocity& value );
rapidjson::Value getValue( const Particle::Pattern::Fade::Acceleration& value );
rapidjson::Value getValue( const Particle::Pattern::Fade::Size& value );
rapidjson::Value getValue( const Particle::Pattern::Fade::Color& value );
rapidjson::Value getValue( const Particle::PatternSet& value );

//--------------------------------------------------------------------------------

// Loading
template< class T >
void getValue( const rapidjson::Value& value, Math::ValueSet< T >& out );
template< class T >
void getValue( const rapidjson::Value& value, vector< T >& out );
void getValue( const rapidjson::Value& value, bool& out );
void getValue( const rapidjson::Value& value, float& out );
void getValue( const rapidjson::Value& value, int& out );
void getValue( const rapidjson::Value& value, milliseconds& out );
void getValue( const rapidjson::Value& value, Math::Vec2& out );
void getValue( const rapidjson::Value& value, Math::Color& out );
void getValue( const rapidjson::Value& value, string& out );

void getValue( const rapidjson::Value& value, Particle::Pattern& out );

void getValue( const rapidjson::Value& value, Particle::Pattern::Initial& out );
void getValue( const rapidjson::Value& value, Emitter::Pattern& out );
void getValue( const rapidjson::Value& value, Emitter::Pattern::SpawnRate& out );
void getValue( const rapidjson::Value& value, Particle::Pattern::Fade& out );
void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Velocity& out );
void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Acceleration& out );
void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Size& out );
void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Color& out );
void getValue( const rapidjson::Value& value, Particle::PatternSet& out );

//================================================================================

// SAVING

//================================================================================

void savePattern( Particle::Pattern pattern, string path ) {
	using namespace rapidjson;

	Document document;
	document.SetObject();

	allocator = &document.GetAllocator();
	document.CopyFrom( getValue( pattern ), *allocator );

	string filepath = Folders::Bullets + path + ".bullet";

	FILE* file;
	if( fopen_s( &file, filepath.c_str(), "wb" ) )
		return;

	char* buffer = new char[ 65536 ];
	FileWriteStream stream( file, buffer, sizeof( buffer ) );
	PrettyWriter< FileWriteStream > writer( stream );
	writer.SetMaxDecimalPlaces( 2 );
	writer.SetFormatOptions( PrettyFormatOptions::kFormatSingleLineArray );

	document.Accept( writer );

	if( file != NULL )
		fclose( file );

	delete[] buffer;
}

//--------------------------------------------------------------------------------

void saveEmitter( Emitter::Pattern pattern, string path ) {
	using namespace rapidjson;

	Document document;
	document.SetObject();

	allocator = &document.GetAllocator();
	document.CopyFrom( getValue( pattern ), *allocator );

	string filepath = Folders::Bullets + path + ".emitter";

	FILE* file;
	if( fopen_s( &file, filepath.c_str(), "wb" ) )
		return;

	char* buffer = new char[ 65536 ];
	FileWriteStream stream( file, buffer, sizeof( buffer ) );
	PrettyWriter< FileWriteStream > writer( stream );
	writer.SetMaxDecimalPlaces( 2 );
	writer.SetFormatOptions( PrettyFormatOptions::kFormatSingleLineArray );

	document.Accept( writer );

	if( file != NULL )
		fclose( file );

	delete[] buffer;
}

//================================================================================

template< class T >
rapidjson::Value getValue( Math::ValueSet< T > value ) {
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

rapidjson::Value getValue( const Particle::Pattern& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "name", getValue( value.name ), *allocator );
	out.AddMember( "initial", getValue( value.initial ), *allocator );
	out.AddMember( "fade", getValue( value.fade ), *allocator );
	out.AddMember( "emitters", getValue( value.emitters ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Particle::Pattern::Initial& value ) {
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

rapidjson::Value getValue( const Particle::Pattern::Fade& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "velocity", getValue( value.velocity ), *allocator );
	out.AddMember( "acceleration", getValue( value.acceleration ), *allocator );
	out.AddMember( "size", getValue( value.size ), *allocator );
	out.AddMember( "color", getValue( value.color ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Particle::Pattern::Fade::Velocity& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "x", getValue( value.x ), *allocator );
	out.AddMember( "y", getValue( value.y ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Particle::Pattern::Fade::Acceleration& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "x", getValue( value.x ), *allocator );
	out.AddMember( "y", getValue( value.y ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Particle::Pattern::Fade::Size& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "active", getValue( value.active ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Particle::Pattern::Fade::Color& value ) {
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

rapidjson::Value getValue( const Emitter::Pattern& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "lifetime", getValue( value.multipliers.lifetime ), *allocator );
	out.AddMember( "velocity", getValue( value.multipliers.velocity ), *allocator );
	out.AddMember( "acceleration", getValue( value.multipliers.acceleration ), *allocator );
	out.AddMember( "size", getValue( value.multipliers.size ), *allocator );
	out.AddMember( "alpha", getValue( value.multipliers.alpha ), *allocator );
	out.AddMember( "number", getValue( value.multipliers.number ), *allocator );

	out.AddMember( "position", getValue( value.position ), *allocator );
	out.AddMember( "activationtype", getValue( static_cast< int >( value.activation.type ) ), *allocator );
	out.AddMember( "starttime", getValue( value.activation.start ), *allocator );
	out.AddMember( "endtime", getValue( value.activation.end ), *allocator );
	out.AddMember( "spawnrate", getValue( value.rate ), *allocator );

	out.AddMember( "patterns", getValue( value.patterns ), *allocator );
	out.AddMember( "patternsets", getValue( value.patterns ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Emitter::Pattern::SpawnRate& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", getValue( value.start ), *allocator );
	out.AddMember( "end", getValue( value.end ), *allocator );
	out.AddMember( "fade", getValue( value.fade ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Particle::PatternSet& value ) {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "random", getValue( value.random ), *allocator );
	out.AddMember( "patterns", getValue( value.patterns ), *allocator );

	return out;
}

//================================================================================

// LOADING

//================================================================================

Particle::Pattern loadPattern( string name ) {
	using namespace rapidjson;

	Particle::Pattern out;

	// Parse JSON
	string path = Folders::Bullets + name + ".bullet";
	FILE* pFile;
	if( fopen_s( &pFile, path.c_str(), "rb" ) )
		return out;

	char* buffer = new char[ 65536 ];
	FileReadStream is( pFile, buffer, sizeof( buffer ) );
	Document document;
	document.ParseStream< 0, UTF8<>, FileReadStream >( is );
	delete[] buffer;

	if( pFile != NULL )
		fclose( pFile );

	if( document.HasParseError() ) {
		Debug::addMessage( "Map \"" + name + "\" has JSON parse error", Debug::DebugType::Error );
		return out;
	}

	getValue( document, out );

	return out;
}

//--------------------------------------------------------------------------------

Emitter::Pattern loadEmitter( string name ) {
	using namespace rapidjson;

	Emitter::Pattern out;

	// Parse JSON
	string path = Folders::Bullets + name + ".emitter";
	FILE* pFile;
	if( fopen_s( &pFile, path.c_str(), "rb" ) )
		return out;

	char* buffer = new char[ 65536 ];
	FileReadStream is( pFile, buffer, sizeof( buffer ) );
	Document document;
	document.ParseStream< 0, UTF8<>, FileReadStream >( is );
	delete[] buffer;

	if( pFile != NULL )
		fclose( pFile );

	if( document.HasParseError() ) {
		Debug::addMessage( "Emitter \"" + name + "\" has JSON parse error", Debug::DebugType::Error );
		return out;
	}

	getValue( document, out );

	return out;
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Particle::Pattern& out ) {

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
void getValue( const rapidjson::Value& value, Math::ValueSet< T >& out ) {
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

void getValue( const rapidjson::Value& value, Particle::Pattern::Initial& out ) {
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

void getValue( const rapidjson::Value& value, Emitter::Pattern& out ) {
	if( value.HasMember( "name" ) )
		getValue( value[ "name" ], out.name );
	if( value.HasMember( "position" ) )
		getValue( value[ "position" ], out.position );
	if( value.HasMember( "activationtype" ) ) {
		int type;
		getValue( value[ "activationtype" ], type );
		out.activation.type = static_cast< Emitter::Pattern::Activation::Type >( type );
	}
	if( value.HasMember( "starttime" ) )
		getValue( value[ "starttime" ], out.activation.start );
	if( value.HasMember( "endtime" ) )
		getValue( value[ "endtime" ], out.activation.end );
	if( value.HasMember( "spawnrate" ) )
		getValue( value[ "spawnrate" ], out.rate );
	if( value.HasMember( "lifetime" ) )
		getValue( value[ "lifetime" ], out.multipliers.lifetime );
	if( value.HasMember( "velocity" ) )
		getValue( value[ "velocity" ], out.multipliers.velocity );
	if( value.HasMember( "acceleration" ) )
		getValue( value[ "acceleration" ], out.multipliers.acceleration );
	if( value.HasMember( "size" ) )
		getValue( value[ "size" ], out.multipliers.size );
	if( value.HasMember( "alpha" ) )
		getValue( value[ "alpha" ], out.multipliers.alpha );
	if( value.HasMember( "number" ) )
		getValue( value[ "number" ], out.multipliers.number );
	if( value.HasMember( "patterns" ) )
		getValue( value[ "patterns" ], out.patterns );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Emitter::Pattern::SpawnRate& out ) {
	if( value.HasMember( "start" ) )
		getValue( value[ "start" ], out.start );
	if( value.HasMember( "end" ) )
		getValue( value[ "end" ], out.end );
	if( value.HasMember( "fade" ) )
		getValue( value[ "fade" ], out.fade );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Particle::Pattern::Fade& out ) {
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

void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Velocity& out ) {
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

void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Acceleration& out ) {
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

void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Size& out ) {
	if( value.HasMember( "start" ) )
		getValue( value[ "start" ], out.start );
	if( value.HasMember( "end" ) )
		getValue( value[ "end" ], out.end );
	if( value.HasMember( "active" ) )
		getValue( value[ "active" ], out.active );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Particle::Pattern::Fade::Color& out ) {
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

void getValue( const rapidjson::Value& value, Particle::PatternSet& out ) {
	if( value.HasMember( "random" ) )
		getValue( value[ "random" ], out.random );
	if( value.HasMember( "patterns" ) )
		getValue( value[ "patterns" ], out.patterns );
}

//--------------------------------------------------------------------------------

}

//================================================================================