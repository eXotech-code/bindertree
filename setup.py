from distutils.core import setup, Extension

module = Extension(
    "rangetree",
    sources=["rangetree.cpp"],
    headers=["rangetree.h"],
    extra_compile_args=["-std=c++17", "-O3"]
)
setup(
    name = "RangeTree",
    version = "1.0",
    description = "A range tree with support for generating map zoom levels.",
    ext_modules=[module]
)
