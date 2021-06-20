cmake_minimum_required (VERSION 3.15)

project (puzzle CXX)
include(cmake/Settings.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

add_library(project_warnings INTERFACE)
include(cmake/Warnings.cmake)
set_project_warnings(project_warnings)

include(cmake/StaticAnalyzers.cmake)

add_executable(puzzle src/main.cpp src/Board.cpp src/BoardRect.cpp src/Direction.cpp src/DisjointDatabase.cpp src/Idastar.cpp src/InputParser.cpp src/Pattern.cpp src/WalkingDistance.cpp src/Util.cpp)
target_include_directories(puzzle PRIVATE include)
target_link_libraries(puzzle PRIVATE project_options project_warnings -ltbb)
