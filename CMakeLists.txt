cmake_minimum_required(VERSION 3.20)
project(gdsa_ranked LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Memaksa CMake membaca jalur SDK dari GitHub Actions secara mutlak
if(DEFINED ENV{GEODE_SDK})
    set(Geode_DIR "$ENV{GEODE_SDK}")
message(STATUS "Geode SDK found at: $ENV{GEODE_SDK}")
else()
    message(FATAL_ERROR "GEODE_SDK environment variable is not set!")
endif()

find_package(Geode REQUIRED)

setup_geode_mod(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
