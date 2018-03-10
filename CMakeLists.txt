cmake_minimum_required (VERSION 3.1)
project (puzzle)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Werror -std=c++1z -O3")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(puzzle main.cpp src/Board.cpp src/DisjointDatabase.cpp src/PartialDatabase.cpp src/Pattern.cpp src/InputParser.cpp)
