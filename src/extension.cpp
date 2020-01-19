#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <App.h>
#include <iostream>

#include "HttpRequestWrapper.h"
#include "HttpResponseWrapper.h"
#include "WebSocketWrapper.h"
#include "AppWrapper.h"

/* Module definition */
static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "uwebsocketspy",
    .m_doc = "uWebSockets.py",
    .m_size = -1,
};

#include <uv.h>

// guesstimate
typedef struct {
    PyObject_HEAD
	uv_loop_t *loop;
} LoopObject;

// integrate with uvloop
static PyObject *print_loop(HttpResponseObject *self, PyObject *args) {

	PyObject *one;
	PyArg_ParseTuple(args, "O", &one);

    LoopObject *loopObject = (LoopObject *) one;

    PyTypeObject *typeObject = one->ob_type;

    printf("type name is: %s\n", typeObject->tp_name);
    printf("item size is: %d\n", typeObject->tp_itemsize);
    printf("loop is maybe instead: %p\n", loopObject->loop);

//    uWS::Loop::get(loop);

	return Py_None;
}

static PyMethodDef methods[] = {
    {"print_loop", (PyCFunction) print_loop, METH_VARARGS, "no doc"},
    {NULL}
};

PyMODINIT_FUNC PyInit_uwebsocketspy() {

    printf("Compiled against Python version: %d.%d.%d\n", PY_MAJOR_VERSION, PY_MINOR_VERSION, PY_MICRO_VERSION);
    printf("Compiled against libuv version: %d.%d.%d\n", UV_VERSION_MAJOR, UV_VERSION_MINOR, UV_VERSION_PATCH);
    printf("Linked against libuv version: %s\n", uv_version_string());

    Py_INCREF(&HttpResponseType);
    Py_INCREF(&AppType);
    Py_INCREF(&WebSocketType);
    Py_INCREF(&HttpRequestType);

    /* Ready all types */
    if (PyType_Ready(&AppType) < 0)
        return NULL;

    if (PyType_Ready(&WebSocketType) < 0)
        return NULL;

    if (PyType_Ready(&HttpResponseType) < 0)
        return NULL;

    if (PyType_Ready(&HttpRequestType) < 0)
        return NULL;

    /* Init module */
    PyObject *m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    /* The module itself need a few functions */
    PyModule_AddFunctions(m, methods);

    /* App as top level class */
    if (PyModule_AddObject(m, "App", (PyObject *) &AppType) < 0) {
        Py_DECREF(&AppType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
