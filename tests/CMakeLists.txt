################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.13)

################################################################################
# Set up compilation of the tests                                              #
################################################################################
add_executable(planes-test
    main.cpp
)

# Note that the CMakeListss in the subdirectory below, are adding source files
# to the main test executable target. We do this because this is how tests
# written with Catch 2 are combined into a single test executable. For more
# information on this characteristic of Catch 2, please refer to:
#   https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#scaling-up
add_subdirectory(planes)

target_link_libraries(planes-test
    planes-engine-lib
    ${CONAN_LIBS}
)

add_test(
    NAME planes-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_BINARY_DIR}/bin/planes-test
)
