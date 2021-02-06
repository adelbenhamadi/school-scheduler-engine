cmake_minimum_required (VERSION 2.6)
project (emploilib)
include_directories("${PROJECT_SOURCE_DIR}/engine13")
include_directories("${PROJECT_SOURCE_DIR}/Console")
add_library (emploilib SHARED emploi-lib.cpp solution.cpp cfilestream.cpp engine13/engine.cpp Console/console.cpp emploi-lib.def)




