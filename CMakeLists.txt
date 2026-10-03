cmake_minimum_required(VERSION 3.13)
project(GDSA_Ranked)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Geode REQUIRED)

add_geode_mod(
    GDSA_Ranked
    "v1.0.0"
    src/main.cpp
)
