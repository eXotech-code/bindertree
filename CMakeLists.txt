cmake_minimum_required(VERSION 3.23)
project(rangetree)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(SRCS "rangetree.cpp" "rangetree.h")
add_library(RangeTree SHARED ${SRCS})

find_package(Python REQUIRED)

set_target_properties(
        RangeTree
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME "RangeTree"
            LINKER_LANGUAGE C
            LINK_LIBRARIES "${PYTHON_LIBRARIES}"
            INCLUDE_DIRECTORIES "/opt/homebrew/Cellar/python@3.10/3.10.9/Frameworks/Python.framework/Versions/3.10/include/python3.10;${PYTHON_INCLUDE_DIR}"
        # COMPILE_DEFINITIONS "Py_LIMITED_API=0x03090000" # (broke in default installation)
)
