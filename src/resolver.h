#include <node.h>
#include <Python.h>

v8::Local<v8::Value> ResolveToV8(PyObject*, v8::Isolate*);
PyObject *ResolveToPy(v8::Local<v8::Value> val);