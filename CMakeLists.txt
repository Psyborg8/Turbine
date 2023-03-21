cmake_minimum_required( VERSION 3.9 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_BUILD_TYPE "DEBUG" )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set( PROJECT_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin )
set( PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src )

project( turbine VERSION 0.0.1 LANGUAGES CXX )

file( GLOB_RECURSE SRC src/*.cpp )
file( GLOB_RECURSE IMGUI_SRC ${CMAKE_CURRENT_LIST_DIR}/deps/imgui/*.cpp )
file( GLOB_RECURSE IMGUI_SFML_SRC ${CMAKE_CURRENT_LIST_DIR}/deps/imgui-sfml/*.cpp )
add_library( turbine ${SRC} ${IMGUI_SRC} ${IMGUI_SFML_SRC} )

target_include_directories( turbine
    PUBLIC
        ${PROJECT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui/
        ${CMAKE_CURRENT_LIST_DIR}/deps/RapidJSON/include/
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui-sfml/
)

# SFML
include( FetchContent )
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.5.1
)
FetchContent_MakeAvailable( SFML )

find_package( fmt )
target_link_libraries(
    turbine
    sfml-graphics
    sfml-audio
    sfml-system
    sfml-window
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

