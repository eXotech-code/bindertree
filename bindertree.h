//
// Created by Jakub Piskiewicz on 05/02/2023.
//

#ifndef RANGETREE_RANGETREE_H
#define RANGETREE_RANGETREE_H

#include "internal_rangetree.h"
#include "internal_bindertree.h"
#include <Python.h>
#include <array>

int add_double(double x, PyObject *list, int index);
int add_long(long x, PyObject *list, int index);
PyObject *vector_to_pylist(std::vector<Node *> &vec);
std::vector<struct record> read_records(PyObject *args);

// Python-facing front-end of the binder tree.
class BinderTree {
public:
  explicit BinderTree(PyObject *args);
  PyObject *search(PyObject *args);
  PyObject *zoom_search(PyObject *args);
private:
  InternalBinderTree *internal_tree;
};

typedef struct {
  PyObject_HEAD;
  BinderTree *m_bindertree;
} BinderTreeObject;

PyObject *BinderTree_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int BinderTree_init(PyObject *self, PyObject *args, PyObject *kwds);
void BinderTree_dealloc(BinderTreeObject *self);
PyObject *BinderTree_zoomsearch(PyObject *self, PyObject *args);
PyObject *BinderTree_search(PyObject *self, PyObject *args);

static PyMethodDef BinderTree_methods[] = {
    {"zoom_search", BinderTree_zoomsearch, METH_VARARGS, "Searches the binder tree and returns data binded for given zoom lvl."},
    {"search", BinderTree_search, METH_VARARGS, "Calls search method of the parent class and returns a list of points on the base lvl."},
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
