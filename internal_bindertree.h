//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#ifndef RANGETREE_INTERNAL_BINDERTREE_H
#define RANGETREE_INTERNAL_BINDERTREE_H

#include "internal_rangetree.h"
#include <array>

constexpr unsigned int MAX_ZOOM = 15;
constexpr float SIZE_X = 0.00080411;
constexpr float SIZE_Y = 0.0004925525;

struct range_data {
  std::vector<Range2D *> *ranges;
  Point *dst;
};

// Return value for zoom_search. Nodes and distance between them as Point.
struct zoomed_nodes {
  std::vector<Node *> *nodes;
  Point *dst;
};

long high_median(std::vector<Node *> &nodes);

class InternalBinderTree : public InternalTree {
public:
  using InternalTree::InternalTree;
  struct zoomed_nodes zoom_search(Range2D *q, long lvl);
private:
  static double greater_len(Range2D *q);
  Range2D *squareify(Range2D *q);
  Point *target(long zoom_lvl);
  struct range_data ranges(Range2D *q, long zoom_lvl);
  struct zoomed_nodes means_from_ranges(struct range_data range_data);
};

#endif // RANGETREE_INTERNAL_BINDERTREE_H
