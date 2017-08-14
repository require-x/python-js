#include <node.h>
#include <Python.h>

#include "resolver.h"

#include <vector>
#include <iostream>

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

bool setup = false;
std::vector<PyObject *> modules;

void Load(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *name, *module;

  if (!setup) {
    Py_Initialize();
    setup = true;
  }

  Local<String> v8str = args[0]->ToString(context).ToLocalChecked();
  String::Utf8Value utf8(v8str);

  name = PyUnicode_DecodeFSDefault(*utf8);
  /* Error checking of pName left out */

  module = PyImport_Import(name);
  Py_DECREF(name);

  int id = modules.size();

  if (module != NULL) {
    modules.push_back(module);
  } else {
    PyErr_Print();
    isolate->ThrowException(v8::Exception::Error(String::NewFromUtf8(isolate, "failed to load module")));
  }

  args.GetReturnValue().Set(v8::Number::New(isolate, id));
}

void Get(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<v8::Context> context = isolate->GetCurrentContext();

  Local<v8::Number> moduleId = Local<v8::Number>::Cast(args[0]);
  PyObject *module = modules[moduleId->Value()];

  Local<String> v8str = args[1]->ToString(context).ToLocalChecked();
  String::Utf8Value utf8(v8str);
  PyObject *pObj = PyObject_GetAttrString(module, *utf8);

  args.GetReturnValue().Set(ResolveToV8(pObj, isolate));
}

void Set(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<v8::Context> context = isolate->GetCurrentContext();

  Local<v8::Number> moduleId = Local<v8::Number>::Cast(args[0]);
  PyObject *module = modules[moduleId->Value()];

  Local<String> v8str = args[1]->ToString(context).ToLocalChecked();
  String::Utf8Value utf8(v8str);
  if (PyObject_SetAttrString(module, *utf8, ResolveToPy(args[2])) == -1) {
    PyErr_Print();
  }

  args.GetReturnValue().Set(ResolveToV8(ResolveToPy(args[2]), isolate));
}

void Has(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<v8::Context> context = isolate->GetCurrentContext();

  Local<v8::Number> moduleId = Local<v8::Number>::Cast(args[0]);
  PyObject *module = modules[moduleId->Value()];

  Local<String> v8str = args[1]->ToString(context).ToLocalChecked();
  String::Utf8Value utf8(v8str);
  if (PyObject_HasAttrString(module, *utf8)) {
    args.GetReturnValue().Set(v8::True(isolate));
  } else {
    args.GetReturnValue().Set(v8::False(isolate));
  }
}

void Keys(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  Local<v8::Number> moduleId = Local<v8::Number>::Cast(args[0]);
  PyObject *module = modules[moduleId->Value()];

  PyObject *pDict = PyObject_GenericGetDict(module, NULL);

  if (pDict == NULL) {
    PyErr_Print();
  } else {
    args.GetReturnValue().Set(ResolveToV8(PyDict_Keys(pDict), isolate));
  }
}

void Del(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<v8::Context> context = isolate->GetCurrentContext();

  Local<v8::Number> moduleId = Local<v8::Number>::Cast(args[0]);
  PyObject *module = modules[moduleId->Value()];

  Local<String> v8str = args[1]->ToString(context).ToLocalChecked();
  String::Utf8Value utf8(v8str);
  if (PyObject_DelAttrString(module, *utf8) == -1) {
    PyErr_Print();
  }
}

void Call(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<v8::Context> context = isolate->GetCurrentContext();

  Local<v8::Number> moduleId = Local<v8::Number>::Cast(args[0]);
  PyObject *module = modules[moduleId->Value()];

  Local<String> v8str = args[1]->ToString(context).ToLocalChecked();
  String::Utf8Value utf8(v8str);
  PyObject *pObj = PyObject_GetAttrString(module, *utf8);

  if (pObj && PyCallable_Check(pObj)) {
    PyObject *pArgs = PyTuple_New(args.Length() - 2);

    for (int i = 0; i < args.Length() - 2; i++) {
      PyTuple_SetItem(pArgs, i, ResolveToPy(args[i + 2]));
    }

    PyObject *pValue = PyObject_CallObject(pObj, pArgs);

    if (pValue == NULL) {
      PyErr_Print();
    }

    args.GetReturnValue().Set(ResolveToV8(pValue, isolate));
  }
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "load", Load);
  NODE_SET_METHOD(exports, "get", Get);
  NODE_SET_METHOD(exports, "set", Set);
  NODE_SET_METHOD(exports, "has", Has);
  NODE_SET_METHOD(exports, "keys", Keys);
  NODE_SET_METHOD(exports, "del", Del);
  NODE_SET_METHOD(exports, "call", Call);
}

NODE_MODULE(python, init)
