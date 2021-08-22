//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "MathTypes.h"
#include "World.h"

//================================================================================

namespace System {

//================================================================================

struct SystemInfo {
	unsigned int width{ 1920 };
	unsigned int height{ 1080 };
};

//================================================================================

bool init( int argc, char** argv );

int start();

void exit();

SystemInfo getSystemInfo();

shared_ptr< Worlds::World > getWorld();

sf::RenderWindow* getWindow();

sf::Time getDeltaTime();


//================================================================================

} // System