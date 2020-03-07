################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.13)

################################################################################
# Just a bridge CMakeLists.txt so that we can access the lower-level           #
# CMakeLists easily from upper-level CMakeLists.                               #
################################################################################
add_subdirectory(engine)
add_subdirectory(utils)

# TODO (Mar. 2, 2020): Create an interface library, the planes module library, here.
