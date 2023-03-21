cmake_minimum_required( VERSION 3.9 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_BUILD_TYPE "DEBUG" )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set( PROJECT_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin )
set( PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src )

project( turbine VERSION 0.0.1 LANGUAGES CXX )

file( GLOB SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp )
add_library( turbine ${SRC} )

target_include_directories( turbine
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/src/editor
)

# Dependencies
include( FetchContent )

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG 2.5.1
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG master
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY "https://github.com/SFML/imgui-sfml.git"
    GIT_TAG master
    GIT_PROGRESS TRUE
)
FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
    GIT_PROGRESS TRUE
)

# SFML
message("Fetching SFML...")
FetchContent_MakeAvailable( sfml )

# ImGui
message("Fetching imgui...")
FetchContent_MakeAvailable( imgui )

# ImGui-SFML
message("Fetching imgui-sfml...")
set( IMGUI_DIR ${imgui_SOURCE_DIR} )
option( IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF )
FetchContent_MakeAvailable( imgui-sfml )

# rapidjson
set( RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
set( RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL "" FORCE )
set( RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE )
message("Fetching rapidjson...")
FetchContent_MakeAvailable( rapidjson )

target_include_directories( turbine PUBLIC ${rapidjson_SOURCE_DIR}/include )

#fmt
find_package( fmt )


target_link_libraries(
    turbine

    ImGui-SFML::ImGui-SFML
    fmt::fmt
)

target_compile_options( turbine
    PRIVATE
        -std=c++20
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-sign-compare
)

if( CMAKE_BUILD_TYPE STREQUAL "DEBUG" )
    target_compile_options( turbine PUBLIC -ggdb3 -Og )
    target_compile_definitions( turbine PRIVATE _DEBUG=1 )
elseif( CMAKE_BUILD_TYPE STREQUAL "RELEASE" )
    target_compile_options( turbine PUBLIC -O2 )
endif()

# Editor
file( GLOB_RECURSE EDITOR_SRC ${CMAKE_CURRENT_LIST_DIR}/src/editor/*.cpp )
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/editor )
add_executable( turbine-editor ${EDITOR_SRC} )
target_link_libraries( turbine-editor turbine )
