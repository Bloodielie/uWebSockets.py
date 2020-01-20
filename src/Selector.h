/* Implements the Python event loop */

#include <uv.h>

typedef struct {
    PyObject_HEAD
	uv_loop_t *loop;
} SelectorObject;

// poll_cb bygger upp vektorn att ge tillbaka

// sel.register(conn, selectors.EVENT_READ, read)
// integer eller object med fileno, bitmask, userdata (obj)
static PyObject *Selector_register(SelectorObject *self, PyObject *args) {

    printf("Register called\n");

    return Py_None;
}

static PyObject *Selector_unregister(SelectorObject *self, PyObject *args) {

    printf("Unregister called\n");

    return Py_None;
}

static PyObject *Selector_modify(SelectorObject *self, PyObject *args) {

    printf("Modify called\n");

    return Py_None;
}

// returns list of triggered events

/* This only returns to Python when there are Python events */
static PyObject *Selector_select(SelectorObject *self, PyObject **args, int nargs) {

    printf("Select called\n");


    //while(true) {
        int remaining = uv_run(self->loop, UV_RUN_ONCE);


    //}


    // de polls som anropats har byggt upp en vektor vi kan ge tillbaka nu

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Selector_get_key(SelectorObject *self, PyObject **args, int nargs) {

    printf("Get_key called\n");


    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Selector_methods[] = {

    {"register", (PyCFunction) Selector_register, METH_VARARGS, "no doc"},
    {"unregister", (PyCFunction) Selector_unregister, METH_VARARGS, "no doc"},
    {"modify", (PyCFunction) Selector_modify, METH_VARARGS, "no doc"},
    {"select", (PyCFunction) Selector_select, METH_FASTCALL, "no doc"},
    {"get_key", (PyCFunction) Selector_get_key, METH_FASTCALL, "no doc"},

    {NULL}
};

static PyObject *Selector_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    SelectorObject *self = (SelectorObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
	    self->loop = uv_default_loop();
    }
    Py_INCREF(self);
    return (PyObject *) self;
}

static PyTypeObject SelectorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "uWS.Selector",
    .tp_basicsize = sizeof(SelectorObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "no doc",
    .tp_methods = Selector_methods,
    .tp_new = Selector_new,
};