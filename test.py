#!/usr/bin/env python3

from rangetree import BinderTree
from json import load

with open("../rangetree/noise-data.json") as f:
    data = load(f)["noise"]
    points = []
    for x in data:
        lvl = [x[-1]]
        x = x[:-1]
        x.reverse()
        points.append(x + lvl)

tree = BinderTree(points)
res = tree.search(17.0, 18.0, 51.0, 52.0, 14)
# res = tree.search(17.0, 18.0, 51.0, 52.0)
for x in res[0]: print(x)
