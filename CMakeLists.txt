cmake_minimum_required(VERSION 3.30.5)

project(AudioPluginBoilerplate)

set(CMAKE_CXX_STANDARD 23)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME JUCE
  GITHUB_REPOSITORY juce-framework/JUCE
  GIT_TAG 8.0.4
  VERSION 8.0.4
  SOURCE_DIR ${LIB_DIR}/juce
)

add_subdirectory(plugin)