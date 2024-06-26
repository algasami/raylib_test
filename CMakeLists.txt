cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(example)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)


# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

# Our Project

add_executable(gravity_sim src/gravity_sim.c src/utils.h)
add_executable(graph src/graph.cpp src/utils.hpp)
#set(raylib_VERBOSE 1)
target_link_libraries(gravity_sim raylib)
target_link_libraries(graph raylib)

set(BOOST_INCLUDEDIR C:/local/boost_1_85_0)
set(BOOST_LIBRARYDIR C:/local/boost_1_85_0/libs)

find_package(Boost 1.85 COMPONENTS graph  REQUIRED )
target_link_libraries(graph Boost::graph )


# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(gravity_sim PROPERTIES SUFFIX ".html")
    set_target_properties(graph PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(gravity_sim "-framework IOKit")
    target_link_libraries(gravity_sim "-framework Cocoa")
    target_link_libraries(gravity_sim "-framework OpenGL")

    target_link_libraries(graph "-framework IOKit")
    target_link_libraries(graph "-framework Cocoa")
    target_link_libraries(graph "-framework OpenGL")
endif()