cmake_minimum_required(VERSION 3.10.2)
project(planes)

# Include Conan-related files.
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup()

add_executable(planes src/main.cpp)

# Make sure we're using C++ 11 when compiling the main executaBLE.
set_property(TARGET planes PROPERTY CXX_STANDARD 11)

conan_target_link_libraries(planes)
