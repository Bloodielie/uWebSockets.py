typedef struct {
    PyObject_HEAD
	uWS::App *app;
} AppObject;

static PyObject *App_run(AppObject *self, PyObject *args) {
    self->app->run();

    return Py_None;//return (PyObject *) self;
}

static PyObject *App_listen(AppObject *self, PyObject *args) {
    /* Takes (hostname), port, callback */
    const char *hostname = "";
    int port = 3000;
    PyObject *callback;

	if (!PyArg_ParseTuple(args, "siO", &hostname, &port, &callback)) {
        printf("Failed to parse hostname, port, callback in listen\n");

        PyErr_Clear();

        if (!PyArg_ParseTuple(args, "iO", &port, &callback)) {
            printf("Failed to parse port, callback in listen\n");

            // should call the callback with empty PyCapsule
            return Py_None;
        }
    }

    printf("port: %d\n", port);

    Py_INCREF(callback);
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
    const char *pattern = "";
    PyObject *callback;
	if (!PyArg_ParseTuple(args, "sO", &pattern, &callback)) {
        // error away
        return Py_None;
    }
    Py_INCREF(callback);

    // todo: move an unique_ptr with custom destructor!

    /* Takes any method pointer as template argument */
    (self->app->*F)(pattern, [callback](auto *res, auto *req) {
        /* Create the HttpResponse */
        Py_INCREF(&HttpResponseType);
        HttpResponseObject *resObjSelf = PyObject_New(HttpResponseObject, &HttpResponseType);
        //PyObject *resObj = PyObject_Init((PyObject *) resObjSelf, &HttpResponseType);
        resObjSelf->res = res;

        /* Create the HttpRequest */
        Py_INCREF(&HttpResponseType);
        HttpRequestObject *reqObjSelf = PyObject_New(HttpRequestObject, &HttpRequestType);
        //PyObject *reqObj = PyObject_Init((PyObject *) reqObjSelf, &HttpRequestType);
        reqObjSelf->req = req;

        /* Call python */
        PyObject *call = PyObject_CallFunction(callback, "OO", resObjSelf, reqObjSelf);
        if (!call) {
            PyErr_Print();
        }

        /* Decrement reference counts */
        Py_DECREF(resObjSelf);
        Py_DECREF(reqObjSelf);
    });

    return Py_None;//(PyObject *) self;
}

static PyObject *App_ws(AppObject *self, PyObject *args) {


    /* Takes a pattern and a dictionary */
    const char *pattern = "";
    PyObject *webSocketBehavior;
	if (!PyArg_ParseTuple(args, "sO", &pattern, &webSocketBehavior)) {
        // error away
        return Py_None;
    }

    if (!PyDict_Check(webSocketBehavior)) {
        // error away?
        return Py_None;
    }

    /* We fill this one */
    uWS::App::WebSocketBehavior behavior = {};

    struct PerSocketData {
        PyObject *self;
    };

    // loopa över alla keys, hantera dem baserat på string?

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(webSocketBehavior, &pos, &key, &value)) {
        /* do something interesting with the values... */

        if (PyUnicode_Check(key)) {
            Py_ssize_t size;
            const char *str = PyUnicode_AsUTF8AndSize(key, &size);
            std::string_view keyString(str, size);

            if (keyString == "open") {
                if (PyCallable_Check(value)) {
                    Py_INCREF(value);
                    behavior.open = [value](auto *ws, auto *req) {
                        // construct the ws and req wrappers here

                        /* Create the WebSocket */
                        Py_INCREF(&WebSocketType);
                        WebSocketObject *webSocket = PyObject_New(WebSocketObject, &WebSocketType);
                        PyObject *wsObj = PyObject_Init((PyObject *) webSocket, &WebSocketType);
                        webSocket->ws = ws;


                        PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();

                        /* Link from our data to the python thing */
                        perSocketData->self = wsObj;
                        Py_INCREF(perSocketData->self); // we need to increase it here as we can be the only holder

                        // link ws->obj = wsObj for later retrieval in message handler!


                        PyObject_CallFunction(value, "OO", wsObj, value);

                        Py_DECREF(wsObj);
                    };
                }
            } else if (keyString == "close") {
                if (PyCallable_Check(value)) {
                    Py_INCREF(value);
                    behavior.close = [value](auto *ws, int code, std::string_view message) {


                        PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();

                        // construct the ws and req wrappers here
                        PyObject_CallFunction(value, "OOO", perSocketData->self, value, value);


                        Py_DECREF(perSocketData->self);
                    };
                }
            } else if (keyString == "message") {
                if (PyCallable_Check(value)) {
                    Py_INCREF(value);
                    behavior.message = [value](auto *ws, std::string_view message, uWS::OpCode opCode) {

                        // ws->getUserData() should hold PyObject *self

                        PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();

                        // is a copy, don't do this! pass a buffer! and neuter it on return
                        PyObject *pythonMessage = PyUnicode_FromStringAndSize(message.data(), message.length());

                        // construct the ws and req wrappers here
                        PyObject_CallFunction(value, "OOO", perSocketData->self, pythonMessage, value);


                        Py_DECREF(pythonMessage);
                    };
                }
            } else if (keyString == "maxPayloadLength") {
                printf("setting maxPAyloadLength!\n");
            } else {
                printf("unhandled key in dict!\n");
            }

        }



    
        
    }

    self->app->ws<PerSocketData>(pattern, std::move(behavior));

    return Py_None;

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

    {"ws", (PyCFunction) App_ws, METH_VARARGS, "no doc"},

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