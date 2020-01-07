typedef struct {
    PyObject_HEAD
	uWS::App *app;
    double kuk;
} AppObject;

static PyObject *App_run(AppObject *self, PyObject *args) {
    self->app->run();

    return Py_None;//return (PyObject *) self;
}

static PyObject *App_listen(AppObject *self, PyObject *args) {
    // parse either port, cb
    // or host, port, cb

    PyObject *callback;
	PyArg_ParseTuple(args, "O", &callback);
    Py_INCREF(callback);

    char *hostname = "localhost";
    int port = 3000;

    self->app->listen(hostname, port, [callback](struct us_listen_socket_t *listenSocket) {
        /* Call python */
        if (!PyObject_CallFunction(callback, NULL)) {
            PyErr_Print();
        }
    });

    return Py_None;//return (PyObject *) self;
}

template <auto F>
static PyObject *App_f(AppObject *self, PyObject *args) {
    /* Parse out route and callback */
    PyObject *callback;
	PyArg_ParseTuple(args, "O", &callback);
    Py_INCREF(callback);

    // todo: move an unique_ptr with custom destructor!

    /* Takes any method pointer as template argument */
    (self->app->*F)("/", [callback](auto *res, auto *req) {
        /* Create the HttpResponse */
        Py_INCREF(&HttpResponseType);
        HttpResponseObject *obj = PyObject_New(HttpResponseObject, &HttpResponseType);
        PyObject *resObj = PyObject_Init((PyObject *) obj, &HttpResponseType);
        obj->res = res;

        /* Create the HttpRequest */
        Py_INCREF(&HttpResponseType);
        HttpResponseObject *obj2 = PyObject_New(HttpResponseObject, &HttpResponseType);
        PyObject *reqObj = PyObject_Init((PyObject *) obj2, &HttpResponseType);
        obj2->res = res;

        /* Call python */
        PyObject *call = PyObject_CallFunction(callback, "OO", resObj, resObj);
        if (!call) {
            PyErr_Print();
        }

        /* Decrement reference counts */
        Py_DECREF(resObj);
        Py_DECREF(reqObj);
    });

    return Py_None;//(PyObject *) self;
}

static PyMethodDef App_methods[] = {
    /* All 9 Http methods */
    {"get", (PyCFunction) App_f<&uWS::App::get>, METH_VARARGS, "no doc"},
    {"post", (PyCFunction) App_f<&uWS::App::post>, METH_VARARGS, "no doc"},
    {"patch", (PyCFunction) App_f<&uWS::App::patch>, METH_VARARGS, "no doc"},
    {"del", (PyCFunction) App_f<&uWS::App::del>, METH_VARARGS, "no doc"},
    {"put", (PyCFunction) App_f<&uWS::App::put>, METH_VARARGS, "no doc"},
    {"head", (PyCFunction) App_f<&uWS::App::head>, METH_VARARGS, "no doc"},
    {"connect", (PyCFunction) App_f<&uWS::App::connect>, METH_VARARGS, "no doc"},
    {"options", (PyCFunction) App_f<&uWS::App::options>, METH_VARARGS, "no doc"},
    {"trace", (PyCFunction) App_f<&uWS::App::trace>, METH_VARARGS, "no doc"},

    {"listen", (PyCFunction) App_listen, METH_VARARGS, "no doc"},
    {"run", (PyCFunction) App_run, METH_NOARGS, "no doc"},

    {NULL}
};

static PyObject *App_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    AppObject *self = (AppObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
	    self->app = new uWS::App();
    }
    return (PyObject *) self;
}

static PyTypeObject AppType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "uWS.App",
    .tp_basicsize = sizeof(AppObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "no doc",
    .tp_methods = App_methods,
    .tp_new = App_new,
};