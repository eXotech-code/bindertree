#!/usr/bin/env python3

from bindertree import BinderTree
from json import load

def test():
    with open("../rangetree/noise-data.json") as f:
        data = load(f)["noise"]
        points = []
        for x in data:
            lvl = [x[-1]]
            x = x[:-1]
            x.reverse()
            points.append(x + lvl)

    # points = [
    #     [1.0,1.0,60],
    #     [3.0,2.0,65],
    #     [4.0,5.0,55],
    #     [5.0,3.0,60],
    #     [5.0,1.0,60]
    # ]

    xs = [x[0] for x in points]
    ys = [x[1] for x in points]
    min_x = min(xs)
    max_x = max(xs)
    min_y = min(ys)
    max_y = max(ys)

    tree = BinderTree(points)
    for x in tree.search(min_x, max_x, min_y, max_y):
        if x[2] > 0: print(x)

if __name__ == "__main__":
    test()
