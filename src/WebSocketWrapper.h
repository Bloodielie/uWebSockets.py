typedef struct {
    PyObject_HEAD
	uWS::WebSocket<false, true> *ws;
} WebSocketObject;

static PyObject *WebSocket_end(WebSocketObject *self, PyObject *args) {
	return Py_None;
}

static PyObject *WebSocket_send(WebSocketObject *self, PyObject **args, int nargs) {

	PyObject *pythonMessage = args[0];

	// string to utf8
	Py_ssize_t size;
	const char *str = PyUnicode_AsUTF8AndSize(pythonMessage, &size);
	std::string_view message(str, size);

    // bara om redan utf8
    //auto *utf8 = PyUnicode_DATA(pythonMessage);
    //Py_ssize_t size = PyUnicode_GetLength(pythonMessage);

    //Py_buffer *buffer = PyMemoryView_GET_BUFFER(pythonMessage);
	//self->ws->send({(char *) buffer->buf, buffer->len}, uWS::OpCode::TEXT);

    self->ws->send({str, size}, uWS::OpCode::TEXT);
	return Py_None;
}

static PyMethodDef WebSocket_methods[] = {
    {"end", (PyCFunction) WebSocket_end, METH_VARARGS, "no doc"},
    {"send", (PyCFunction) WebSocket_send, METH_FASTCALL, "no doc"},
    {NULL}
};

static PyTypeObject WebSocketType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "WebSocket",
    .tp_basicsize = sizeof(WebSocketObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "no doc",
    .tp_methods = WebSocket_methods,
    .tp_new = PyType_GenericNew,
};