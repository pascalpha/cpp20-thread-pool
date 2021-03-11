cmake_minimum_required(VERSION 3.16)
project(tpl)
set(CMAKE_CXX_STANDARD 20)

add_library(tpl INTERFACE)
target_include_directories(tpl INTERFACE src)
target_link_libraries(tpl INTERFACE pthread)

add_executable(demo main.cc)
target_link_libraries(demo tpl)

add_executable(demo-2 main-2.cc)
target_link_libraries(demo-2 tpl)