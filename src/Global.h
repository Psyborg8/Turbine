#pragma once

//================================================================================

// Devlog

// 24-12-20
// Started devlog. Project was uploaded to github
// Just removed Box2D and implemented cute-headers library
// Quad has been replaced with Box, which is an axis-aligned bounding box
// Rendering for Box has been added but not tested.
// To do: //
//		  // Fix all errors from removing Box2D
//		  // Implement and test walls with Box class
//		  // Reimplement Player graphics with Box class
//		  // Implement and test collision with Box class

//================================================================================

/* Preprocessor Definitions */
#define NOMINMAX // Causes problems in Mathfu

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

#pragma warning( push, 0 )
#define CUTE_PNG_IMPLEMENTATION_ONCE
#include <cute_png.h>
#define CUTE_FONT_IMPLEMENTATION_ONCE
#include <cute_font.h>
#define CUTE_C2_IMPLEMENTATION_ONCE
#include <cute_c2.h>
#define CUTE_MATH2D_IMPLEMENTATION_ONCE
#include <cute_math2d.h>
#pragma warning( pop )

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

using Vec2 = c2v;

using Callback = function< void() >;
using KeyCallback = function< void( int key ) >;
using AxisCallback = function< void( float x, float y ) >;

//================================================================================

/* Globals */
namespace Folders
{

static const string DATA = "Data\\";

static const string SHADER = DATA + "Shader\\";
static const string VERTEX_SHADER = SHADER + "Vertex\\";
static const string FRAGMENT_SHADER = SHADER + "Fragment\\";

static const string ASSETS = DATA + "Assets\\";

} // Folders