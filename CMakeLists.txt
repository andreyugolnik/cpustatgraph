project(cpustatgraph)
cmake_minimum_required(VERSION 2.6)

include(FindPkgConfig)
include(CheckLibraryExists)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    message(STATUS "=== RELEASE MODE ===")
    add_definitions( "-DNDEBUG" )
    add_definitions( "-Wall -O2" )
else()
    message(STATUS "=== DEBUG MODE ===")
    add_definitions( "-DDEBUG" )
    add_definitions( "-Wall -Wextra -pedantic -O0 -g" )
endif()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

find_package(XCB REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(
    ${XCB_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
    )

file( GLOB_RECURSE SOURCES "src/*.cpp" )

add_executable( cpustatgraph ${SOURCES} )

target_link_libraries( cpustatgraph
    ${XCB_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    )

