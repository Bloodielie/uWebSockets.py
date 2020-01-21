/* Implements the Python event loop */

#include <uv.h>
#include <map>

#undef NDEBUG
#include <cassert>

typedef struct {
    PyObject_HEAD
    uv_loop_t *loop;
    PyTypeObject *namedTuple;
    /* We build the list of (SelectorKey, events) tuples in iterations */
    PyObject *list;
    int list_length;
} SelectorObject;

/* These map perfectly to uv */
const int read_mask = 1;
const int write_mask = 2;

/* This is the (native) map from fd to uv_poll_t, which points to SelectorKey in userdata */
std::map<int, uv_poll_t *> polls;

/* Takes an integer, or object with method fileno. Returns -1 on invalid input. */
int getFd(PyObject *arg) {
    int fd = -1;
    if (PyLong_Check(arg)) {
        fd = PyLong_AsLong(arg);
    } else {
        PyObject *fdObj = PyObject_CallMethod(arg, "fileno", NULL);
        if (PyLong_Check(fdObj)) {
            fd = PyLong_AsLong(fdObj);
        }
        Py_DECREF(fdObj);
    }
    return fd;
}

/* Takes fileobj, bitmask, userdata. Returns SelectorKey. */
static PyObject *Selector_register(SelectorObject *self, PyObject **args, int nargs) {
    /* First two args are required */
    if (nargs < 2) {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    /* fd */
    int fd = getFd(args[0]);
    if (fd == -1) {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    /* mask */
    int mask = 0;
    if (PyLong_Check(args[1])) {
        mask = PyLong_AsLong(args[1]);
    } else {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    PyObject *selectorKey = PyStructSequence_New(self->namedTuple);
    /* Fileobj */
    Py_INCREF(args[0]);
    PyStructSequence_SetItem(selectorKey, 0, args[0]);
    /* fd */
    PyStructSequence_SetItem(selectorKey, 1, PyLong_FromLong(fd));
    /* mask */
    PyStructSequence_SetItem(selectorKey, 2, PyLong_FromLong(mask));

    /* Third arg is optional */
    if (nargs == 3) {
        /* userdata */
        Py_INCREF(args[2]);
        PyStructSequence_SetItem(selectorKey, 3, args[2]);
    }

    /* Do we already have this fd? */
    if (polls.find(fd) != polls.end()) {
        PyErr_SetString(PyExc_KeyError, "yo!");
        return NULL;
    }

    /* Create the actual poll */
    uv_poll_t *p = new uv_poll_t;
    uv_poll_init_socket(self->loop, p, fd);

    /* The poll points to its SelectorKey */
    p->data = selectorKey;

    /* Start polling according to mask */
    uv_poll_start(p, mask, [](uv_poll_t *handle, int status, int events) {
        /* Get loop from poll, and from that SelectorObject */
        uv_loop_t *loop = uv_handle_get_loop((uv_handle_t *) handle);
        SelectorObject *self = (SelectorObject *) loop->data;

        /* Grab our SelectorKey */
        PyObject *selectorKey = (PyObject *) handle->data;

        PyObject *eventsLong = PyLong_FromLong(events);
        // this increases ref count of all passed objects
        PyObject *tuple = PyTuple_Pack(2, selectorKey, eventsLong);
        Py_DECREF(eventsLong);

        // this does not increase, it steals the existing reference
        PyList_SetItem(self->list, self->list_length++, tuple);
    });

    /* Finally add it to the map */
    polls.insert({fd, p});

    /* Return the SelectorKey */
    Py_INCREF(selectorKey);
    return selectorKey;
}

/* Takes fileobj or integer */
static PyObject *Selector_unregister(SelectorObject *self, PyObject **args, int nargs) {

    /* We require exactly one argument */
    if (nargs != 1) {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    /* We also require that to hold a valid fd */
    int fd = getFd(args[0]);
    if (fd == -1) {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    /* Decrease the SelectorKey refcount */
    uv_poll_t *p = polls[fd];
    PyObject *selectorKey = (PyObject *) p->data;
    Py_DECREF(selectorKey);

    /* Stop and delete the poll */
    uv_poll_stop(p);
    uv_close((uv_handle_t *) p, [](uv_handle_t *p) {
        delete (uv_poll_t *) p;
    });

    /* Remove it from our map */
    polls.erase(fd);

    Py_INCREF(Py_None);
    return Py_None;
}

/* Takes fileobj, events and optionally data. Returns a new SelectorKey */
static PyObject *Selector_modify(SelectorObject *self, PyObject **args, int nargs) {

    /* We require at least 2 args */
    if (nargs < 2) {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    /* We also require first one to be a valid fd */
    int fd = getFd(args[0]);
    if (fd == -1) {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    uv_poll_t *p = polls[fd];

    /* Delete the old selectorKey */
    PyObject *selectorKey = (PyObject *) p->data;
    Py_DECREF(selectorKey);

    /* Create a new one instead */

    Py_INCREF(Py_None);
    return Py_None;
}

/* This only returns to Python when there are Python events, or when the loop has ready handles. */

/* Optionally takes timeout */
static PyObject *Selector_select(SelectorObject *self, PyObject **args, int nargs) {
    if (nargs == 1) {

            int timeout = PyLong_AsLong(args[0]);
            if (timeout == 0) {


                // make it a function
                int high = self->list_length;
                self->list_length = 0;
                return PyList_GetSlice(self->list, 0, high);


            }

    } else {
            //printf("Select called\n");
    }

    /* We want to stay as long as we can in this loop, not yielding to asyncio until we have to */
    while (true) {

        int keepGoing = uv_run(self->loop, UV_RUN_ONCE);
        /* We don't need to keep going if we don't have to */
        if (!keepGoing) {
            break;
        }

        /* If we have any ready polls for Python we have to return them now */
        if (PyList_Size(self->list)) {
            break;
        }

        /* If the event loop has any _ready Handles we need to return to trigger them */
        // todo
    }

    /* Return the ready list of (SelectorKey, events) tuples, make a new one */
    PyObject *list = self->list;


    // make it a function
    int high = self->list_length;
    self->list_length = 0;
    return PyList_GetSlice(self->list, 0, high);
}

/* How is this even a thing? We would need to create a whole map here every time */
static PyObject *Selector_get_map(SelectorObject *self, PyObject *args) {

    printf("Mappen innehåller %d polls\n", polls.size());

    //PyErr_SetString(PyExc_ValueError, "uWS.Selector.get_map not implemented!");
    //return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Selector_get_key(SelectorObject *self, PyObject **args, int nargs) {

    int fd = getFd(args[0]);
    if (fd == -1) {
        PyErr_SetString(PyExc_ValueError, "yo!");
        return NULL;
    }

    auto it = polls.find(fd);
    if (it == polls.end()) {
        PyErr_SetString(PyExc_KeyError, "yo!");
        return NULL;
    }

    PyObject *selectorKey = (PyObject *) it->second->data;

    /* Return the selectorKey */
    Py_INCREF(selectorKey);
    return selectorKey;
}

// essentially like closing the event loop altogether
static PyObject *Selector_close(SelectorObject *self, PyObject *args) {

    printf("Close called\n");


    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Selector_methods[] = {

    {"register", (PyCFunction) Selector_register, METH_FASTCALL, "no doc"},
    {"unregister", (PyCFunction) Selector_unregister, METH_FASTCALL, "no doc"},
    {"modify", (PyCFunction) Selector_modify, METH_FASTCALL, "no doc"},
    {"select", (PyCFunction) Selector_select, METH_FASTCALL, "no doc"},
    {"get_key", (PyCFunction) Selector_get_key, METH_FASTCALL, "no doc"},
    {"get_map", (PyCFunction) Selector_get_map, METH_VARARGS, "no doc"},
    {"close", (PyCFunction) Selector_get_map, METH_FASTCALL, "no doc"},

    {NULL}
};

static PyObject *Selector_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    SelectorObject *self = (SelectorObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
	    self->loop = uv_default_loop();

        PyStructSequence_Field fields[] = {
            {"fileobj", "doc"},
            {"fd", "doc"},
            {"events", "doc"},
            {"data", "doc"},
            {"__module__", "doc"},
            NULL
        };

        PyStructSequence_Desc desc = {
            "uWS.SelectorKey",
            "doc",
            fields,
            4
        };

        /* We start with an empty list */

        /* Let's make a list with a hard limit */
        self->list = PyList_New(1024);
        self->list_length = 0;

        /* Create selectorKey type object */
        self->namedTuple = PyStructSequence_NewType(&desc);

        /* The uv_loop_t points to self in userdata */
        self->loop->data = self;
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