cmake_minimum_required(VERSION 3.13)

project(dns-server-starter-cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

add_executable(server ${SOURCE_FILES})

# Add address sanitizer flags
target_compile_options(server PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
target_link_options(server PRIVATE -fsanitize=address)
