################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.13)

################################################################################
# Set up compilation of the engine ECS module.                                 #
################################################################################
add_library(planes-engine-ecs-lib STATIC "")
target_sources(planes-engine-ecs-lib
    PRIVATE
        component.cpp
        entity.cpp
)
target_link_libraries(planes-engine-ecs-lib
    ${CONAN_LIBS}
)
