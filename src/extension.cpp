#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <App.h>
#include <iostream>

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

PyMODINIT_FUNC PyInit_uwebsocketspy() {

    printf("Compiled against Python version: %d.%d.%d\n", PY_MAJOR_VERSION, PY_MINOR_VERSION, PY_MICRO_VERSION);

    //Py_TYPE(&AppType) = &PyType_Type;
    //Py_TYPE(&HttpResponseType) = &PyType_Type;

    Py_INCREF(&HttpResponseType);
    Py_INCREF(&AppType);
    Py_INCREF(&WebSocketType);

    /* Ready all types */
    if (PyType_Ready(&AppType) < 0)
        return NULL;

    if (PyType_Ready(&WebSocketType) < 0)
        return NULL;

    if (PyType_Ready(&HttpResponseType) < 0)
        return NULL;

    /* Init module */
    PyObject *m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    /* App as top level class */
    if (PyModule_AddObject(m, "App", (PyObject *) &AppType) < 0) {
        Py_DECREF(&AppType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
