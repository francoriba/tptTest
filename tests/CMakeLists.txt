# Tests CMakeLists.txt
include_directories(../include)
include_directories(../lib/unity/src)
include_directories(../lib/cjson)

# Unit tests
add_executable(test_main unit/test_main.c)
target_link_libraries(test_main unity_framework cjson)

# Add test to CTest
add_test(NAME unit_tests COMMAND test_main)
