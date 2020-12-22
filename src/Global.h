#pragma once

//================================================================================

// TODO:
// Box 2D setup
// World system
// Text
// Logging and trackers

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
#include <mathfu/constants.h>
#include <mathfu/vector.h>
#include <mathfu/matrix.h>
#include <mathfu/quaternion.h>
#include <mathfu/io.h>
#include <mathfu/utilities.h>
#include <mathfu/glsl_mappings.h>

#include <box2d/box2d.h>
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