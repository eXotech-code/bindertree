//
// Created by Jakub Piskiewicz on 04/02/2023.
//

#include "rangetree.h"

Point::Point(double x, double y) {
  this->x = x;
  this->y = y;
}

Point::Point() {
  this->x = -1;
  this->y = -1;
}

Point *Point::dst(Point *other){
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

Node::Node(double key, double x, double y, long lvl, Node *nil) {
  this->key = key;
  this->point = Point(x, y);
  this->lvl = lvl;
  this->nil = nil;
  this->left = nil;
  this->right = nil;
  this->p = nil;
  this->color = BLACK;
  this->aux = nullptr;
}

Node::Node(Node *obj) {
  this->key = obj->key;
  this->point = obj->point; // TODO: Check if this is by value (I think so).
  this->lvl = obj->lvl;
  this->nil = obj->nil;
  this->left = this->nil;
  this->right = this->nil;
  this->p = this->nil;
  this->color = BLACK;
  this->aux = nullptr;
}

Node::Node() {
  this->key = 0;
  this->point = Point(0, 0);
  this->lvl = -1;
  this->nil = nullptr;
  this->left = nullptr;
  this->right = nullptr;
  this->p = nullptr;
  this->color = BLACK;
  this->aux = nullptr;
}

bool Node::is_external() {
  return this->left == this->nil && this->right == this->nil;
}

double Node::x() const {
  return this->point.x;
}

double Node::y() const {
  return this->point.y;
}

Node *Node::get_y_keyed() {
  return new Node(
      this->y(),
      this->x(),
      this->y(),
      this->lvl,
      this->nil
  );
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
  Point *halfdim;

  halfdim = *(this->dim()) / 2;
  return new Point(this->x.low + halfdim->x, this->y.low + halfdim->y);
}

Point *Range2D::dst(Range2D *other) {
  return this->center()->abs_subtr(other->center());
}

InternalTree::InternalTree(struct record *data, int &len) {
  this->nil = new Node();
  this->root = this->nil;
  this->build(data, len);
}

InternalTree::InternalTree(std::vector<Node *> nodes, Node *nil) {
  // This assumes that nil is a valid Node.
  this->nil = nil;
  this->root = this->nil;
  this->build(nodes);
}

void InternalTree::left_rotate(Node *x) {
  Node *y;

  y = x->right;
  x->right = y->left;
  if (y->left != this->nil) {
    y->left->p = x;
  }
  y->p = x->p;
  if (x->p == this->nil) {
    this->root = y;
    this->root->p = this->nil;
  } else if (x == x->p->left) {
    x->p->left = y;
  } else {
    x->p->right = y;
  }
  y->left = x;
  x->p = y;
}

void InternalTree::right_rotate(Node *x) {
  Node *y;

  y = x->left;
  x->left = y->right;
  if (y->right != this->nil) {
    y->right->p = x;
  }
  y->p = x->p;
  if (x->p == this->nil) {
    this->root = y;
    this->root->p = this->nil;
  } else if (x == x->p->right) {
    x->p->right = y;
  } else {
    x->p->left = y;
  }
  y->right = x;
  x->p = y;
}

void InternalTree::insert_fixup(Node *x) {
  Node *y;

  while (x->p->color == RED) {
    if (x->p == x->p->p->left) {
      y = x->p->p->right;
      if (y->color == RED) {
        x->p->color = BLACK;
        y->color = BLACK;
        x->p->p->color = RED;
        x = x->p->p;
      } else if (x == x->p->right) {
        x = x->p;
        this->left_rotate(x);
      } else {
        x->p->color = BLACK;
        x->p->p->color = RED;
        this->right_rotate(x->p->p);
      }
    } else {
      y = x->p->p->left;
      if (y->color == RED) {
        x->p->color = BLACK;
        y->color = BLACK;
        x->p->p->color = RED;
        x = x->p->p;
      } else if (x == x->p->left) {
        x = x->p;
        this->right_rotate(x);
      } else {
        x->p->color = BLACK;
        x->p->p->color = RED;
        this->left_rotate(x->p->p);
      }
    }
  }
  this->root->color = BLACK;
}

void InternalTree::insert(Node *x) {
  Node *y, *z;

  y = this->nil;
  z = this->root;
  while(z != this->nil) {
    y = z;
    if (x->key < z->key) {
      z = z->left;
    } else {
      z = z->right;
    }
  }
  x->p = y;
  if (y == this->nil) {
    this->root = x;
  } else if (x->key < y->key) {
    y->left = x;
  } else {
    y->right = x;
  }
  x->left = this->nil;
  x->right = this->nil;
  x->color = RED;
  this->insert_fixup(x);
}

void InternalTree::fin(Node *x) {
  if (x != this->nil && !x->is_external()) {
    this->fin(x->left);
    this->insert(new Node(x));
    this->fin(x->right);
  }
}

void InternalTree::leaves_in_subtree(Node *x, std::vector<Node *> &leaves) {
  if (x != this->nil) {
    this->leaves_in_subtree(x->left, leaves);
    if (x->is_external()) {
      leaves.push_back(x);
    }
    this->leaves_in_subtree(x->right, leaves);
  }
}

void InternalTree::add_auxilaries(Node *x) {
  if (x != this->nil) {
    std::vector<Node *> aux_data;

    this->add_auxilaries(x->left);
    this->leaves_in_subtree(x, aux_data);
    for (Node* & node : aux_data) {
      node = node->get_y_keyed();
    }
    /* TODO: Instead of building a new InternalTree object
     * just build it using this tree's build() and assign
     * a pointer to a new root node to x-> aux. */
    x->aux = new InternalTree(aux_data, this->nil);
    this->add_auxilaries(x->right);
  }
}

void InternalTree::build(struct record *data, int &len) {
  for (int i = 0; i < len; i++) {
    Node *x;

    struct record rec  = data[i];
    x = new Node(rec.x, rec.x, rec.y, rec.lvl, this->nil);
    this->insert(x);
  }

  this->fin(this->root);
  this->add_auxilaries(this->root);
}

void InternalTree::build(const std::vector<Node *> nodes) {
  for (Node *node : nodes) {
    Node *x;

    x = new Node(node);
    this->insert(x);
  }

  this->fin(this->root);
}

void InternalTree::search_1D(Range *q, Node *p, Range *c, std::vector<Node *> &nodes) {
  if (p != this->nil) {
    if (p->is_external()) {
      if (q->contains(p->key)) {
        nodes.push_back(p);
      }
    } else if (q->contains(c)) {
      this->leaves_in_subtree(p, nodes);
    } else if (!q->is_disjoint_from(c)) {
      this->search_1D(q, p->left, new Range(c->low, p->key), nodes);
      this->search_1D(q, p->right, new Range(p->key, c->high), nodes);
    }
  }
}

void InternalTree::search_2D(Range2D *q, Node *p, Range *c, std::vector<Node *> &nodes) {
  if (p != this->nil) {
    if (p->is_external()) {
      if (q->x.contains(p->key)) {
        nodes.push_back(p);
      }
    } else if (q->x.contains(c)) {
      InternalTree *aux_tree;

      aux_tree = (InternalTree *)p->aux;
      this->search_1D(&(q->y), aux_tree->root, new Range(-1000, 1000), nodes);
    } else if (!q->x.is_disjoint_from(c)) {
      this->search_2D(q, p->left, new Range(c->low, p->key), nodes);
      this->search_2D(q, p->right, new Range(p->key, c->high), nodes);
    }
  }
}

void InternalTree::search(Range2D *q, std::vector<Node *> &nodes) {
  this->search_2D(q, this->root, new Range(-1000, 1000), nodes);
}

RangeTree::RangeTree(PyObject *args) {
  struct record *data;
  int len;

  data = this->read_records(args, len);
  this->internal_tree = new InternalTree(data, len);
}

struct record *RangeTree::read_records(PyObject *args, int &len) {
  PyObject *pythonList;
  struct record *records;
  struct record erroneous_record = { -1, -1, -1 };

  if (!PyArg_ParseTuple(args, "O", &pythonList)) {
    return nullptr;
  }

  len = PyObject_Length(pythonList);
  if (len < 0) {
    return nullptr;
  }
  records = (record *)malloc(sizeof(struct record *) * len);
  if (records == nullptr) {
    return nullptr;
  }
  for (int i = 0; i < len; i++) {
    PyObject *objectList;
    objectList = PyList_GetItem(pythonList, i);
    if (!PyList_Check(objectList)) {
      records[i] = erroneous_record;
    } else {
      double *doubleList;
      PyObject *lvlObj;
      struct record new_record = {};

      doubleList = (double *)malloc(sizeof(double *) * 2);
      for (int j = 0; j < 2; j++) {
        PyObject *val;
        val = PyList_GetItem(objectList, j);
        if (!PyFloat_Check(val)) {
          records[i] = erroneous_record;
          break;
        }
        doubleList[j] = PyFloat_AsDouble(val);
      }

      new_record.x = doubleList[0];
      new_record.y = doubleList[1];
      lvlObj = PyList_GetItem(objectList, 2);
      if (!PyLong_Check(lvlObj)) {
        records[i] = erroneous_record;
      }
      new_record.lvl = PyLong_AsLong(lvlObj);
      records[i] = new_record;
    }
  }

  return records;
}

int RangeTree::add_double(double x, PyObject *list, int index) {
  PyObject *newf;

  newf = PyFloat_FromDouble(x);
  if (!newf) {
    return -1;
  }

  return PyList_SetItem(list, index, newf);
}

int RangeTree::add_long(long x, PyObject *list, int index) {
  PyObject *newl;

  newl = PyLong_FromLong(x);
  if (!newl) {
    return -1;
  }

  return PyList_SetItem(list, index, newl);
}

PyObject *RangeTree::search(PyObject *args) {
  double xmin, xmax, ymin, ymax;
  Point *low, *high;
  Range2D *q;
  PyObject *pylist;

  PyArg_ParseTuple(args, "dddd", &xmin, &xmax, &ymin, &ymax);
  low = new Point(xmin, ymin);
  high = new Point(xmax, ymax);
  q = new Range2D(low, high);
  std::vector<Node *> nodes = {};
  this->internal_tree->search(q, nodes);

  pylist = PyList_New((Py_ssize_t)nodes.size());
  if (!pylist) {
    return nullptr;
  }
  for (int i = 0; i < nodes.size(); i++) {
    PyObject *node_content;
    Node *x;

    node_content = PyList_New(3);
    if (!node_content) {
      return nullptr;
    }
    x = nodes[i];
    if (this->add_double(x->x(), node_content, 0) == -1) {
      return nullptr;
    }
    if (this->add_double(x->y(), node_content, 1) == -1) {
      return nullptr;
    }
    if (this->add_long(x->lvl, node_content, 2) == -1) {
      return nullptr;
    }
    if (PyList_SetItem(pylist, i, node_content) == -1) {
      return nullptr;
    }
  }

  return pylist;
}

PyObject *RangeTree_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  RangeTreeObject *self;

  self = (RangeTreeObject*)type->tp_alloc(type, 0);
  if (self != nullptr) {
    self->m_rangetree = nullptr;
  }

  return (PyObject *)self;
}

int RangeTree_init(PyObject *self, PyObject *args, PyObject *kwds) {
  RangeTreeObject *m;

  m = (RangeTreeObject *)self;
  m->m_rangetree = (RangeTree *)PyObject_Malloc(sizeof(RangeTree));
  if (!m->m_rangetree) {
    PyErr_SetString(PyExc_RuntimeError, "Memory allocation for RangeTree object failed");
    return -1;
  }

  m->m_rangetree = new RangeTree(args);
  return 0;
}

void RangeTree_dealloc(RangeTreeObject *self) {
  PyTypeObject *tp;
  RangeTreeObject* m;

  tp = Py_TYPE(self);
  m = reinterpret_cast<RangeTreeObject *>(self);

  if (m->m_rangetree) {
    PyObject_Free(m->m_rangetree);
  }

  tp->tp_free(self);
  Py_DECREF(tp);
}

PyObject *RangeTree_search(PyObject *self, PyObject *args) {
  RangeTreeObject *m;

  m = reinterpret_cast<RangeTreeObject *>(self);

  return m->m_rangetree->search(args);
}

static PyModuleDef rangetree_def = {
    PyModuleDef_HEAD_INIT,
    "RangeTree",
    "A range tree with support for generating map zoom levels.",
    -1,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

PyMODINIT_FUNC PyInit_rangetree(void) {
  PyObject *module, *rangetree_class;

  module = PyModule_Create(&rangetree_def);
  rangetree_class = PyType_FromSpec(&rangetree_spec);
  if (!rangetree_class) {
    return nullptr;
  }
  Py_INCREF(rangetree_class);
  if (PyModule_AddObject(module, "RangeTree", rangetree_class) == -1) {
    Py_DECREF(rangetree_class);
    Py_DECREF(module);
    return nullptr;
  }

  return module;
}
