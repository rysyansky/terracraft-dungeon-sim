cmake_minimum_required(VERSION 3.17)
project(TerracraftBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app)

target_sources(app PRIVATE
    main.cpp
    src/Reader.cpp
    src/AliceCrawler.cpp
    src/GameSimulator.cpp
    src/FogOfWar.cpp
)
target_include_directories(app PRIVATE include)

target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)