cmake_minimum_required(VERSION 3.30)
project(tinytinyrenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CMakePrintHelpers)
    add_definitions(-DDEBUG=1)
endif()


add_subdirectory(renderUtils)
add_subdirectory(thirdparty/special-lamp)
add_executable(tinytinyrenderer main.cpp)
target_link_libraries(tinytinyrenderer PRIVATE renderUtils)
target_include_directories(tinytinyrenderer PRIVATE thirdparty)
target_include_directories(renderUtils PRIVATE thirdparty)

# Im using clang++, I'm not sure why cmake returns GNU.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(tinytinyrenderer PRIVATE -mavx2 -mfma -static-libstdc++ -static-libgcc)
endif()