//
// Created by Jakub Piskiewicz on 05/02/2023.
//

#ifndef RANGETREE_RANGETREE_H
#define RANGETREE_RANGETREE_H

#include "internal_rangetree.h"
#include "internal_bindertree.h"
#include <Python.h>

int add_double(double x, PyObject *list, int index);
int add_long(long x, PyObject *list, int index);
PyObject *vector_to_pylist(std::vector<Node *> &vec);
struct record *read_records(PyObject *args, int &len);

// Python-facing front-end of the range tree.
class RangeTree {
public:
  explicit RangeTree(PyObject *args);
  PyObject *search(PyObject *args);
private:
  InternalTree *internal_tree;
};

// Python-facing front-end of the binder tree.
class BinderTree {
public:
  explicit BinderTree(PyObject *args);
  PyObject *search(PyObject *args);
private:
  InternalBinderTree *internal_tree;
};

typedef struct {
  PyObject_HEAD;
  RangeTree *m_rangetree;
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

typedef struct {
  PyObject_HEAD;
  BinderTree *m_bindertree;
} BinderTreeObject;

PyObject *BinderTree_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int BinderTree_init(PyObject *self, PyObject *args, PyObject *kwds);
void BinderTree_dealloc(BinderTreeObject *self);
PyObject *BinderTree_search(PyObject *self, PyObject *args);

static PyMethodDef BinderTree_methods[] = {
    {"search", BinderTree_search, METH_VARARGS, "Searches the binder tree and returns data binded for given zoom lvl."},
    {nullptr, nullptr, 0, nullptr}
};

static PyType_Slot BinderTree_slots[] = {
    {Py_tp_new, (void *)BinderTree_new},
    {Py_tp_init, (void *)BinderTree_init},
    {Py_tp_dealloc, (void *)BinderTree_dealloc},
    {Py_tp_methods, BinderTree_methods},
    {0, nullptr}
};

static PyType_Spec bindertree_spec = {
    "BinderTree",
    sizeof(BinderTreeObject) + sizeof(BinderTree),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    BinderTree_slots
};

int add_class(PyType_Spec *spec);

#endif // RANGETREE_RANGETREE_H
