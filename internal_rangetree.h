//
// Created by Jakub Piskiewicz on 08/02/2023.
//

#ifndef RANGETREE_INTERNAL_RANGETREE_H
#define RANGETREE_INTERNAL_RANGETREE_H

#include <cmath>
#include <vector>

// One record in the noise file.
struct record {
  double x;
  double y;
  long lvl;
};

constexpr int BLACK = 0;
constexpr int RED = 1;

class Point {
public:
  double x;
  double y;
  Point(double x, double y);
  Point();
  Point *dst(Point *other);
  Point *abs_subtr(Point *other) const;
  Point *operator/(int rhs);
};

class Node {
public:
  double key;
  Point point {};
  long lvl;
  Node *left;
  Node *right;
  Node *p;
  int color;
  void *aux;
  Node(double key, double x, double y, long lvl, Node *nil);
  Node(Point *loc, long lvl); // This is Node returned by the zoom_search in InternalBinderTree.
  explicit Node(Node *obj); // Copy constructor.
  Node();
  bool is_external();
  [[nodiscard]] double x() const;
  [[nodiscard]] double y() const;
  Node *get_y_keyed(); // Swaps the key to y from x.

private:
  Node *nil;
};

class Range {
public:
  double low;
  double high;
  Range(double low, double high);
  Range();
  bool contains(double val);
  bool contains(Range *val);
  bool is_disjoint_from(Range *val);
  double len();
};

class Range2D {
public:
  Range x {};
  Range y {};
  Range2D(Point *low, Point* high);
  Point *dim();
  Point *center();
  Point *dst(Range2D *other)  ;
};

class InternalTree {
public:
  explicit InternalTree(const std::vector<struct record> &data);
  InternalTree(const std::vector<Node *> &nodes, Node *nil);
  void search(Range2D *q, std::vector<Node *> &nodes);

private:
  Node *nil = new Node();
  Node *root = nil;
  void left_rotate(Node *x);
  void right_rotate(Node *y);
  void insert_fixup(Node *x);
  void insert(Node *x);
  void fin(Node *x);
  void add_auxilaries(Node *x);
  void build(const std::vector<struct record> &data);
  void build(const std::vector<Node *> &nodes);
  void leaves_in_subtree(Node *x, std::vector<Node *> &leaves);
  void search_1D(Range *q, Node *p, Range *c, std::vector<Node *> &nodes);
  void search_2D(Range2D *q, Node *p, Range *c, std::vector<Node *> &nodes);
};

#endif // RANGETREE_INTERNAL_RANGETREE_H
