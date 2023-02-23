//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#include "internal_bindertree.h"

int high_median(std::vector<record> &points) {
  const long mid = ceil(points.size() / 2);

  return points[mid].lvl;
}

double InternalBinderTree::greater_len(Range2D &q) {
  const double len_x = q.x.len();
  const double len_y = q.y.len();

  if (len_x > len_y) {
    return len_x;
  }
  return len_y;
}

Range2D InternalBinderTree::squareify(Range2D &q) {
  const double len_side = this->greater_len(q);
  Point high = {q.x.low + len_side, q.y.low + len_side / 2};

  return Range2D(Point(q.x.low, q.y.low), high);
}

Point InternalBinderTree::target(int zoom_lvl) {
  /* How much the distance between points on the map changed
   * compared to MAX_ZOOM. */
  const auto magnf = (float)(MAX_ZOOM - zoom_lvl) * 2;
  return Point(SIZE_X * magnf, SIZE_Y * magnf);
}

struct range_data InternalBinderTree::ranges(Range2D &q, int zoom_lvl) {
  Point target = this->target(zoom_lvl);
  Range2D squarified = this->squareify(q);
  struct range_data data = {
    {Range2D(squarified)},
    {SIZE_X, SIZE_Y}
  };

  if (!(target.x <= SIZE_X || target.y <= SIZE_Y)) {
    bool iterate = true;

    while (iterate) {
      std::vector<Range2D> new_ranges {};
      for (Range2D &r : data.ranges) {
        Point center = r.center();

        // bottom-left
        Range2D a = {{r.x.low, r.y.low}, center};
        // bottom-right
        Range2D b = {{center.x, r.y.low}, {r.x.high, center.y}};
        // top-right
        Range2D c = {center, {r.x.high, r.y.high}};
        // top-left
        Range2D d = {{r.x.low, center.y}, {center.x, r.y.high}};

        new_ranges.insert(new_ranges.end(), {a, b, c, d});
      }

      // Delete old ranges as they are no longer needed.
      data.ranges = new_ranges;
      data.dst = data.ranges[0].dst(data.ranges[2]);
      iterate = data.dst.x > target.x * 2 || data.dst.y > target.y * 2;
    }
  }

  return data;
}

return_points InternalBinderTree::means_from_ranges(range_data range_data) {
  return_points res {};

  res.points = std::vector<record> {};
  res.dst = range_data.dst;

  for (Range2D &q : range_data.ranges) {
    std::vector<record> points = this->search(q);

    if (!points.empty()) {
      if (range_data.dst.x != SIZE_X) {
        Point center = q.center();
        res.points.push_back({{center.x, center.y}, high_median(points)});
      } else {
        res.points = points;
      }
    }
  }

  return res;
}

return_points InternalBinderTree::zoom_search(Range2D &q, int lvl) {
  const range_data ranges = this->ranges(q, lvl);
  return this->means_from_ranges(ranges);
}
