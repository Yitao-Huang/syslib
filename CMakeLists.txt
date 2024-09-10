cmake_minimum_required(VERSION 3.15)
project(SysLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type: Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build: Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()

# Compiler flags for Debug and Release
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")  # Add debug symbols, no optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")        # Optimize fully, disable assertions

file(GLOB SOURCES "src/*.cpp" "src/includes/*.h")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -I/usr/local/opt/llvm/include -lc++abi")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib/c++")
endif()

# Custom target for building in Debug mode
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
    COMMENT "Building with Debug configuration"
)

# Custom target for building in Release mode (default)
add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release
    COMMENT "Building with Release configuration"
)


add_executable(main ${SOURCES})