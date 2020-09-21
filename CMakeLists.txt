# CMakeList.txt : CMake project for brs, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project (brs_project LANGUAGES CXX)

# Add source to this project's header.
# add_library (brs INTERFACE)
# target_sources (brs INTERFACE "brs.hpp")
# target_include_directories (brs INTERFACE "brs/")

# Enable testing
enable_testing ()

add_subdirectory ( "tests" )
