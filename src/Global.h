#pragma once

//================================================================================

// Definitions
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

#include <GL/glew.h>
#include <GL/glut.h>

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

using Callback = function< void() >;
using KeyCallback = function< void( int key ) >;
using AxisCallback = function< void( float x, float y ) >;
using Vec2Callback = function< void( int x, int y ) >;

//================================================================================

/* Globals */
namespace Folders {

static const string Data = "Data\\";

static const string Shader = Data + "Shader\\";
static const string VertexShader = Shader + "Vertex\\";
static const string FragmentShader = Shader + "Fragment\\";

static const string Assets = Data + "Assets\\";
static const string Tilesets = Assets + "Tilesets\\";
static const string Sprites = Assets + "Sprites\\";

static const string Worlds = Data + "Worlds\\";

} // Folders

//================================================================================

/* Enums */

enum class KeyCode {
	// ASCII
	Backspace = 10,
	Tab = 11,
	Escape = 27,
	Space = 32,
	Delete = 127,

	Num_0 = 48,
	Num_1, Num_2, Num_3, Num_4, Num_5, Num_6, Num_7, Num_8, Num_9,

	a = 97,
	b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,

	// Special characters
	F1 = 201, 
	F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	Left, Right, Up, Down,
	PageUp, PageDown,
	Home,
	Insert
};

//--------------------------------------------------------------------------------

enum class CollisionType {
	None = 0,
	Static,
	Dynamic
};

//================================================================================