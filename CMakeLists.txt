# Require CMake 3.16
cmake_minimum_required(VERSION 3.16)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra) # Not necessary, but makes the compiler tell you whenever you do weird stuff in your code

project(SIMON8 CXX) # Or C for C projects

include_directories(${PROJECT_SOURCE_DIR}/inc) # Or whatever you named your include directory

set(SOURCES
    src/main.cpp
    # More source files go here
)

# Set header files
set(HEADERS
    # Header files go here
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
