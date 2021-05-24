//================================================================================

#pragma once

//================================================================================

#include "MathTypes.h"
#include "World.h"

//================================================================================

namespace System {

//================================================================================

struct SystemInfo {
	int width{ 800 };
	int height{ 450 };
};

//================================================================================

bool init( int argc, char** argv );

int start();

void exit();

SystemInfo getSystemInfo();

shared_ptr< Worlds::World > getWorld();

bool getKeyState( KeyCode key );

Math::Vec2 getMousePosition();

double getDeltaTime();


//================================================================================

} // System