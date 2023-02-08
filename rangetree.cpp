//
// Created by Jakub Piskiewicz on 04/02/2023.
//

#include "rangetree.h"

int add_double(double x, PyObject *list, int index) {
  PyObject *newf;

  newf = PyFloat_FromDouble(x);
  if (!newf) {
    return -1;
  }

  return PyList_SetItem(list, index, newf);
}

int add_long(long x, PyObject *list, int index) {
  PyObject *newl;

  newl = PyLong_FromLong(x);
  if (!newl) {
    return -1;
  }

  return PyList_SetItem(list, index, newl);
}

struct record *read_records(PyObject *args, int &len) {
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

PyObject *vector_to_pylist(std::vector<Node *> &vec) {
  PyObject *pylist;

  pylist = PyList_New((Py_ssize_t)vec.size());
  if (!pylist) {
    return nullptr;
  }
  for (unsigned long i = 0; i < vec.size(); i++) {
    PyObject *node_content;
    Node *x;

    node_content = PyList_New(3);
    if (!node_content) {
      return nullptr;
    }
    x = vec[i];
    if (add_double(x->x(), node_content, 0) == -1) {
      return nullptr;
    }
    if (add_double(x->y(), node_content, 1) == -1) {
      return nullptr;
    }
    if (add_long(x->lvl, node_content, 2) == -1) {
      return nullptr;
    }
    if (PyList_SetItem(pylist, i, node_content) == -1) {
      return nullptr;
    }
  }

  return pylist;
}

RangeTree::RangeTree(PyObject *args) {
  struct record *data;
  int len;

  data = read_records(args, len);
  this->internal_tree = new InternalTree(data, len);
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

  pylist = vector_to_pylist(nodes);

  return pylist;
}

PyObject *RangeTree_search(PyObject *self, PyObject *args) {
  RangeTreeObject *m;

  m = reinterpret_cast<RangeTreeObject *>(self);

  return m->m_rangetree->search(args);
}

BinderTree::BinderTree(PyObject *args) {
  struct record *data;
  int len;

  data = read_records(args, len);
  this->internal_tree = new InternalBinderTree(data, len);
}

PyObject *BinderTree::search(PyObject *args) {
    double xmin, xmax, ymin, ymax;
    long lvl;
    Point *low, *high;
    Range2D *q;
    PyObject *nodelist, *pylist, *sizelist;

    PyArg_ParseTuple(args, "ddddi", &xmin, &xmax, &ymin, &ymax, &lvl);
    low = new Point(xmin, ymin);
    high = new Point(xmax, ymax);
    q = new Range2D(low, high);
    struct zoomed_nodes query_res = this->internal_tree->zoom_search(q, lvl);

    nodelist = vector_to_pylist(*query_res.nodes);
    pylist = PyList_New(2);
    if (!pylist) {
      return nullptr;
    }
    if (PyList_SetItem(pylist, 0, nodelist) == -1) {
      return nullptr;
    }
    sizelist = PyList_New(2);
    if (!sizelist) {
      return nullptr;
    }
    if (add_double(query_res.dst->x, sizelist, 0) == -1) {
      return nullptr;
    }
    if (add_double(query_res.dst->y, sizelist, 1) == -1) {
      return nullptr;
    }
    if (PyList_SetItem(pylist, 1, sizelist) == -1) {
      return nullptr;
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
