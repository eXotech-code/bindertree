//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#ifndef RANGETREE_INTERNAL_RANGETREE_H
#define RANGETREE_INTERNAL_RANGETREE_H

#include <vector>
#include <cmath>
#include <algorithm>

// One record in the noise file.
struct record {
  std::vector<double> p;
  int lvl;
};

class Point {
public:
  double x;
  double y;
  Point(double x, double y);
  Point(); // WHAT IS THIS FOR???
  Point dst(Point &other);
  Point abs_subtr(Point other);
  Point operator/(int rhs);
};

class Range {
public:
  double low;
  double high;
  Range(double low, double high);
  Range(); // What for??
  bool contains(double val);
  bool contains(Range &val);
  bool is_disjoint_from(Range &val);
  double len();
};

class Range2D {
public:
  Range2D(Point low, Point high) : x(Range(low.x, high.x)), y(Range(low.y, high.y)) {};
  Range x;
  Range y;
  Point dim();
  Point center();
  Point dst(Range2D other)  ;
};

struct node {
  std::vector<double> p;
  int lvl;
  node *l; // left child
  node *r; // right child
  node *aux; // pointer to root of auxilary tree (only relevant for nodes in first dim).
  node(record k) : p(k.p), lvl(k.lvl), l(nullptr), r(nullptr), aux(nullptr) {};
};

class InternalTree {
public:
  InternalTree(std::vector<record> points);
  std::vector<record> search(Range2D &q);
  ~ InternalTree();
private:
  node *root;
  node *build(std::vector<record> &points, int d, std::vector<record>::iterator l, std::vector<record>::iterator r);
  bool is_external(node *x);
  void leaves_in_subtree(node *x, std::vector<node *> &leaves);
  void search_1D(Range &q, node *p, Range c, std::vector<node *> &nodes);
  void search_2D(Range2D &q, node *p, Range c, std::vector<node *> &nodes);
  // Recursively destroys subtree of x along with the auxilary trees.
  void destroy(node *x);
};

#endif // RANGETREE_INTERNAL_RANGETREE_H
