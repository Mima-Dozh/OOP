add_executable(lab1 main.cpp)
target_link_libraries(lab1 PRIVATE lab1_utils)
set_target_properties(lab1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
