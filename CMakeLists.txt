cmake_minimum_required(VERSION 3.15)
project(SysLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -I/usr/local/opt/llvm/include -lc++abi")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++")
endif()

# Define the source files
file(GLOB SOURCES "src/*.h")

# Define a variable for the sources of each target
set(TEST_SOURCES ${SOURCES} "tests/test.cpp")
set(BENCHMARK_SOURCES ${SOURCES} "tests/benchmark.cpp")

# Compiler flags for Debug and Release
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Function to add executable target with a specific configuration
function(add_executable_with_config target_name source_files)
    add_executable(${target_name} ${source_files})
    target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target_name} PRIVATE ${CMAKE_CXX_FLAGS_DEBUG})
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target_name} PRIVATE ${CMAKE_CXX_FLAGS_RELEASE})
    endif()
endfunction()

# Add executable targets for both configurations and both sources
add_executable_with_config(test_debug "${TEST_SOURCES}")
add_executable_with_config(test_release "${TEST_SOURCES}")
set_target_properties(test_release PROPERTIES OUTPUT_NAME "test")

add_executable_with_config(benchmark_debug "${BENCHMARK_SOURCES}")
add_executable_with_config(benchmark_release "${BENCHMARK_SOURCES}")
set_target_properties(benchmark_release PROPERTIES OUTPUT_NAME "benchmark")

# Set output directory for the executables
set_target_properties(test_debug test_release benchmark_debug benchmark_release
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)