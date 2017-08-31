#include <node.h>
#include <Python.h>

#include "globals.h"
#include "resolver.h"
#include "protocols/mapping.h"
#include "protocols/object.h"
#include "protocols/sequence.h"

#include <vector>
#include <string>

#include <iostream>

#pragma message("Using Python " PY_VERSION)

PyObject *pModules;

v8::Global<v8::Function> errorConstructor;

void HandleError(v8::Isolate *isolate) {
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *exception, *v, *tb;

  PyErr_Fetch(&exception, &v, &tb);
  if (exception == NULL)
    return;
    
  PyErr_NormalizeException(&exception, &v, &tb);
  if (tb == NULL) {
    tb = Py_None;
    Py_INCREF(tb);
  }

  #if PY_MAJOR_VERSION == 3
  PyException_SetTraceback(v, tb);
  #endif

  if (exception == NULL)
    return;

  PyObject *traceback = PyImport_Import(PYSTRING_FROMSTRING("traceback"));

  PyObject *format_exception = PyObject_GetAttrString(traceback, "format_exception");

  if (format_exception && PyCallable_Check(format_exception)) {
    PyObject *pValue = PyObject_CallFunctionObjArgs(format_exception, exception, v, tb, NULL);

    v8::Local<v8::Value> argv[2];
    argv[0] = v8::Exception::Error(v8::String::NewFromUtf8(isolate, PYSTRING_TOSTRING(PyObject_Str(v))));
    argv[1] = ResolveToV8(pValue, isolate);

    isolate->ThrowException(errorConstructor.Get(isolate)->NewInstance(context, 2, argv).ToLocalChecked());

  }
}

void Init(const v8::FunctionCallbackInfo<v8::Value>& args) {
  errorConstructor = v8::Global<v8::Function>(args.GetIsolate(), v8::Local<v8::Function>::Cast(args[0]));
}

void Cleanup(const v8::FunctionCallbackInfo<v8::Value>& args) {
  errorConstructor.Reset();
}

void Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::Array> arr = v8::Array::New(isolate, 3);

  arr->Set(context, 0, v8::Number::New(isolate, PY_MAJOR_VERSION));
  arr->Set(context, 1, v8::Number::New(isolate, PY_MINOR_VERSION));
  arr->Set(context, 2, v8::Number::New(isolate, PY_MICRO_VERSION)); 

  args.GetReturnValue().Set(arr);
}

void Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *name, *module;

  v8::Local<v8::String> v8str = args[0]->ToString(context).ToLocalChecked();
  v8::String::Utf8Value utf8(v8str);

  name = PYSTRING_FROMSTRING(*utf8);
  /* Error checking of pName left out */

  module = PyImport_Import(name);
  if (module == NULL) {
    HandleError(isolate);
    return;
  }

  Py_DECREF(name);

  PyDict_SetItem(pModules, PYSTRING_FROMSTRING((std::string("__require_x_module__") + *utf8).c_str()), module);

  args.GetReturnValue().Set(v8::Number::New(isolate, (double)reinterpret_cast<intptr_t>(module)));
}

void Call(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (pObj && PyCallable_Check(pObj)) {
    PyObject *pArgs;
    v8::Local<v8::Value> kw;
    if (args[args.Length() - 1]->IsObject() && std::string("Kwargs").compare(*v8::String::Utf8Value(v8::Local<v8::Object>::Cast(args[args.Length() - 1])->GetConstructorName())) == 0) {
      pArgs = PyTuple_New(args.Length() - 2);
      kw = args[args.Length() - 1];
    } else {
      pArgs = PyTuple_New(args.Length() - 1);
      kw = v8::Local<v8::Value>::Cast(v8::Object::New(isolate));
    }

    for (int i = 0; i < args.Length() - 1; i++) {
      if (!(i == args.Length() - 2 && args[i + 1]->IsObject() && std::string("Kwargs").compare(*v8::String::Utf8Value(v8::Local<v8::Object>::Cast(args[i + 1])->GetConstructorName())) == 0)) {
        PyTuple_SetItem(pArgs, i, ResolveToPy(args[i + 1], isolate));
      }
    }

    PyObject *pValue = PyObject_Call(pObj, pArgs, ResolveToPy(kw, isolate));
    if (pValue == NULL) {
      HandleError(isolate);
      return;
    }

    args.GetReturnValue().Set(ResolveToV8(pValue, isolate));
  }
}

void ToString(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  args.GetReturnValue().Set(ResolveToV8(PyObject_Str(pObj), isolate));
}

void Eval(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::String> v8str = args[0]->ToString(context).ToLocalChecked();
  v8::String::Utf8Value utf8(v8str);

  PyObject *pGlobal = PyDict_New();
  PyObject *pLocal = PyDict_New();

  PyDict_Merge(pLocal, pModules, 0);

  for (int i = 0; i < args.Length() - 1; i++) {
    PyDict_SetItem(pLocal, PYSTRING_FROMSTRING((std::string("__require_x_argument__") + std::to_string(i)).c_str()), ResolveToPy(args[i + 1], isolate));
  }

  PyObject *pValue = PyRun_String(*utf8, Py_eval_input, pLocal, pGlobal);
  if (pValue == NULL) {
    HandleError(isolate);
    return;
  }

  args.GetReturnValue().Set(ResolveToV8(pValue, isolate));
}

void Exec(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::String> v8str = args[0]->ToString(context).ToLocalChecked();
  v8::String::Utf8Value utf8(v8str);

  PyObject *pGlobal = PyDict_New();
  PyObject *pLocal = PyDict_New();

  PyDict_Merge(pLocal, pModules, 0);

  for (int i = 0; i < args.Length() - 1; i++) {
    PyDict_SetItem(pLocal, PYSTRING_FROMSTRING((std::string("__require_x_argument__") + std::to_string(i)).c_str()), ResolveToPy(args[i + 1], isolate));
  }

  PyObject *pValue = PyRun_String(*utf8, Py_file_input, pLocal, pGlobal);
  if (pValue == NULL) {
    HandleError(isolate);
    return;
  }

  args.GetReturnValue().Set(ResolveToV8(pGlobal, isolate));
}

void init(v8::Local<v8::Object> exports) {
  Py_Initialize();

  if (!PyEval_ThreadsInitialized()) {
    PyEval_InitThreads();
  }

  pModules = PyDict_New();

  NODE_SET_METHOD(exports, "init", Init);
  NODE_SET_METHOD(exports, "cleanup", Cleanup);
  NODE_SET_METHOD(exports, "version", Version);
  NODE_SET_METHOD(exports, "load", Load);
  NODE_SET_METHOD(exports, "call", Call);
  NODE_SET_METHOD(exports, "toString", ToString);
  NODE_SET_METHOD(exports, "pyEval", Eval);
  NODE_SET_METHOD(exports, "pyExec", Exec);

  NODE_SET_METHOD(exports, "objectGet", ObjectGet);
  NODE_SET_METHOD(exports, "objectSet", ObjectSet);
  NODE_SET_METHOD(exports, "objectHas", ObjectHas);
  NODE_SET_METHOD(exports, "objectDel", ObjectDel);

  #if PY_MAJOR_VERSION == 3
  NODE_SET_METHOD(exports, "objectKeys", ObjectKeys);
  #endif

  NODE_SET_METHOD(exports, "mappingGet", MappingGet);
  NODE_SET_METHOD(exports, "mappingSet", MappingSet);
  NODE_SET_METHOD(exports, "mappingHas", MappingHas);
  NODE_SET_METHOD(exports, "mappingKeys", MappingKeys);
  NODE_SET_METHOD(exports, "mappingDel", MappingDel);

  NODE_SET_METHOD(exports, "sequenceGet", SequenceGet);
  NODE_SET_METHOD(exports, "sequenceSet", SequenceSet);
  NODE_SET_METHOD(exports, "sequenceSize", SequenceSize);
  NODE_SET_METHOD(exports, "sequenceDel", SequenceDel);
}

NODE_MODULE(python, init)
