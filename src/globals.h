#ifndef GLOBALS_H
#define GLOBALS_H

#include <node.h>

extern v8::Global<v8::Function> errorConstructor;

void HandleError(v8::Isolate *);

#endif

#include <Python.h>

#if PY_MAJOR_VERSION == 2
#define PYSTRING_TOSTRING PyString_AsString
#define PYSTRING_FROMSTRING PyString_FromString
#define PYSTRING_CHECK PyString_Check
#else
#define PYSTRING_TOSTRING PyUnicode_AsUTF8
#define PYSTRING_FROMSTRING PyUnicode_FromString
#define PYSTRING_CHECK PyUnicode_Check
#endif