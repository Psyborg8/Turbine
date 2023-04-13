#pragma once

//================================================================================

// Definitions
#define NOMINMAX

#define _CRT_SECURE_NO_WARNINGS

//================================================================================

/* Includes */
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <functional>
#include <imgui-SFML.h>
#undef IMGUI_USER_CONFIG
#include <imgui.h>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

//================================================================================

// Post-Include Definitions
#undef GetObject

//================================================================================

/* Aliasing */
using std::array;
using std::function;
using std::list;
using std::make_pair;
using std::make_shared;
using std::make_unique;
using std::map;
using std::pair;
using std::set;
using std::shared_ptr;
using std::stack;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using std::chrono::hours;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::minutes;
using std::chrono::nanoseconds;
using std::chrono::seconds;

using chronoClockPoint = std::chrono::time_point< std::chrono::high_resolution_clock >;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using namespace std::chrono_literals;

using KeyPair = pair< sf::Keyboard::Key, sf::Keyboard::Key >;

//================================================================================

/* Globals */

constexpr float PI = 3.1415926535f;

namespace Folders {

static const string Data = "./Data/";

static const string Shader		   = Data + "Shader/";
static const string VertexShader   = Shader + "Vertex/";
static const string FragmentShader = Shader + "Fragment/";

static const string Assets	  = Data + "Assets/";
static const string Tilesets  = Assets + "Tilesets/";
static const string Sprites	  = Assets + "Sprites/";
static const string Fonts	  = Assets + "Fonts/";
static const string Bullets	  = Assets + "Bullets/";
static const string Particles = Assets + "Particles/";

static const string Maps = Data + "Maps/";

static const string Sound = Data + "Sounds/";

}	 // namespace Folders

//================================================================================

/* Enums */

enum class CollisionType { None = 0, Static, Dynamic };

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
	Background		 = -100,
	BackgroundDetail = -50,
	Terrain			 = 0,
	Player			 = 25,
	Foreground		 = 50,
	UI				 = 100,
};

//--------------------------------------------------------------------------------

enum class MapLayer { Background = 0, BackgroundDetail, Terrain };

//--------------------------------------------------------------------------------

enum class RandomColorType {
	ShuffleRGB,
	MixRGB,
	ShuffleHSV,
	MixHSV,
	SortedHSV,
};

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

enum class InheritanceType { None, Copy, Offset, Multiply };

//--------------------------------------------------------------------------------

enum class EmitterType { Set, Random, Sequence };

//================================================================================
