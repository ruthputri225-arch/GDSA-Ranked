cmake_minimum_required(VERSION 3.20)
project(gdsa_ranked LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Menghubungkan otomatis dengan SDK yang didownload GitHub Actions
if(NOT Geode_DIR AND DEFINED ENV{GEODE_SDK})
    set(Geode_DIR "$ENV{GEODE_SDK}")
endif()

find_package(Geode REQUIRED)

setup_geode_mod(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
