project(epuck_grouping)
cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
include(CMakeModules/enki.cmake)

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_subdirectory(projects)
