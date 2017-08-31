#include <node.h>
#include <Python.h>

#include <string>

#include "globals.h"

v8::Local<v8::Value> ResolveToV8(PyObject *pObj, v8::Isolate *isolate) {
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  if (pObj == NULL) {
    return v8::Local<v8::Value>::Cast(v8::Undefined(isolate));
  } else if (pObj == Py_None) {
    return v8::Local<v8::Value>::Cast(v8::Null(isolate));
  } else if (pObj == Py_True) {
    return v8::Local<v8::Value>::Cast(v8::True(isolate));
  } else if (pObj == Py_False) {
    return v8::Local<v8::Value>::Cast(v8::False(isolate));
  } else if (PyLong_Check(pObj)) {
    return v8::Local<v8::Value>::Cast(v8::Number::New(isolate, PyLong_AsDouble(pObj)));
  } else if (PyFloat_Check(pObj)) {
    return v8::Local<v8::Value>::Cast(v8::Number::New(isolate, PyFloat_AsDouble(pObj)));
  } 
  
  #if PY_MAJOR_VERSION == 2
  else if (PyInt_Check(pObj)) {
    return v8::Local<v8::Value>::Cast(v8::Number::New(isolate, PyInt_AsLong(pObj)));
  } 
  #endif

  else if (PYSTRING_CHECK(pObj)) {
    return v8::Local<v8::Value>::Cast(v8::String::NewFromUtf8(isolate, PYSTRING_TOSTRING(pObj)));
  } else if (PyCallable_Check(pObj)) {
    v8::Local<v8::Function> obj = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value>& args) {}).ToLocalChecked();
    v8::Local<v8::Object> internalObj = v8::Object::New(isolate);

    intptr_t ptr = reinterpret_cast<intptr_t>(pObj);

    internalObj->Set(context, v8::String::NewFromUtf8(isolate, "address"), v8::Number::New(isolate, (double)ptr));
    obj->Set(context, v8::String::NewFromUtf8(isolate, "__require_x__"), internalObj);

    return v8::Local<v8::Value>::Cast(obj);
  } else if (PySequence_Check(pObj)) {
    v8::Local<v8::Object> obj = v8::Array::New(isolate);
    v8::Local<v8::Object> internalObj = v8::Object::New(isolate);

    intptr_t ptr = reinterpret_cast<intptr_t>(pObj);

    internalObj->Set(context, v8::String::NewFromUtf8(isolate, "address"), v8::Number::New(isolate, (double)ptr));
    obj->Set(context, v8::String::NewFromUtf8(isolate, "__require_x__"), internalObj);

    return v8::Local<v8::Value>::Cast(obj);
  } else {
    v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Object> internalObj = v8::Object::New(isolate);

    intptr_t ptr = reinterpret_cast<intptr_t>(pObj);

    internalObj->Set(context, v8::String::NewFromUtf8(isolate, "address"), v8::Number::New(isolate, (double)ptr));
    obj->Set(context, v8::String::NewFromUtf8(isolate, "__require_x__"), internalObj);

    return v8::Local<v8::Value>::Cast(obj);
  }
}

PyObject *ResolveToPy(v8::Local<v8::Value> val, v8::Isolate *isolate) {
  if (val->IsNull()) {
    return Py_None;
  } else if (val->IsUndefined()) {
    return Py_None;
  } else if (val->IsTrue()) {
    return Py_True;
  } else if (val->IsFalse()) {
    return Py_False;
  } else if (val->IsNumber()) {
    v8::Local<v8::Number> num = v8::Local<v8::Number>::Cast(val);
    return PyFloat_FromDouble(num->Value());
  } else if (val->IsString()) {
    v8::Local<v8::String> str = v8::Local<v8::String>::Cast(val);
    v8::String::Utf8Value utf8(str);
    return PYSTRING_FROMSTRING(*utf8);
  } else if (val->IsArray()) {
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(val);
    v8::Local<v8::Value> internalObj = arr->Get(v8::String::NewFromUtf8(isolate, "__require_x__"));

    if (!internalObj->IsUndefined()) {
      return reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(v8::Local<v8::Object>::Cast(internalObj)->Get(v8::String::NewFromUtf8(isolate, "address")))->Value());
    }

    PyObject *pList = PyList_New(arr->Length());

    for (unsigned int i = 0; i < arr->Length(); i++) {
      PyList_SetItem(pList, i, ResolveToPy(arr->Get(i), isolate));
    }

    return pList;
  } else if (val->IsObject()) {
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(val);
    v8::Local<v8::Value> internalObj = obj->Get(v8::String::NewFromUtf8(isolate, "__require_x__"));

    if (!internalObj->IsUndefined()) {
      return reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(v8::Local<v8::Object>::Cast(internalObj)->Get(v8::String::NewFromUtf8(isolate, "address")))->Value());
    }

    v8::Local<v8::Array> propertyNames = obj->GetOwnPropertyNames();

    PyObject *pDict = PyDict_New();

    for (unsigned int i = 0; i < propertyNames->Length(); i++) {
      v8::Local<v8::Value> key = propertyNames->Get(i);
      v8::Local<v8::Value> val = obj->Get(key);

      PyDict_SetItem(pDict, ResolveToPy(key, isolate), ResolveToPy(val, isolate));
    }

    return pDict;
  } else {
    return Py_None;
  }
}