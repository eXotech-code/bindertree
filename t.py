#!/usr/bin/env python3

from rangetree import BinderTree
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

    tree = BinderTree(points)
    failed_searches = [
        [16.94367815631615, 17.16992786013973, 51.07830436494131, 51.149321347915574, 13],
        [16.78733330134007, 17.01358299376397, 51.05540717814627, 51.09951763930037, 13],
        [16.925502817847605, 17.151752510271503, 51.08502801349243, 51.129110243303145, 13]
    ]
    for f in failed_searches:
        res = tree.search(*f[:-1])
        print(res[0])
