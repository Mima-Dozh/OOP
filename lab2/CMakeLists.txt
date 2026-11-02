cmake_minimum_required(VERSION 3.16)

project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(lab2_decimal STATIC
    lib/decimal.cpp
    lib/decimal.h
)
target_include_directories(lab2_decimal PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)
target_compile_features(lab2_decimal PUBLIC cxx_std_17)

add_subdirectory(bin)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
