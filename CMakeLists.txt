################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.1)
project(planes)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include CMake project modules.
include("BuildType")

# Integrate with Conan.
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup()

# Make sure we're using C++ 11 when compiling the main executaBLE.
set(CMAKE_CXX_STANDARD 11)

################################################################################
# Set up compilation of the app.                                               #
################################################################################
include_directories("${CMAKE_SOURCE_DIR}/src")

set(PROJECT_MAIN "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_executable(planes ${PROJECT_MAIN})
target_link_libraries(planes ${CONAN_LIBS})

################################################################################
# Set up compilation of the tests.                                             #
################################################################################
enable_testing()
add_subdirectory(tests)
