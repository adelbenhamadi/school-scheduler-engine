cmake_minimum_required (VERSION 2.8)
project (scheduler)
include_directories("${PROJECT_SOURCE_DIR}/app")
include_directories("${PROJECT_SOURCE_DIR}/engine")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/Console")

add_executable (scheduler app/main.cpp engine/cfilestream.cpp engine/cfilestream.h engine/classes.h engine/engine.cpp engine/engine.h engine/solution.cpp engine/solution.h)