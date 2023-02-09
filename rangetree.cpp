//
// Created by Jakub Piskiewicz on 04/02/2023.
//

#include "rangetree.h"

int add_double(double x, PyObject *list, int index) {
  PyObject *newf = PyFloat_FromDouble(x);
  if (!newf) {
    return -1;
  }

  return PyList_SetItem(list, index, newf);
}

int add_long(long x, PyObject *list, int index) {
  PyObject *newl = PyLong_FromLong(x);
  if (!newl) {
    return -1;
  }

  return PyList_SetItem(list, index, newl);
}

std::vector<struct record> read_records(PyObject *args) {
  PyObject *pythonList = nullptr;

  if (!PyArg_ParseTuple(args, "O", &pythonList)) {
    return {};
  }

  const int len = PyObject_Length(pythonList);
  if (len < 0) {
    return {};
  }

  auto records = std::vector<struct record>(len);
  for (int i = 0; i < len; i++) {
    PyObject *objectList = PyList_GetItem(pythonList, i);
    if (!PyList_Check(objectList)) {
      return {};
    } else {
      struct record new_record = {};
      std::array<double, 2>doubleList = {0, 0};

      for (int j = 0; j < 2; j++) {
        PyObject *val = PyList_GetItem(objectList, j);
        if (!PyFloat_Check(val)) {
          return {};
        }
        doubleList[j] = PyFloat_AsDouble(val);
      }

      new_record.x = doubleList[0];
      new_record.y = doubleList[1];
      PyObject *lvlObj = PyList_GetItem(objectList, 2);
      if (!PyLong_Check(lvlObj)) {
        return {};
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
  const std::vector<struct record> data = read_records(args);
  if (data.empty()) {
    PyErr_SetString(
        PyExc_RuntimeError,
        "Got empty list of points from read_records"
        );
  }
  this->internal_tree = new InternalTree(data);
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

BinderTree::BinderTree(PyObject *args) {
  std::vector<struct record> data;

  data = read_records(args);
  this->internal_tree = new InternalBinderTree(data);
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

PyObject *RangeTree_search(PyObject *self, PyObject *args) {
    RangeTreeObject *m;

    m = reinterpret_cast<RangeTreeObject *>(self);

    return m->m_rangetree->search(args);
}

PyObject *BinderTree_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    BinderTreeObject *self;

    self = (BinderTreeObject*)type->tp_alloc(type, 0);
    if (self != nullptr) {
      self->m_bindertree = nullptr;
    }

    return (PyObject *)self;
}

int BinderTree_init(PyObject *self, PyObject *args, PyObject *kwds) {
    BinderTreeObject *m;

    m = (BinderTreeObject *)self;
    m->m_bindertree = (BinderTree *)PyObject_Malloc(sizeof(BinderTree));
    if (!m->m_bindertree) {
      PyErr_SetString(PyExc_RuntimeError, "Memory allocation for RangeTree object failed");
      return -1;
    }

    m->m_bindertree = new BinderTree(args);

    return 0;
}

void BinderTree_dealloc(BinderTreeObject *self) {
    PyTypeObject *tp;
    BinderTreeObject *m;

    tp = Py_TYPE(self);
    m = reinterpret_cast<BinderTreeObject *>(self);

    if (m->m_bindertree) {
      PyObject_Free(m->m_bindertree);
    }

    tp->tp_free(self);
    Py_DECREF(tp);
}

PyObject *BinderTree_search(PyObject *self, PyObject *args) {
    BinderTreeObject *m;

    m = reinterpret_cast<BinderTreeObject *>(self);

    return m->m_bindertree->search(args);
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

int add_class(PyType_Spec *spec, PyObject *py_module) {
    PyObject *new_class = PyType_FromSpec(spec);
    if (!new_class) {
      return -1;
    }
    Py_INCREF(new_class);
    if (PyModule_AddObject(py_module, spec->name, new_class) == -1) {
      Py_DECREF(new_class);
      Py_DECREF(py_module);
      return -1;
    }

    return 0;
}

PyMODINIT_FUNC PyInit_rangetree(void) {
  PyObject *py_module;

  py_module = PyModule_Create(&rangetree_def);

  if (add_class(&rangetree_spec, py_module) == -1) {
      return nullptr;
  }
  if (add_class(&bindertree_spec, py_module) == -1) {
      return nullptr;
  }

  return py_module;
}
