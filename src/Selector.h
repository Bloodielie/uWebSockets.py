/* Implements the Python event loop */

#include <uv.h>

typedef struct {
    PyObject_HEAD
	uv_loop_t *loop;
    PyTypeObject *namedTuple;
    PyObject *list;
} SelectorObject;

int read_mask = 1;
int write_mask = 2;

// vi behöver en fd -till PyObject mapper som håller SelectionKey från fd
#include <map>

std::map<int, PyObject *> polls;
std::map<int, uv_poll_t *> uv_polls;

// poll_cb bygger upp vektorn att ge tillbaka

// sel.register(conn, selectors.EVENT_READ, read)
// integer eller object med fileno, bitmask, userdata (obj)
static PyObject *Selector_register(SelectorObject *self, PyObject **args, int nargs) {

    /* Is first argument an integer? */
    int fd = -1;
    if (PyLong_Check(args[0])) {
        fd = PyLong_AsLong(args[0]);
        printf("FD är: %d\n", fd);
    } else {
        printf("Första är inte fd!\n");
        PyObject *fileobj = args[0];

        PyObject *fdObj = PyObject_CallMethod(fileobj, "fileno", NULL);
        if (PyLong_Check(fdObj)) {
            fd = PyLong_AsLong(fdObj);
            printf("FD från object är: %d\n", fd);
        } else {
            // throw exception and return null
        }
    }

    int mask = 0;
    if (PyLong_Check(args[1])) {
        mask = PyLong_AsLong(args[1]);
        printf("bitmask är: %d\n", mask);
    }

    printf("Register called with fd: %d mask: %d\n", fd, mask);

    // SelectorKey
    //fileobj, fd, events, data

    PyObject *namedTuple = PyStructSequence_New(self->namedTuple);

    Py_INCREF(args[0]);


    PyStructSequence_SetItem(namedTuple, 0, args[0]);
    PyStructSequence_SetItem(namedTuple, 1, PyLong_FromLong(fd));
    PyStructSequence_SetItem(namedTuple, 2, PyLong_FromLong(mask));

    // optional - kolla längden på args!
    if (nargs == 3) {
        printf("Vi har user data!\n");
        Py_INCREF(args[2]);
        PyStructSequence_SetItem(namedTuple, 3, args[2]);
    }

    // kolla om vi redan har denna fd eller lägg till
    Py_INCREF(namedTuple); // behövs den? efter new? ja för vi sparar den hos oss också
    polls.insert({fd, namedTuple});


    // registrera den själva grejen, peka till pyobjektet
    // vi kan nå SelectorKey:n genom fd också
    uv_poll_t *p = new uv_poll_t;
    uv_poll_init_socket(self->loop, p, fd);


    // starta pollning enligt mask
    uv_poll_start(p, mask, [](uv_poll_t *handle, int status, int events) {
        // show fd for debugging
        uv_os_fd_t fd;
        uv_fileno((uv_handle_t *)handle, &fd);
        //printf("Poll med FD: %d är redo!\n", fd);

        // get loop and from that SelectorObject
        uv_loop_t *loop = uv_handle_get_loop((uv_handle_t *) handle);
        SelectorObject *self = (SelectorObject *) loop->data;

        // make tuple of SelectorKEy and events
        PyObject *tuple = PyTuple_Pack(2, (PyObject *) handle->data, PyLong_FromLong(events));
        Py_INCREF(tuple);
        // add it to this iteration list
        PyList_Append(self->list, tuple);
    });


    uv_polls.insert({fd, p});

    Py_INCREF(namedTuple);
    p->data = namedTuple;
    return namedTuple;
}

static PyObject *Selector_unregister(SelectorObject *self, PyObject **args, int nargs) {

    int fd = -1;
    if (PyLong_Check(args[0])) {
        fd = PyLong_AsLong(args[0]);
        printf("FD är: %d\n", fd);
    } else {
        printf("Första är inte fd!\n");
        PyObject *fileobj = args[0];

        PyObject *fdObj = PyObject_CallMethod(fileobj, "fileno", NULL);
        if (PyLong_Check(fdObj)) {
            fd = PyLong_AsLong(fdObj);
            printf("FD från object är: %d\n", fd);
        } else {
            // throw exception and return null
        }
    }

    uv_poll_t *p = uv_polls[fd];

    uv_poll_stop(p);
    uv_close((uv_handle_t *) p, [](uv_handle_t *p) {
        delete (uv_poll_t *) p;
    });

    // tar fd eller fileno-obj som key

    printf("Unregister called\n");


    uv_polls.erase(fd);
    polls.erase(fd);



    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Selector_modify(SelectorObject *self, PyObject *args) {

    printf("Modify called\n");

    Py_INCREF(Py_None);
    return Py_None;
}

// returns list of triggered events

/* This only returns to Python when there are Python events */
static PyObject *Selector_select(SelectorObject *self, PyObject **args, int nargs) {
    if (nargs == 1) {
            //printf("Select called with timeout: %d\n", PyLong_AsLong(args[0]));

            int timeout = PyLong_AsLong(args[0]);
            if (timeout == 0) {
                // important to just give it empty list when timeout is 0
                PyObject *list = self->list;

                self->list = PyList_New(0);

                Py_INCREF(list);
                return list;
            }

    } else {
            //printf("Select called\n");
    }

    while (true) {

        // if we have no polls just return
        /*if (polls.size() == 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }*/

        int still = uv_run(self->loop, UV_RUN_ONCE);

        //printf("One loop iteration!\n");
        //break;

        //if (!still)
            //break;

        /* If we have any ready polls for Python we have to return them now */
        if (PyList_Size(self->list)) {
            break;
        }
    }



    // This returns a list of (key, events) tuples, one for each ready file object.
    // key is the SelectorKey instance corresponding to a ready file object. events is a bitmask of events ready on this file object.

    // vi kan ha en lista för-allokerad som pollsen fyller i, appendar till
    // pollsen måste veta om sin SelectorKey
    // (SelectorKey, events), ()

    PyObject *list = self->list;

    self->list = PyList_New(0);

    Py_INCREF(list);
    return list;
}

static PyObject *Selector_get_map(SelectorObject *self, PyObject *args) {

    printf("Get_map called\n");



    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Selector_get_key(SelectorObject *self, PyObject *args) {

    printf("Get_key called\n");

    int fd;
    int ok = PyArg_ParseTuple(args, "i", &fd);

    printf("ok: %d, fd: %d\n", ok, fd);

    //PyExc_KeyError

    PyErr_SetString(PyExc_KeyError, "yo!");
    return NULL;


    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Selector_close(SelectorObject *self, PyObject *args) {

    printf("Close called\n");


    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Selector_methods[] = {

    {"register", (PyCFunction) Selector_register, METH_FASTCALL, "no doc"},
    {"unregister", (PyCFunction) Selector_unregister, METH_FASTCALL, "no doc"},
    {"modify", (PyCFunction) Selector_modify, METH_VARARGS, "no doc"},
    {"select", (PyCFunction) Selector_select, METH_FASTCALL, "no doc"},
    {"get_key", (PyCFunction) Selector_get_key, METH_VARARGS, "no doc"},
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

        self->list = PyList_New(0);

        // create tuple type
        self->namedTuple = PyStructSequence_NewType(&desc);

        printf("Named tuple is: %p\n", self->namedTuple);

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