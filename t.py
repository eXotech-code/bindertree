#!/usr/bin/env python3

from bindertree import BinderTree
from json import load

with open("../rangetree/noise-data.json") as f:
    data = load(f)["noise"]
    points = []
    for x in data:
        lvl = [x[-1]]
        x = x[:-1]
        x.reverse()
        points.append(x + lvl)

xs = [x[0] for x in points]
ys = [x[1] for x in points]
min_x = min(xs)
max_x = max(xs)
min_y = min(ys)
max_y = max(ys)

tree = BinderTree(points)

print(min_x, max_x, min_y, max_y)
tree_srt = sorted([x[:-1] for x in tree.search(min_x, max_x, min_y, max_y)])
pt_srt = sorted([x[:-1] for x in points])
lines_tree = [", ".join([str(y) for y in x]) for x in tree_srt]
lines_pt = [", ".join([str(y) for y in x]) for x in pt_srt]
print("TREE\n", "\n".join(lines_tree[:10]), "\n")
print("POINTS\n", "\n".join(lines_pt[:10]))
print(tree_srt == pt_srt)
