################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.13)

################################################################################
# Set up compilation of the engine library.                                    #
################################################################################
add_subdirectory(ecs)

add_library(planes-engine-lib INTERFACE)
target_link_libraries(planes-engine-lib
    INTERFACE
        planes-engine-ecs-lib
)
