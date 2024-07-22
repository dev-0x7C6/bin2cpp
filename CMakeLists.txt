cmake_minimum_required(VERSION 3.20...3.32)

project(
  bin2cpp
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_definitions("-D__cpp_concepts=202002L")

include(GNUInstallDirs)

find_package(CLI11 REQUIRED)
find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(PkgConfig REQUIRED)


add_subdirectory("src")
