#include <node.h>
#include <Python.h>

using v8::Isolate;
using v8::Local;
using v8::Value;

Local<Value> ResolveToV8(PyObject*, Isolate*);
PyObject *ResolveToPy(Local<Value> val);