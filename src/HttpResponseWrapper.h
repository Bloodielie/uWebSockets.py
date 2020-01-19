typedef struct {
    PyObject_HEAD
	uWS::HttpResponse<false> *res;
} HttpResponseObject;

static PyObject *HttpResponse_end(HttpResponseObject *self, PyObject *args) {

	PyObject *one;
	PyArg_ParseTuple(args, "O", &one);

	// string to utf8
	Py_ssize_t size;
	const char *str = PyUnicode_AsUTF8AndSize(one, &size);
	std::string_view message(str, size);

	self->res->end(message);
	return Py_None;
}

// fast call
static PyObject *HttpResponse_fastEnd(HttpResponseObject *self, PyObject **args, int nargs) {

	PyObject *one = args[0];

	// string to utf8
	Py_ssize_t size;
	const char *str = PyUnicode_AsUTF8AndSize(one, &size);
	std::string_view message(str, size);

	self->res->end(message);
	return Py_None;
}

static PyObject *HttpResponse_writeHeader(HttpResponseObject *self, PyObject *args) {

	PyObject *key, *value;
	PyArg_ParseTuple(args, "OO", &key, &value);

	// string to utf8
	Py_ssize_t size;
	const char *str = PyUnicode_AsUTF8AndSize(key, &size);
	std::string_view keyString(str, size);

	str = PyUnicode_AsUTF8AndSize(value, &size);
	std::string_view valueString(str, size);

	self->res->writeHeader(keyString, valueString);
	return Py_None;
}

// HttpResponse method list
static PyMethodDef HttpResponse_methods[] = {
    {"end", (PyCFunction) HttpResponse_end, METH_VARARGS, "no doc"},
	{"fastEnd", (PyCFunction) HttpResponse_fastEnd, METH_FASTCALL, "no doc"},
	{"writeHeader", (PyCFunction) HttpResponse_writeHeader, METH_VARARGS, "no doc"},
    {NULL}
};

static PyTypeObject HttpResponseType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "HttpResponse",
    .tp_basicsize = sizeof(HttpResponseObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "no doc",
    .tp_methods = HttpResponse_methods,
    .tp_new = PyType_GenericNew,
};