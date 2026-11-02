execute_process(
  COMMAND "${LAB3_EXECUTABLE}"
  INPUT_FILE "${CMAKE_CURRENT_LIST_DIR}/demo_input.txt"
  OUTPUT_VARIABLE output
  ERROR_VARIABLE errors
  RESULT_VARIABLE result
  TIMEOUT 10
)
if(NOT result STREQUAL "0")
  message(FATAL_ERROR "lab3 failed: ${result}\n${errors}")
endif()

set(expected
"Commands: add triangle|square|rectangle x1 y1 ...; list; total; erase INDEX; demo; quit
0: Triangle (0, 0) (2, 0) (1, 1.73205) center=(1, 0.57735) area=1.73205
1: Square (0, 0) (1, 0) (1, 1) (0, 1) center=(0.5, 0.5) area=1
2: Rectangle (0, 0) (2, 0) (2, 1) (0, 1) center=(1, 0.5) area=2
Total area: 4.73205
Total area: 9.4641
Erased
Total area: 8.4641
")
string(REPLACE "\r\n" "\n" output "${output}")
if(NOT output STREQUAL expected)
  message(FATAL_ERROR "Unexpected demo output:\n${output}\nExpected:\n${expected}")
endif()
