//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#include "internal_bindertree.h"

int high_median(std::vector<record> &points) {
  const long mid = ceil(points.size() / 2);

  return points[mid].lvl;
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
  const double len_side = this->greater_len(q);
  auto high = new Point(q->x.low + len_side, q->y.low + len_side / 2);

  return new Range2D(new Point(q->x.low, q->y.low), high);
}

Point *InternalBinderTree::target(long zoom_lvl) {
  /* How much the distance between points on the map changed
   * compared to MAX_ZOOM. */
  const float magnf = (float)(MAX_ZOOM - zoom_lvl) * 2;
  return new Point(SIZE_X * magnf, SIZE_Y * magnf);
}

struct range_data InternalBinderTree::ranges(Range2D *q, long zoom_lvl) {
  Point *target = this->target(zoom_lvl);
  Range2D *squarified = this->squareify(q);
  struct range_data data = {
      new std::vector<Range2D *>(1, new Range2D(*squarified)),
      new Point(SIZE_X, SIZE_Y)
  };

  if (!(target->x <= SIZE_X || target->y <= SIZE_Y)) {
    bool iterate = true;

    while (iterate) {
      auto new_ranges = new std::vector<Range2D *>;
      for (Range2D *r : *data.ranges) {
        Point *center = r->center();

        // bottom-left
        auto a = new Range2D(new Point(r->x.low, r->y.low), center);
        // bottom-right
        auto b = new Range2D(new Point(center->x, r->y.low), new Point(r->x.high, center->y));
        // top-right
        auto c = new Range2D(center, new Point(r->x.high, r->y.high));
        // top-left
        auto d = new Range2D(new Point(r->x.low, center->y), new Point(center->x, r->y.high));

        new_ranges->insert(new_ranges->end(), {a, b, c, d});
      }

      // Delete old ranges as they are no longer needed.
      if (data.ranges) {
        for (Range2D *old_range : *data.ranges) {
          delete old_range;
        }
        delete data.ranges;
      }
      data.ranges = new_ranges;
      delete data.dst;
      data.dst = (*data.ranges)[0]->dst((*data.ranges)[2]);
      iterate = data.dst->x > target->x * 2 || data.dst->y > target->y * 2;
    }
  }

  delete target;
  delete squarified;

  return data;
}

return_points InternalBinderTree::means_from_ranges(range_data range_data) {
  return_points res {};

  res.points = std::vector<record> {};
  res.dst = range_data.dst;

  for (Range2D *q : *range_data.ranges) {
    std::vector<record> points = this->search(q);

    if (!points.empty()) {
      if (range_data.dst->x != SIZE_X) {
        Point *center = q->center();
        res.points.push_back({{center->x, center->y}, high_median(points)});
      } else {
        res.points = points;
      }
    }
  }

  return res;
}

return_points InternalBinderTree::zoom_search(Range2D *q, int lvl) {
  const range_data ranges = this->ranges(q, lvl);
  return this->means_from_ranges(ranges);
}
