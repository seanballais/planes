################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.13)

################################################################################
# This is a bridge CMakeLists (at least, for now) to let us easily access the  #
# lower-level CMakeLists from the upper-level CMakeLists.                      #
################################################################################
add_subdirectory(ecs)
