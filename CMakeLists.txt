cmake_minimum_required (VERSION 3.1)
project (taslogger)

if (NOT DEFINED RapidJSON_ROOT)
	message (FATAL_ERROR "Could not find RapidJSON, point RapidJSON_ROOT to the RapidJSON base directory.")
endif ()
find_path (RapidJSON_INCLUDE_DIR rapidjson/writer.h HINTS ${RapidJSON_ROOT}/include)
if (RapidJSON_INCLUDE_DIR)
	message (STATUS "Found RapidJSON include directory.")
else ()
	message (FATAL_ERROR "Could not find RapidJSON include directory.")
endif ()

include_directories (${RapidJSON_INCLUDE_DIR})

set (CMAKE_CXX_STANDARD 11)

add_library (taslogger src/taslogger.cpp)
