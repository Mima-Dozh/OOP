cmake_minimum_required(VERSION 3.16)

project(lab1 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(lab1_utils STATIC
    lib/utils.cpp
    lib/utils.h
)
target_include_directories(lab1_utils PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)
target_compile_features(lab1_utils PUBLIC cxx_std_17)

add_subdirectory(bin)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
