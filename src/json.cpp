//================================================================================

#include "json.h"

//--------------------------------------------------------------------------------

#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>

#include <filesystem>

#include "debug.h"
#include "string-utils.h"

//================================================================================

namespace json {

//--------------------------------------------------------------------------------

rapidjson::MemoryPoolAllocator< rapidjson::CrtAllocator >* allocator;

//--------------------------------------------------------------------------------

bool save( function< rapidjson::Value() > func, string path ) {
	using namespace rapidjson;

	if( !std::filesystem::exists( std::filesystem::path( path ) ) ) {

		vector< string > folders = Utils::tokenize( path, "/\\" );
		folders.erase( folders.end() - 1u );

		string dir;
		for( string folder : folders ) {
			dir += Utils::format( "%s/", folder.c_str() );
			if( !std::filesystem::exists( std::filesystem::path( dir ) ) )
				std::filesystem::create_directory( std::filesystem::path( dir ) );
		}
	}

	Document document;
	allocator = &document.GetAllocator();
	document.CopyFrom( func(), *allocator );

	FILE* file;
	if( fopen_s( &file, path.c_str(), "wb" ) ) {
		Debug::addMessage( Utils::format( "File %s failed to open", path.c_str() ), DebugType::Error );
		return false;
	}

	char writeBuffer[ 65536 ];
	FileWriteStream stream( file, writeBuffer, sizeof( writeBuffer ) );
	PrettyWriter< FileWriteStream > writer( stream );
	writer.SetMaxDecimalPlaces( 2 );
	writer.SetFormatOptions( PrettyFormatOptions::kFormatSingleLineArray );

	document.Accept( writer );

	if( document.HasParseError() ) {
		Debug::addMessage( Utils::format( "File %s has parse error", path.c_str() ), DebugType::Error );
		return false;
	}

	fclose( file );
	return true;
}

//--------------------------------------------------------------------------------

bool load( function< bool( const rapidjson::Value& ) > func, string path ) {
	using namespace rapidjson;

	// Parse JSON
	path;
	FILE* pFile;
	if( fopen_s( &pFile, path.c_str(), "rb" ) )
		return false;

	char* buffer = new char[ 65536 ];
	FileReadStream is( pFile, buffer, sizeof( buffer ) );
	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>( is );

	if( pFile != NULL )
		fclose( pFile );

	if( document.HasParseError() ) {
		Debug::addMessage( "Document \"" + path + "\" has JSON parse error", DebugType::Error );
		return false;
	}

	delete[] buffer;

	return func( document );
}

//--------------------------------------------------------------------------------

void setAllocator( rapidjson::MemoryPoolAllocator< rapidjson::CrtAllocator >* _allocator ) {
	allocator = _allocator;
}

//--------------------------------------------------------------------------------

rapidjson::MemoryPoolAllocator< rapidjson::CrtAllocator >& getAllocator() {
	return *allocator;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( Math::ValueSet< int > value ) {
	rapidjson::Value out;

	out.SetObject();

	out.AddMember( "min", getValue( value.min ), *allocator );
	out.AddMember( "max", getValue( value.max ), *allocator );
	out.AddMember( "hsv", getValue( value.hsv ), *allocator );
	out.AddMember( "lock", getValue( value.lock ), *allocator );
	out.AddMember( "inverse", getValue( value.inverse ), *allocator );
	out.AddMember( "random", getValue( value.random ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( Math::ValueSet< float > value ) {
	rapidjson::Value out;

	out.SetObject();

	out.AddMember( "min", getValue( value.min ), *allocator );
	out.AddMember( "max", getValue( value.max ), *allocator );
	out.AddMember( "hsv", getValue( value.hsv ), *allocator );
	out.AddMember( "lock", getValue( value.lock ), *allocator );
	out.AddMember( "inverse", getValue( value.inverse ), *allocator );
	out.AddMember( "random", getValue( value.random ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( Math::ValueSet< Math::Vec2 > value ) {
	rapidjson::Value out;

	out.SetObject();

	out.AddMember( "min", getValue( value.min ), *allocator );
	out.AddMember( "max", getValue( value.max ), *allocator );
	out.AddMember( "hsv", getValue( value.hsv ), *allocator );
	out.AddMember( "lock", getValue( value.lock ), *allocator );
	out.AddMember( "inverse", getValue( value.inverse ), *allocator );
	out.AddMember( "random", getValue( value.random ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( Math::ValueSet< Math::Color > value ) {
	rapidjson::Value out;

	out.SetObject();

	out.AddMember( "min", getValue( value.min ), *allocator );
	out.AddMember( "max", getValue( value.max ), *allocator );
	out.AddMember( "hsv", getValue( value.hsv ), *allocator );
	out.AddMember( "lock", getValue( value.lock ), *allocator );
	out.AddMember( "inverse", getValue( value.inverse ), *allocator );
	out.AddMember( "random", getValue( value.random ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( bool value ) {
	rapidjson::Value out;
	out.SetBool( value );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( float value ) {
	rapidjson::Value out;
	out.SetFloat( value );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( int value ) {
	rapidjson::Value out;
	out.SetInt( value );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( size_t value ) {
	rapidjson::Value out;
	out.SetUint64( value );
	return out;
}

//--------------------------------------------------------------------------------

rapidjson::Value getValue( const string& value ) {
	rapidjson::Value out;
	out.SetString( value.c_str(), rapidjson::SizeType( value.length() ), *allocator );
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

	out.AddMember( "r", getValue( value.r ), *allocator );
	out.AddMember( "g", getValue( value.g ), *allocator );
	out.AddMember( "b", getValue( value.b ), *allocator );
	out.AddMember( "a", getValue( value.a ), *allocator );

	return out;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::ValueSet< int >& out ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "min" ) )
		getValue( value[ "min" ], out.min );
	if( value.HasMember( "max" ) )
		getValue( value[ "max" ], out.max );
	if( value.HasMember( "random" ) )
		getValue( value[ "random" ], out.random );
	if( value.HasMember( "lock" ) )
		getValue( value[ "lock" ], out.lock );
	if( value.HasMember( "inverse" ) )
		getValue( value[ "inverse" ], out.inverse );
	if( value.HasMember( "hsv" ) )
		getValue( value[ "hsv" ], out.hsv );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::ValueSet< float >& out ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "min" ) )
		getValue( value[ "min" ], out.min );
	if( value.HasMember( "max" ) )
		getValue( value[ "max" ], out.max );
	if( value.HasMember( "random" ) )
		getValue( value[ "random" ], out.random );
	if( value.HasMember( "lock" ) )
		getValue( value[ "lock" ], out.lock );
	if( value.HasMember( "inverse" ) )
		getValue( value[ "inverse" ], out.inverse );
	if( value.HasMember( "hsv" ) )
		getValue( value[ "hsv" ], out.hsv );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::ValueSet< Math::Vec2 > & out ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "min" ) )
		getValue( value[ "min" ], out.min );
	if( value.HasMember( "max" ) )
		getValue( value[ "max" ], out.max );
	if( value.HasMember( "random" ) )
		getValue( value[ "random" ], out.random );
	if( value.HasMember( "lock" ) )
		getValue( value[ "lock" ], out.lock );
	if( value.HasMember( "inverse" ) )
		getValue( value[ "inverse" ], out.inverse );
	if( value.HasMember( "hsv" ) )
		getValue( value[ "hsv" ], out.hsv );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::ValueSet< Math::Color >& out ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "min" ) )
		getValue( value[ "min" ], out.min );
	if( value.HasMember( "max" ) )
		getValue( value[ "max" ], out.max );
	if( value.HasMember( "random" ) )
		getValue( value[ "random" ], out.random );
	if( value.HasMember( "lock" ) )
		getValue( value[ "lock" ], out.lock );
	if( value.HasMember( "inverse" ) )
		getValue( value[ "inverse" ], out.inverse );
	if( value.HasMember( "hsv" ) )
		getValue( value[ "hsv" ], out.hsv );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, bool& out ) {
	if( !value.IsBool() )
		return;
	out = value.GetBool();
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, float& out ) {
	if( !value.IsFloat() )
		return;
	out = value.GetFloat();
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, int& out ) {
	if( !value.IsInt() )
		return;
	out = value.GetInt();
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, size_t& out ) {
	if( !value.IsUint64() )
		return;
	out = value.GetUint64();
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, string& out ) {
	if( !value.IsString() )
		return;
	out = value.GetString();
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::Vec2& out ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "x" ) )
		getValue( value[ "x" ], out.x );
	if( value.HasMember( "y" ) )
		getValue( value[ "y" ], out.y );
}

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::Color& out ) {
	if( !value.IsObject() )
		return;

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

//================================================================================