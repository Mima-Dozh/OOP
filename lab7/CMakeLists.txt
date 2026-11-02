cmake_minimum_required(VERSION 3.20)
project(oop_lab7 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()
find_package(Threads REQUIRED)
add_library(lab7_core
  src/combat/visitor.cpp
  src/events/observer.cpp
  src/npc/druid.cpp
  src/npc/factory.cpp
  src/npc/kind.cpp
  src/npc/npc.cpp
  src/npc/orc.cpp
  src/npc/squirrel.cpp
  src/simulation/battle.cpp
  src/simulation/simulation.cpp
  src/world/world.cpp)
target_include_directories(lab7_core PUBLIC include)
target_link_libraries(lab7_core PUBLIC Threads::Threads)
add_executable(lab7 src/main.cpp)
target_include_directories(lab7 PRIVATE include)
target_link_libraries(lab7 PRIVATE lab7_core)
if(MSVC)
  target_compile_options(lab7 PRIVATE /W4 /utf-8)
  target_compile_options(lab7_core PRIVATE /W4 /utf-8)
else()
  target_compile_options(lab7 PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(lab7_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
include(CTest)
if(BUILD_TESTING)
  include(FetchContent)
  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
  endif()
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  if(NOT DEFINED FETCHCONTENT_SOURCE_DIR_GOOGLETEST AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../lab1/build/_deps/googletest-src/CMakeLists.txt")
    set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST "${CMAKE_CURRENT_SOURCE_DIR}/../lab1/build/_deps/googletest-src")
  endif()
  FetchContent_MakeAvailable(googletest)
  add_executable(lab7_tests tests/tests.cpp)
  target_include_directories(lab7_tests PRIVATE include)
  target_link_libraries(lab7_tests PRIVATE GTest::gtest_main lab7_core)
  include(GoogleTest)
  gtest_discover_tests(lab7_tests)
endif()
