# Require CMake 3.16
cmake_minimum_required(VERSION 3.16)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra) # Not necessary, but makes the compiler tell you whenever you do weird stuff in your code

# Default to RelWithDebugInfo
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebugInfo)
endif()

project(SIMON8 CXX) # Or C for C projects

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/inc) # Or whatever you named your include directory
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCES
    src/main.cpp
    src/renderer.cpp
    # More source files go here
)

# Set header files
set(HEADERS
    inc/renderer.h
    # Header files go here
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
