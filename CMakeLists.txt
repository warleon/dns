cmake_minimum_required(VERSION 3.13)

project(dns-server-starter-cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

# Download and build Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

# Collect source files for the DNS library (excluding main)
file(GLOB_RECURSE DNS_LIB_SOURCES src/dns/*.cpp src/dns/*.hpp)

# Create a library for the DNS components
add_library(dns_lib ${DNS_LIB_SOURCES})
target_include_directories(dns_lib PUBLIC src)

# Main executable
add_executable(server src/server.cpp)
target_link_libraries(server PRIVATE dns_lib)

# Add address sanitizer flags to main executable (debug mode only)
target_compile_options(server PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -g>)
target_link_options(server PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)

# Enable testing
enable_testing()

# Test executable
file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
if(TEST_SOURCES)
    add_executable(dns_tests ${TEST_SOURCES})
    target_link_libraries(dns_tests PRIVATE 
        dns_lib 
        gtest 
        gtest_main
    )
    
    # Add tests to CTest
    include(GoogleTest)
    gtest_discover_tests(dns_tests)
    
    # Add address sanitizer flags to tests (debug mode only)
    target_compile_options(dns_tests PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -g>)
    target_link_options(dns_tests PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
endif()
