cmake_minimum_required(VERSION 3.28)
project(Raycaster LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(raycaster src/main.cpp)
target_compile_features(raycaster PRIVATE cxx_std_17)
target_link_libraries(raycaster PRIVATE SFML::Graphics)
