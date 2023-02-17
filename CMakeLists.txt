cmake_minimum_required(VERSION 3.23)
project(BinderTree)
set(CMAKE_CXX_STANDARD 17)

find_package(Python3 REQUIRED COMPONENTS Development)

add_library(bindertree MODULE bindertree.cpp internal_rangetree.cpp internal_bindertree.cpp)
target_include_directories(bindertree PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(bindertree PRIVATE ${Python3_LIBRARIES})
set_target_properties(bindertree
  PROPERTIES
  PREFIX ""
  SUFFIX ".so"
  CMAKE_LIBRARY_OUTPUT_DIRECTORY "build/lib"
)
