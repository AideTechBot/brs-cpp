# Executables
add_executable (test_brs_color color_tests.cpp)
add_executable (test_brs_bitreader bitreader_tests.cpp)
add_executable (test_brs_reader brsreader_tests.cpp)
set_target_properties(test_brs_reader PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable (test_brs_uuid uuid_tests.cpp)

# Tests
add_test (brs_color test_brs_color WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test (brs_bitreader test_brs_bitreader WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test (brs_reader test_brs_reader WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test (brs_uuid test_brs_uuid WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})