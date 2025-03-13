cmake_minimum_required(VERSION 3.10)
project(MediaLock VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force static runtime for Debug build
set(CMAKE_CXX_FLAGS_DEBUG "/MTd" CACHE STRING "Debug flags" FORCE)

include_directories("C:/cryptopp")
link_directories("C:/cryptopp/x64/Output/Debug")

add_executable(MediaLock src/main.cpp)
target_link_libraries(MediaLock cryptlib.lib)