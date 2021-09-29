project(epuck_grouping)
cmake_minimum_required(VERSION 3.21)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
include(CMakeModules/enki.cmake)

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL)
find_package(OpenGL REQUIRED)

add_subdirectory(projects)