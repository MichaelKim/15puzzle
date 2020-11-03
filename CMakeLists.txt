cmake_minimum_required (VERSION 3.15)
project (puzzle)

add_executable(puzzle src/main.cpp src/Board.cpp src/BoardRect.cpp src/Direction.cpp src/DisjointDatabase.cpp src/Idastar.cpp src/InputParser.cpp src/Pattern.cpp src/WalkingDistance.cpp src/Util.cpp)
target_include_directories(puzzle PUBLIC include)
set_target_properties(puzzle PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)
target_compile_options(puzzle PUBLIC -O3 -Wall -Wextra)
target_link_options(puzzle PUBLIC -coverage)

find_program(
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
)
set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
