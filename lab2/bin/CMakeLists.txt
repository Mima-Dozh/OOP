add_executable(lab2 main.cpp)
target_link_libraries(lab2 PRIVATE lab2_decimal)
set_target_properties(lab2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
