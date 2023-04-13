//================================================================================

#pragma once

//================================================================================

#include "global.h"
#include "mathtypes.h"

#include <rapidjson/document.h>

//================================================================================

namespace json {

//--------------------------------------------------------------------------------

bool save( function< rapidjson::Value() > func, string path );
bool load( function< bool( const rapidjson::Value& ) > func, string path );

//--------------------------------------------------------------------------------

void setAllocator( rapidjson::MemoryPoolAllocator< rapidjson::CrtAllocator >* allocator );
rapidjson::MemoryPoolAllocator< rapidjson::CrtAllocator >& getAllocator();

rapidjson::Value getValue( Math::ValueSet< int > value );
rapidjson::Value getValue( Math::ValueSet< float > value );
rapidjson::Value getValue( Math::ValueSet< Math::Vec2 > value );
rapidjson::Value getValue( Math::ValueSet< Math::Color > value );

rapidjson::Value getValue( bool value );
rapidjson::Value getValue( float value );
rapidjson::Value getValue( int value );
rapidjson::Value getValue( size_t value );
rapidjson::Value getValue( const string& value );
rapidjson::Value getValue( Math::Vec2 value );
rapidjson::Value getValue( Math::Color value );

//--------------------------------------------------------------------------------

void getValue( const rapidjson::Value& value, Math::ValueSet< int >& out );
void getValue( const rapidjson::Value& value, Math::ValueSet< float >& out );
void getValue( const rapidjson::Value& value, Math::ValueSet< Math::Vec2 >& out );
void getValue( const rapidjson::Value& value, Math::ValueSet< Math::Color >& out );

void getValue( const rapidjson::Value& value, bool& out );
void getValue( const rapidjson::Value& value, float& out );
void getValue( const rapidjson::Value& value, int& out );
void getValue( const rapidjson::Value& value, size_t& out );
void getValue( const rapidjson::Value& value, string& out );
void getValue( const rapidjson::Value& value, Math::Vec2& out );
void getValue( const rapidjson::Value& value, Math::Color& out );

//--------------------------------------------------------------------------------

}

//================================================================================