#include <node.h>
#include <Python.h>

#include "../globals.h"
#include "../resolver.h"

#include <iostream>

void SequenceGet(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  PyObject *pValue = PySequence_GetItem(pObj, (Py_ssize_t)v8::Local<v8::Number>::Cast(args[1])->Value());
  if (pValue == NULL) {
    HandleError(isolate);
    return;
  }

  args.GetReturnValue().Set(ResolveToV8(pValue, isolate));
}

void SequenceSet(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PySequence_SetItem(pObj, (Py_ssize_t)v8::Local<v8::Number>::Cast(args[1])->Value(), ResolveToPy(args[2], isolate)) == -1) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(v8::True(isolate));
  }
}

void SequenceSize(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (!PySequence_Check(pObj)) {
    return args.GetReturnValue().Set(v8::Number::New(isolate, 0));
  }

  Py_ssize_t pValue = PySequence_Size(pObj);

  if (pValue == -1) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(v8::Number::New(isolate, (double)pValue));
  }
}

void SequenceDel(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PySequence_DelItem(pObj, (Py_ssize_t)v8::Local<v8::Number>::Cast(args[1])->Value()) == -1) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(v8::True(isolate));
  }
}