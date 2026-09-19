include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "Use the shared runtime with GoogleTest" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Build GoogleMock")
FetchContent_MakeAvailable(googletest)

add_executable(lab1_tests utils_test.cpp)
target_link_libraries(lab1_tests PRIVATE lab1_utils GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(lab1_tests)
