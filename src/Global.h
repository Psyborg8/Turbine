#pragma once

//================================================================================

// Definitions
#pragma warning( disable : 4244 ) 
#pragma warning( disable : 26812 )
#define NOMINMAX

//================================================================================

/* Includes */
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <cassert>

#include <algorithm>
#include <functional>
#include <memory>
#include <variant>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stack>
#include <chrono>
#include <array>

#include <SFML/graphics.hpp>
#include <SFML/Audio.hpp>

#include <imgui.h>
#include <imgui-sfml.h>

//================================================================================

// Post-Include Definitions
#undef GetObject

//================================================================================

/* Aliasing */
using std::string;
using std::vector;
using std::map;
using std::unordered_map;
using std::set;
using std::unordered_set;
using std::stack;
using std::unique_ptr;
using std::make_unique;
using std::shared_ptr;
using std::make_shared;
using std::function;
using std::pair;
using std::make_pair;
using std::array;

using std::chrono::hours;
using std::chrono::minutes;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::nanoseconds;

using chronoClockPoint = std::chrono::time_point< std::chrono::high_resolution_clock >;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using namespace std::chrono_literals;

using KeyPair = pair< sf::Keyboard::Key, sf::Keyboard::Key >;

//================================================================================

/* Globals */

constexpr float PI = 3.1415926535f;

namespace Folders {

static const string Data = "./Data/";

static const string Shader = Data + "Shader/";
static const string VertexShader = Shader + "Vertex/";
static const string FragmentShader = Shader + "Fragment/";

static const string Assets = Data + "Assets/";
static const string Tilesets = Assets + "Tilesets/";
static const string Sprites = Assets + "Sprites/";
static const string Fonts = Assets + "Fonts/";
static const string Bullets = Assets + "Bullets/";
static const string Particles = Assets + "Particles/";

static const string Maps = Data + "Maps/";

static const string Sound = Data + "Sounds/";

} // Folders

//================================================================================

/* Enums */

enum class CollisionType {
	None = 0,
	Static,
	Dynamic
};

//--------------------------------------------------------------------------------

enum class ControllerButton {
	Bottom,
	Left,
	Top,
	Right,
	R,
	L,
	RTrigger,
	LTrigger,
	LeftStick,
	RightStick,
	Select,
	Start,
	Special,
	Logo,
	None,
};

//--------------------------------------------------------------------------------

enum class ControllerAxis {
	RightStickX,
	RightStickY,
	LeftStickX,
	LeftStickY,
	RTrigger,
	LTrigger,
	DPadX,
	DPadY,
	None
};

//--------------------------------------------------------------------------------

enum class RenderPriority {
	Background = -100,
	BackgroundDetail = -50,
	Terrain = 0,
	Player = 25,
	Foreground = 50,
	UI = 100,
};

//--------------------------------------------------------------------------------

enum class MapLayer {
	Background = 0,
	BackgroundDetail,
	Terrain
};

//================================================================================