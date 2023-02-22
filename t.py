#!/usr/bin/env python3

from bindertree import BinderTree
# from json import load

# with open("../rangetree/noise-data.json") as f:
#     data = load(f)["noise"]
#     points = []
#     for x in data:
#         lvl = [x[-1]]
#         x = x[:-1]
#         x.reverse()
#         points.append(x + lvl)

def test():
    points = [
        [16.8552225081, 51.0274727095, 60], #1
        [16.8552225081, 51.027965262, 60],  #2
        [16.8552225081, 51.0284578145, 60], #3
        [16.8552225081, 51.028950367, 60],  #4
        [16.8552225081, 51.0294429195, 60], #5
        [16.8552225081, 51.029935472, 60],  #6
        [16.8552225081, 51.0304280245, 60], #7
        [16.8552225081, 51.030920577, 60],  #8
        [16.8552225081, 51.0314131295, 60], #9
        [16.8552225081, 51.031905682, 60]   #10
    ]

    xs = [x[0] for x in points]
    ys = [x[1] for x in points]
    min_x = min(xs)
    max_x = max(xs)
    min_y = min(ys)
    max_y = max(ys)

    tree = BinderTree(points)
    # tree.search(min_x, max_x, min_y, max_y)

# print("SEARCH RANGE:", min_x, max_x, min_y, max_y)
# print("POINTS")
# for x in sorted(points, key=lambda val: (val[0], val[1])):
#     print(x)
# print("TREE")
# for x in sorted(results, key=lambda val: (val[0], val[1])):
#     print(x)
