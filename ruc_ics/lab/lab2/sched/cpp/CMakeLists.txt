cmake_minimum_required(VERSION 3.10)
project(ccsp-sched VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall)

add_subdirectory(src)
add_subdirectory(thirdparty)
