cmake_minimum_required(VERSION 3.23)
project(RangeTree)
set(CMAKE_CXX_STANDARD 17)

find_package(Python3 REQUIRED COMPONENTS Development)

add_library(rangetree SHARED rangetree.cpp internal_rangetree.cpp internal_bindertree.cpp)
target_include_directories(rangetree PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(rangetree PRIVATE ${Python3_LIBRARIES})
set_target_properties(rangetree PROPERTIES PREFIX "")
set_target_properties(rangetree PROPERTIES SUFFIX ".so")

set_target_properties(rangetree PROPERTIES RUNTIME_OUTPUT_DIRECTORY build/)