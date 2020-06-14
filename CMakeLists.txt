cmake_minimum_required(VERSION 3.10)
project(terrainGenCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3")

add_executable(biomeGen biomeGen.cpp)
add_executable(terrainGen biomeGen.cpp terrainGen.cpp)
add_executable(Main biomeGen.cpp terrainGen.cpp Main.cpp)