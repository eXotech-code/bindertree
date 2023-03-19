# rangetree-cpp

This is a fast Python library written in C++ that utlizes a range tree
with additional layer that binds individual points in groups in order to generate
zoom levels used to display the data on the map.

The underlying data structure is a recursively built alpha-balanced binary tree
from George S. Lueker's 1978 whitepaper "A Data Structure for Orthagonal Range Queries".

If you find this weirdo useful feel free to steal it.
