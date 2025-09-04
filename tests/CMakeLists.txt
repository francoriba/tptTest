# Tests CMakeLists.txt
include_directories(../include)
include_directories(../lib/unity/src)

# Unit tests
add_executable(test_main unit/test_main.c)

# Enlazar con unity_framework
target_link_libraries(test_main unity_framework)

# Linkear con cJSON (desde Conan o submódulo)
if(cjson_FOUND)
  target_link_libraries(test_main cjson::cjson)
else()
  include_directories(../lib/cjson)
  target_link_libraries(test_main cjson)
endif()

# Add test to CTest
add_test(NAME unit_tests COMMAND test_main)
