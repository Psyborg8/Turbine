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

#define saveValue(str) ( out.AddMember( #str, getValue( str ), *allocator ) )
#define loadValue(str) if( json.HasMember(#str) ) getValue( json[#str], str )

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
rapidjson::Value getValue( const Particle::PatternSet& value );
rapidjson::Value getValue( const Emitter::Pattern& value );

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
void getValue( const rapidjson::Value& value, Particle::PatternSet& out );
void getValue( const rapidjson::Value& value, Emitter::Pattern& out );

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
	out.AddMember( "lock", getValue( value.lock ), *allocator );
	out.AddMember( "hsv", getValue( value.hsv ), *allocator );
	out.AddMember( "inverse", getValue( value.inverse ), *allocator );

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

	/* Properties */
	saveValue( value.properties.name );
	saveValue( value.properties.lifetime );
	saveValue( value.properties.number );

	/* Shape */
	saveValue( value.shape.type );

	// Size
	saveValue( value.shape.size.size );
	saveValue( value.shape.size.fade.start );
	saveValue( value.shape.size.fade.end );
	saveValue( value.shape.size.fade.x );
	saveValue( value.shape.size.fade.y );

	// Color
	saveValue( value.shape.color.color );
	saveValue( value.shape.color.fade.target );
	saveValue( value.shape.color.fade.r );
	saveValue( value.shape.color.fade.g );
	saveValue( value.shape.color.fade.b );
	saveValue( value.shape.color.fade.a );

	// Outline
	saveValue( value.shape.outline.color.color );
	saveValue( value.shape.outline.color.fade.target );
	saveValue( value.shape.outline.color.fade.r );
	saveValue( value.shape.outline.color.fade.g );
	saveValue( value.shape.outline.color.fade.b );
	saveValue( value.shape.outline.color.fade.a );

	saveValue( value.shape.outline.thickness.thickness );
	saveValue( value.shape.outline.thickness.fade.start );
	saveValue( value.shape.outline.thickness.fade.end );
	saveValue( value.shape.outline.thickness.fade.active );

	// Texture
	saveValue( value.shape.texture );
	saveValue( value.shape.shader );

	// Origin
	saveValue( value.shape.origin.position.type );
	saveValue( value.shape.origin.position.position );
	saveValue( value.shape.origin.position.circle.radius );
	saveValue( value.shape.origin.position.circle.fill );
	saveValue( value.shape.origin.position.circle.uniform );
	saveValue( value.shape.origin.position.rectangle.size );
	saveValue( value.shape.origin.position.rectangle.fill );
	saveValue( value.shape.origin.position.rectangle.uniform );

	saveValue( value.shape.origin.velocity.type );
	saveValue( value.shape.origin.velocity.direction.direction );
	saveValue( value.shape.origin.velocity.direction.power );
	saveValue( value.shape.origin.velocity.direction.fade.target );
	saveValue( value.shape.origin.velocity.direction.fade.start );
	saveValue( value.shape.origin.velocity.direction.fade.end );
	saveValue( value.shape.origin.velocity.direction.fade.direction );
	saveValue( value.shape.origin.velocity.direction.fade.power );

	saveValue( value.shape.origin.velocity.value.value );
	saveValue( value.shape.origin.velocity.value.fade.target );
	saveValue( value.shape.origin.velocity.value.fade.x );
	saveValue( value.shape.origin.velocity.value.fade.y );

	saveValue( value.shape.origin.velocity.point.point );
	saveValue( value.shape.origin.velocity.point.power );
	saveValue( value.shape.origin.velocity.point.fade.target );
	saveValue( value.shape.origin.velocity.point.fade.start );
	saveValue( value.shape.origin.velocity.point.fade.end );
	saveValue( value.shape.origin.velocity.point.fade.point );
	saveValue( value.shape.origin.velocity.point.fade.power );
	saveValue( value.shape.origin.velocity.point.scale.active );
	saveValue( value.shape.origin.velocity.point.scale.factor );
	saveValue( value.shape.origin.velocity.point.scale.inverse );


	/* Physics */
	// Position
	saveValue( value.physics.position.type );
	saveValue( value.physics.position.position );
	saveValue( value.physics.position.circle.radius );
	saveValue( value.physics.position.circle.fill );
	saveValue( value.physics.position.circle.uniform );
	saveValue( value.physics.position.rectangle.size );
	saveValue( value.physics.position.rectangle.fill );
	saveValue( value.physics.position.rectangle.uniform );

	// Velocity
	saveValue( value.physics.velocity.type );
	saveValue( value.physics.velocity.direction.direction );
	saveValue( value.physics.velocity.direction.power );
	saveValue( value.physics.velocity.direction.fade.target );
	saveValue( value.physics.velocity.direction.fade.start );
	saveValue( value.physics.velocity.direction.fade.end );
	saveValue( value.physics.velocity.direction.fade.direction );
	saveValue( value.physics.velocity.direction.fade.power );
					 
	saveValue( value.physics.velocity.value.value );
	saveValue( value.physics.velocity.value.fade.target );
	saveValue( value.physics.velocity.value.fade.x );
	saveValue( value.physics.velocity.value.fade.y );
					 
	saveValue( value.physics.velocity.point.point );
	saveValue( value.physics.velocity.point.power );
	saveValue( value.physics.velocity.point.fade.target );
	saveValue( value.physics.velocity.point.fade.start );
	saveValue( value.physics.velocity.point.fade.end );
	saveValue( value.physics.velocity.point.fade.point );
	saveValue( value.physics.velocity.point.fade.power );
	saveValue( value.physics.velocity.point.scale.active );
	saveValue( value.physics.velocity.point.scale.factor );
	saveValue( value.physics.velocity.point.scale.inverse );

	// Acceleration
	saveValue( value.physics.acceleration.type );
	saveValue( value.physics.acceleration.direction.direction );
	saveValue( value.physics.acceleration.direction.power );
	saveValue( value.physics.acceleration.direction.fade.target );
	saveValue( value.physics.acceleration.direction.fade.start );
	saveValue( value.physics.acceleration.direction.fade.end );
	saveValue( value.physics.acceleration.direction.fade.direction );
	saveValue( value.physics.acceleration.direction.fade.power );
							 
	saveValue( value.physics.acceleration.value.value );
	saveValue( value.physics.acceleration.value.fade.target );
	saveValue( value.physics.acceleration.value.fade.x );
	saveValue( value.physics.acceleration.value.fade.y );
							 
	saveValue( value.physics.acceleration.point.point );
	saveValue( value.physics.acceleration.point.power );
	saveValue( value.physics.acceleration.point.fade.target );
	saveValue( value.physics.acceleration.point.fade.start );
	saveValue( value.physics.acceleration.point.fade.end );
	saveValue( value.physics.acceleration.point.fade.point );
	saveValue( value.physics.acceleration.point.fade.power );
	saveValue( value.physics.acceleration.point.scale.active );
	saveValue( value.physics.acceleration.point.scale.factor );
	saveValue( value.physics.acceleration.point.scale.inverse );

	// Rotation
	saveValue( value.physics.rotation.rotation );
	saveValue( value.physics.rotation.fade.target );
	saveValue( value.physics.rotation.fade.active );
	
	saveValue( value.physics.rotation.spin.spin );
	saveValue( value.physics.rotation.spin.fade.start );
	saveValue( value.physics.rotation.spin.fade.end );
	saveValue( value.physics.rotation.spin.fade.active );

	/* Inheritance */
	saveValue( value.inheritance.position );
	saveValue( value.inheritance.velocity );
	saveValue( value.inheritance.size );
	saveValue( value.inheritance.alpha );

	/* Emitters*/
	saveValue( value.emitters );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Particle::PatternSet& value ) {
	rapidjson::Value out;
	out.SetObject();

	saveValue( value.random );
	saveValue( value.patterns );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const Emitter::Pattern& value ) {
	rapidjson::Value out;
	out.SetObject();

	saveValue( value.properties.name );
	saveValue( value.properties.position );
	saveValue( value.properties.activation.type );
	saveValue( value.properties.activation.start );
	saveValue( value.properties.activation.end );
	saveValue( value.properties.rate.start );
	saveValue( value.properties.rate.end );
	saveValue( value.properties.rate.fade );

	saveValue( value.multipliers.lifetime );
	saveValue( value.multipliers.velocity );
	saveValue( value.multipliers.acceleration );
	saveValue( value.multipliers.size );
	saveValue( value.multipliers.alpha );
	saveValue( value.multipliers.number );

	saveValue( value.patterns );
	saveValue( value.sets );

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
	Debug::addMessage( "Loading Pattern: " + path, Debug::DebugType::Info );

	FILE* pFile;
	if( fopen_s( &pFile, path.c_str(), "rb" ) ) {
		Debug::addMessage( "Pattern " + path + " doesn't exist.", Debug::DebugType::Error );
		return out;
	}

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
	Debug::addMessage( "Loading Emitter: " + path, Debug::DebugType::Info );

	FILE* pFile;
	if( fopen_s( &pFile, path.c_str(), "rb" ) ) {
		Debug::addMessage( "Emitter " + path + " doesn't exist.", Debug::DebugType::Error );
		return out;
	}

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

template< class T >
void getValue( const rapidjson::Value& value, Math::ValueSet< T >& out ) {
	if( value.HasMember( "min" ) )
		getValue( value[ "min" ], out.min );
	if( value.HasMember( "max" ) )
		getValue( value[ "max" ], out.max );
	if( value.HasMember( "random" ) )
		getValue( value[ "random" ], out.random );
	if( value.HasMember( "lock" ) )
		getValue( value[ "lock" ], out.lock );
	if( value.HasMember( "hsv" ) )
		getValue( value[ "hsv" ], out.hsv );
	if( value.HasMember( "inverse" ) )
		getValue( value[ "inverse" ], out.inverse );
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

void getValue( const rapidjson::Value& json, Particle::Pattern& value ) {
	/* Properties */
	loadValue( value.properties.name );
	loadValue( value.properties.lifetime );
	loadValue( value.properties.number );

	/* Shape */
	if( json.HasMember( "value.shape.type" ) )
		getValue( json[ "value.shape.type" ], ( int& )value.shape.type );

	// Size
	loadValue( value.shape.size.size );
	loadValue( value.shape.size.fade.start );
	loadValue( value.shape.size.fade.end );
	loadValue( value.shape.size.fade.x );
	loadValue( value.shape.size.fade.y );

	// Color
	loadValue( value.shape.color.color );
	loadValue( value.shape.color.fade.target );
	loadValue( value.shape.color.fade.r );
	loadValue( value.shape.color.fade.g );
	loadValue( value.shape.color.fade.b );
	loadValue( value.shape.color.fade.a );

	// Outline
	loadValue( value.shape.outline.color.color );
	loadValue( value.shape.outline.color.fade.target );
	loadValue( value.shape.outline.color.fade.r );
	loadValue( value.shape.outline.color.fade.g );
	loadValue( value.shape.outline.color.fade.b );
	loadValue( value.shape.outline.color.fade.a );

	loadValue( value.shape.outline.thickness.thickness );
	loadValue( value.shape.outline.thickness.fade.start );
	loadValue( value.shape.outline.thickness.fade.end );
	loadValue( value.shape.outline.thickness.fade.active );

	// Texture
	loadValue( value.shape.texture );
	loadValue( value.shape.shader );

	// Origin
	loadValue( value.shape.origin.position.position );
	loadValue( value.shape.origin.position.circle.radius );
	loadValue( value.shape.origin.position.circle.fill );
	loadValue( value.shape.origin.position.circle.uniform );
	loadValue( value.shape.origin.position.rectangle.size );
	loadValue( value.shape.origin.position.rectangle.fill );
	loadValue( value.shape.origin.position.rectangle.uniform );

	if( json.HasMember( "value.shape.origin.velocity.type" ) )
		getValue( json[ "value.shape.origin.velocity.type" ], ( int& )value.shape.origin.velocity.type );

	loadValue( value.shape.origin.velocity.direction.direction );
	loadValue( value.shape.origin.velocity.direction.power );
	loadValue( value.shape.origin.velocity.direction.fade.target );
	loadValue( value.shape.origin.velocity.direction.fade.start );
	loadValue( value.shape.origin.velocity.direction.fade.end );
	loadValue( value.shape.origin.velocity.direction.fade.direction );
	loadValue( value.shape.origin.velocity.direction.fade.power );

	loadValue( value.shape.origin.velocity.value.value );
	loadValue( value.shape.origin.velocity.value.fade.target );
	loadValue( value.shape.origin.velocity.value.fade.x );
	loadValue( value.shape.origin.velocity.value.fade.y );

	loadValue( value.shape.origin.velocity.point.point );
	loadValue( value.shape.origin.velocity.point.power );
	loadValue( value.shape.origin.velocity.point.fade.target );
	loadValue( value.shape.origin.velocity.point.fade.start );
	loadValue( value.shape.origin.velocity.point.fade.end );
	loadValue( value.shape.origin.velocity.point.fade.point );
	loadValue( value.shape.origin.velocity.point.fade.power );
	loadValue( value.shape.origin.velocity.point.scale.active );
	loadValue( value.shape.origin.velocity.point.scale.factor );
	loadValue( value.shape.origin.velocity.point.scale.inverse );


	/* Physics */
	// Position
	if( json.HasMember( "value.physics.position.type" ) )
		getValue( json[ "value.physics.position.type" ], ( int& )value.physics.position.type );

	loadValue( value.physics.position.position );
	loadValue( value.physics.position.circle.radius );
	loadValue( value.physics.position.circle.fill );
	loadValue( value.physics.position.circle.uniform );
	loadValue( value.physics.position.rectangle.size );
	loadValue( value.physics.position.rectangle.fill );
	loadValue( value.physics.position.rectangle.uniform );

	// Velocity
	if( json.HasMember( "value.physics.velocity.type" ) )
		getValue( json[ "value.physics.velocity.type" ], ( int& )value.physics.velocity.type );

	loadValue( value.physics.velocity.direction.direction );
	loadValue( value.physics.velocity.direction.power );
	loadValue( value.physics.velocity.direction.fade.target );
	loadValue( value.physics.velocity.direction.fade.start );
	loadValue( value.physics.velocity.direction.fade.end );
	loadValue( value.physics.velocity.direction.fade.direction );
	loadValue( value.physics.velocity.direction.fade.power );

	loadValue( value.physics.velocity.value.value );
	loadValue( value.physics.velocity.value.fade.target );
	loadValue( value.physics.velocity.value.fade.x );
	loadValue( value.physics.velocity.value.fade.y );

	loadValue( value.physics.velocity.point.point );
	loadValue( value.physics.velocity.point.power );
	loadValue( value.physics.velocity.point.fade.target );
	loadValue( value.physics.velocity.point.fade.start );
	loadValue( value.physics.velocity.point.fade.end );
	loadValue( value.physics.velocity.point.fade.point );
	loadValue( value.physics.velocity.point.fade.power );
	loadValue( value.physics.velocity.point.scale.active );
	loadValue( value.physics.velocity.point.scale.factor );
	loadValue( value.physics.velocity.point.scale.inverse );

	// Acceleration
	if( json.HasMember( "value.physics.acceleration.type" ) )
		getValue( json[ "value.physics.acceleration.type" ], ( int& )value.physics.acceleration.type );

	loadValue( value.physics.acceleration.direction.direction );
	loadValue( value.physics.acceleration.direction.power );
	loadValue( value.physics.acceleration.direction.fade.target );
	loadValue( value.physics.acceleration.direction.fade.start );
	loadValue( value.physics.acceleration.direction.fade.end );
	loadValue( value.physics.acceleration.direction.fade.direction );
	loadValue( value.physics.acceleration.direction.fade.power );

	loadValue( value.physics.acceleration.value.value );
	loadValue( value.physics.acceleration.value.fade.target );
	loadValue( value.physics.acceleration.value.fade.x );
	loadValue( value.physics.acceleration.value.fade.y );

	loadValue( value.physics.acceleration.point.point );
	loadValue( value.physics.acceleration.point.power );
	loadValue( value.physics.acceleration.point.fade.target );
	loadValue( value.physics.acceleration.point.fade.start );
	loadValue( value.physics.acceleration.point.fade.end );
	loadValue( value.physics.acceleration.point.fade.point );
	loadValue( value.physics.acceleration.point.fade.power );
	loadValue( value.physics.acceleration.point.scale.active );
	loadValue( value.physics.acceleration.point.scale.factor );
	loadValue( value.physics.acceleration.point.scale.inverse );

	// Rotation
	loadValue( value.physics.rotation.rotation );
	loadValue( value.physics.rotation.fade.target );
	loadValue( value.physics.rotation.fade.active );

	loadValue( value.physics.rotation.spin.spin );
	loadValue( value.physics.rotation.spin.fade.start );
	loadValue( value.physics.rotation.spin.fade.end );
	loadValue( value.physics.rotation.spin.fade.active );

	/* Inheritance */
	loadValue( value.inheritance.position );
	loadValue( value.inheritance.velocity );
	loadValue( value.inheritance.size );
	loadValue( value.inheritance.alpha );

	/* Emitters*/
	loadValue( value.emitters );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& json, Particle::PatternSet& value ) {
	loadValue( value.random );
	loadValue( value.patterns );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& json, Emitter::Pattern& value ) {
	loadValue( value.properties.name );
	loadValue( value.properties.position );

	if( json.HasMember( "value.properties.activation.type" ) )
		getValue( json[ "value.properties.activation.type" ], ( int& )value.properties.activation.type );

	loadValue( value.properties.activation.start );
	loadValue( value.properties.activation.end );
	loadValue( value.properties.rate.start );
	loadValue( value.properties.rate.end );
	loadValue( value.properties.rate.fade );

	loadValue( value.multipliers.lifetime );
	loadValue( value.multipliers.velocity );
	loadValue( value.multipliers.acceleration );
	loadValue( value.multipliers.size );
	loadValue( value.multipliers.alpha );
	loadValue( value.multipliers.number );

	loadValue( value.patterns );
	loadValue( value.sets );
}

//--------------------------------------------------------------------------------

}

//================================================================================