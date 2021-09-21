//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "Object.h"

//================================================================================

namespace Debug {

//--------------------------------------------------------------------------------

enum class DebugType {
	None = 0,
	Info,
	Warning,
	Error,
	Input,
	Performance,
};

//--------------------------------------------------------------------------------

// UI
void init( Object* world );

void show();

void hide();

// Console
void addSetCommand( string name, bool& value, string helper = "" );
void addSetCommand( string name, float& value, string helper = "" );
void addSetCommand( string name, int& value, string helper = "" );
void addSetCommand( string name, unsigned int& value, string helper = "" );
void addSetCommand( string name, size_t& value, string helper = "" );
void addSetCommand( string name, Math::Vec2& value, string helper = "" );
void addSetCommand( string name, milliseconds& value, string helper = "" );

void addShowCommand( string name, function< void() > callback, string helper = "" );
void addHideCommand( string name, function< void() > callback, string helper = "" );

void addCommand( string command, size_t args, function< void( vector< string > args ) > func, string helper = "" );
void addCommand( string command, function< void() > func, string helper = "" );

void addMessage( string message, DebugType type = DebugType::None );

void incDrawCall();

// Timers
void startTimer( string name );

void stopTimer( string name );

float getAverageTime( string name );

//--------------------------------------------------------------------------------

}

//================================================================================