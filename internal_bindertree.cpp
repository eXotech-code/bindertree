//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#include "internal_bindertree.h"

long high_median(std::vector<Node *> &nodes) {
  const long mid = ceil(nodes.size() / 2);

  return nodes[mid]->lvl;
}

double InternalBinderTree::greater_len(Range2D *q) {
  const double len_x = q->x.len();
  const double len_y = q->y.len();

  if (len_x > len_y) {
    return len_x;
  }
  return len_y;
}

Range2D *InternalBinderTree::squareify(Range2D *q) {
  Point *high;

  const double len_side = this->greater_len(q);
  high = new Point(q->x.low + len_side, q->y.low + len_side / 2);

  return new Range2D(new Point(q->x.low, q->y.low), high);
}

Point *InternalBinderTree::target(long zoom_lvl) {
  /* How much the distance between points on the map changed
   * compared to MAX_ZOOM. */
  const float magnf = (float)(MAX_ZOOM - zoom_lvl) * 2;
  return new Point(SIZE_X * magnf, SIZE_Y * magnf);
}

struct range_data InternalBinderTree::ranges(Range2D *q, long zoom_lvl) {
  Point *target;
  Range2D *squarified;
  std::vector<Range2D *> *ranges;
  bool iterate;
  Point *dst;

  target = this->target(zoom_lvl);
  squarified = this->squareify(q);
  ranges = new std::vector<Range2D *>(1, squarified);
  struct range_data data = {ranges, new Point(SIZE_X, SIZE_Y)};

  if (!(target->x <= SIZE_X || target->y <= SIZE_Y)) {
    iterate = true;
    while (iterate) {
      std::vector<Range2D *> *new_ranges;

      new_ranges = new std::vector<Range2D *>;
      for (Range2D *r : *ranges) {
        Point *center;
        Range2D *a, *b, *c, *d;

        center = r->center();

        // bottom-left
        a = new Range2D(new Point(r->x.low, r->y.low), center);
        // bottom-right
        b = new Range2D(new Point(center->x, r->y.low), new Point(r->x.high, center->y));
        // top-right
        c = new Range2D(center, new Point(r->x.high, r->y.high));
        // top-left
        d = new Range2D(new Point(r->x.low, center->y), new Point(center->x, r->y.high));

        new_ranges->insert(new_ranges->end(), {a, b, c, d});
      }
      ranges = new_ranges;
      dst = (*ranges)[0]->dst((*ranges)[2]);
      iterate = dst->x > target->x * 2 || dst->y > target->y * 2;
    }

    data.dst = dst;
  }

  return data;
}

struct zoomed_nodes InternalBinderTree::means_from_ranges(struct range_data range_data) {
  struct zoomed_nodes return_val {};

  return_val.nodes = new std::vector<Node *>;
  return_val.dst = range_data.dst;

  for (Range2D *q : *range_data.ranges) {
    std::vector<Node *> *nodes = new std::vector<Node *>;

    this->search(q, *nodes);
    if (!nodes->empty()) {
      if (range_data.dst->x != SIZE_X) {
        const long median = high_median(*nodes);
        Point *center = q->center();
        return_val.nodes->push_back(new Node(center, median));
      } else {
        return_val.nodes = nodes;
      }
    }
  }

  return return_val;
}

struct zoomed_nodes InternalBinderTree::zoom_search(Range2D *q, long lvl) {
  struct range_data ranges = this->ranges(q, lvl);
  return this->means_from_ranges(ranges);
}