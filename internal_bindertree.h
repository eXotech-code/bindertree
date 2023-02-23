//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#ifndef RANGETREE_INTERNAL_BINDERTREE_H
#define RANGETREE_INTERNAL_BINDERTREE_H

#include "internal_rangetree.h"
#include <cmath>

constexpr unsigned int MAX_ZOOM = 15;
constexpr float SIZE_X = 0.00080411;
constexpr float SIZE_Y = 0.0004925525;

struct range_data {
  std::vector<Range2D> ranges;
  Point dst;
};

// Return value for zoom_search. Nodes and distance between them as Point.
struct return_points {
  std::vector<record> points;
  Point dst;
};

int high_median(std::vector<record> &points);

class InternalBinderTree : public InternalTree {
public:
  using InternalTree::InternalTree;
  return_points zoom_search(Range2D &q, int lvl);
private:
  static double greater_len(Range2D &q);
  Range2D squareify(Range2D &q);
  Point target(int zoom_lvl);
  struct range_data ranges(Range2D &q, int zoom_lvl);
  return_points means_from_ranges(range_data range_data);
};

#endif // RANGETREE_INTERNAL_BINDERTREE_H
