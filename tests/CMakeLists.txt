################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.1)

################################################################################
# Set up compilation of the tests                                              #
################################################################################
file(GLOB_RECURSE TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(planes-test ${TEST_SRC})
target_link_libraries(planes-test ${CONAN_LIBS})

add_test(NAME planes-test
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_BINARY_DIR}/bin/planes-test --force-colour)
