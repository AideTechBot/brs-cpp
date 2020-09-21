# Executables
add_executable (test_brs_color color_tests.cpp)
add_executable (test_brs_bitreader bitreader_tests.cpp)
add_executable (test_brs_reader brsreader_tests.cpp)
add_executable (test_brs_uuid uuid_tests.cpp)

# Tests
add_test (brs_color test_brs_color)
add_test (brs_bitreader test_brs_bitreader)
add_test (brs_reader test_brs_reader)
add_test (brs_uuid test_brs_uuid)