cmake_minimum_required(VERSION 2.6)
project(flurr)

add_library(flurr SHARED FlurrCore.cpp) 

include_directories("$(PROJECT_SOURCE_DIR)/include/")
add_subdirectory("source/flurr" "bin")

add_executable(Basic main.cpp)
target_link_libraries(Basic flurr)
