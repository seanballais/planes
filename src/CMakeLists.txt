################################################################################
# Set up CMake.                                                                #
################################################################################
cmake_minimum_required(VERSION 3.13)

################################################################################
# Set up compilation of the main application.                                  #
################################################################################
set(PLANES_MAIN "${CMAKE_SOURCE_DIR}/src/main.cpp")

# Add the necessary libraries.
add_subdirectory(planes)

add_executable(planes-game "")
target_sources(planes-game
    PRIVATE
        main.cpp
)
target_link_libraries(planes-game
    planes-engine-lib
    ${CONAN_LIBS}
)
