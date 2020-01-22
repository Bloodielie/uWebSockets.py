typedef struct {
    PyObject_HEAD
	uWS::HttpRequest *req;
} HttpRequestObject;

static PyObject *HttpRequest_getHeader(HttpRequestObject *self, PyObject *args) {

	PyObject *one;
	PyArg_ParseTuple(args, "O", &one);

	// string to utf8
	Py_ssize_t size;
	const char *str = PyUnicode_AsUTF8AndSize(one, &size);
	std::string_view header(str, size);

	std::cout << header << " is: " << self->req->getHeader(header) << std::endl;
	
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef HttpRequest_methods[] = {
    {"getHeader", (PyCFunction) HttpRequest_getHeader, METH_VARARGS, "no doc"},
    {NULL}
};

static PyTypeObject HttpRequestType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "HttpRequest",
    .tp_basicsize = sizeof(HttpRequestObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "no doc",
    .tp_methods = HttpRequest_methods,
    .tp_new = PyType_GenericNew,
};