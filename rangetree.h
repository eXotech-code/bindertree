//
// Created by Jakub Piskiewicz on 05/02/2023.
//

#ifndef RANGETREE_RANGETREE_H
#define RANGETREE_RANGETREE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <cmath>
#include <vector>
#include <iostream>

#define BLACK 0
#define RED 1

// One record in the noise file.
struct record {
  double x;
  double y;
  long lvl;
};

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
  InternalTree(struct record *data, int &len);
  InternalTree(std::vector<Node *> nodes, Node *nil);
  void search(Range2D *q, std::vector<Node *> &nodes);

private:
  Node *nil;
  Node *root;
  void left_rotate(Node *x);
  void right_rotate(Node *y);
  void insert_fixup(Node *x);
  void insert(Node *x);
  void fin(Node *x);
  void add_auxilaries(Node *x);
  void build(struct record *data, int &len);
  void build(std::vector<Node *> nodes);
  void leaves_in_subtree(Node *x, std::vector<Node *> &leaves);
  void search_1D(Range *q, Node *p, Range *c, std::vector<Node *> &nodes);
  void search_2D(Range2D *q, Node *p, Range *c, std::vector<Node *> &nodes);
};

// Python-facing front-end of the tree.
class RangeTree {
public:
  explicit RangeTree(PyObject *args);
  PyObject *search(PyObject *args);
private:
  InternalTree *internal_tree;
  struct record *read_records(PyObject *args, int &len);
  int add_double(double x, PyObject *list, int index);
  int add_long(long x, PyObject *list, int index);
};

typedef struct {
  PyObject_HEAD;
  RangeTree* m_rangetree;
} RangeTreeObject;

PyObject *RangeTree_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int RangeTree_init(PyObject *self, PyObject *args, PyObject *kwds);
void RangeTree_dealloc(RangeTreeObject *self);
PyObject *RangeTree_search(PyObject *self, PyObject *args);

static PyMethodDef RangeTree_methods[] = {
    {"search", RangeTree_search, METH_VARARGS, "Searches the range tree."},
    {nullptr, nullptr, 0, nullptr}
};

static PyType_Slot RangeTree_slots[] = {
    {Py_tp_new, (void *)RangeTree_new},
    {Py_tp_init, (void *)RangeTree_init},
    {Py_tp_dealloc, (void *)RangeTree_dealloc},
    {Py_tp_methods, RangeTree_methods},
    {0, nullptr}
};

static PyType_Spec rangetree_spec = {
    "RangeTree",
    sizeof(RangeTreeObject) + sizeof(RangeTree),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    RangeTree_slots
};

#endif // RANGETREE_RANGETREE_H
