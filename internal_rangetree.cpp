//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#include "internal_rangetree.h"

Point::Point(double x, double y) {
  this->x = x;
  this->y = y;
}

Point::Point() {
  this->x = -1;
  this->y = -1;
}

Point *Point::dst(Point *other) {
  double x_dst, y_dst;
  Point *dst;

  x_dst = std::abs(other->x - this->x);
  y_dst = std::abs(other->y - this->y);
  dst = new Point(x_dst, y_dst);

  return dst;
}

Point *Point::abs_subtr(Point *other) const {
  double subtr_x, subtr_y;

  subtr_x = std::abs(this->x - other->x);
  subtr_y = std::abs(this->y - other->y);

  return new Point(subtr_x, subtr_y);
}

Point *Point::operator/(int rhs) {
  return new Point(this->x / rhs, this->y / rhs);
}

Range::Range(double low, double high) {
  this->low = low;
  this->high = high;
}

Range::Range() {
  this->low = -1;
  this->high = -1;
}

bool Range::contains(Range *val) {
  return val->low >= this->low && val->high <= this->high;
}

bool Range::contains(double val) {
  return val >= this->low && val <= this->high;
}

bool Range::is_disjoint_from(Range *val) {
  return val->high < this->low || val->low > this->high;
}

double Range::len() {
  return std::abs(this->high - this->low);
}

Range2D::Range2D(Point *low, Point *high) {
  this->x = Range(low->x, high->x);
  this->y = Range(low->y, high->y);
}

Point *Range2D::dim() {
  return new Point(this->x.len(), this->y.len());
}

Point *Range2D::center() {
  Point *halfdim = *(this->dim()) / 2;
  return new Point(this->x.low + halfdim->x, this->y.low + halfdim->y);
}

Point *Range2D::dst(Range2D *other) {
  return this->center()->abs_subtr(other->center());
}

node *InternalTree::build(std::vector<record> &points, int d, std::vector<record>::iterator l, std::vector<record>::iterator r) {
   size_t n = r - l;

   size_t m = static_cast<int>(n / 2);

   record k = points[m];

   auto x = new node(k);
   if (n != 1) {
     x->l = this->build(points, d, l, l + m);
     x->r = this->build(points, d, l + m, r);
   }

   if (d != 0) {
      // We are in the second dimension.
      // Merge the left, k and right.
      std::vector<record> merged = {l, r};
      // Sort the array according to d-1 value.
      std::sort(merged.begin(), merged.end(), [d](record &a, record &b) {
          return a.p[d-1] < b.p[d-1];
      });
      // Build the auxilary tree from data in merged for d-1.
      x->aux = this->build(merged, d-1, merged.begin(), merged.end());
   }

   return x;
}

InternalTree::InternalTree(std::vector<record> points) {
  std::sort(points.begin(), points.end(), [](record &a, record &b) {
    return a.p[1] < b.p[1];
  });

  this->root = this->build(points, 1, points.begin(), points.end());
}

bool InternalTree::is_external(node *x) {
  return x->l == nullptr && x->r == nullptr;
}

void InternalTree::leaves_in_subtree(node *x, std::vector<node *> &leaves) {
  if (x != nullptr) {
    this->leaves_in_subtree(x->l, leaves);
    if (this->is_external(x)) {
      leaves.push_back(x);
    }
    this->leaves_in_subtree(x->r, leaves);
  }
}

void InternalTree::search_1D(Range *q, node *p, Range *c, std::vector<node *> &nodes) {
  if (p != nullptr) {
    if (this->is_external(p)) {
      if (q->contains(p->p[0])) {
        nodes.push_back(p);
      }
    } else if (q->contains(c)) {
      this->leaves_in_subtree(p, nodes);
    } else if (!q->is_disjoint_from(c)) {
      this->search_1D(q, p->l, new Range(c->low, p->p[0]), nodes);
      this->search_1D(q, p->r, new Range(p->p[0], c->high), nodes);
    }
  }
}

void InternalTree::search_2D(Range2D *q, node *p, Range *c, std::vector<node *> &nodes) {
  if (p != nullptr) {
    if (this->is_external(p) && q->y.contains(p->p[1])) {
      nodes.push_back(p);
    } else if (q->y.contains(c)) {
      this->search_1D(&(q->x), p->aux, new Range(-1000, 1000), nodes);
    } else if (!q->y.is_disjoint_from(c)) {
      this->search_2D(q, p->l, new Range(c->low, p->p[1]), nodes);
      this->search_2D(q, p->r, new Range(p->p[1], c->high), nodes);
    }
  }
}

std::vector<record> InternalTree::search(Range2D *q) {
  std::vector<node *> nodes {};

  this->search_2D(q, this->root, new Range(-1000, 1000), nodes);

  std::vector<record> points {};
  for (auto node : nodes) {
    points.push_back({node->p, node->lvl});
  }

  return points;
}

void InternalTree::destroy(node *x) {
  if (x != nullptr) {
    this->destroy(x->l);

    this->destroy(x->aux);
    delete x;

    this->destroy(x->r);
  }
}

InternalTree::~ InternalTree() {
  this->destroy(this->root);
}
