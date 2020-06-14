cmake_minimum_required(VERSION 3.10)
project(terrainGenCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -O3)

add_executable(terrainGenCpp biomeGen.cpp)