#include <node.h>
#include <Python.h>

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
  } else if (PyUnicode_Check(pObj)) {
    return v8::Local<v8::Value>::Cast(v8::String::NewFromUtf8(isolate, PyUnicode_AsUTF8(pObj)));
  } else if (PyList_Check(pObj)) {
    v8::Local<v8::Array> arr = v8::Array::New(isolate, PyList_Size(pObj));

    for (int i = 0; i < PyList_Size(pObj); i++) {
      arr->Set(context, i, ResolveToV8(PyList_GetItem(pObj, i), isolate));
    }

    return v8::Local<v8::Value>::Cast(arr);
  } else if (PyTuple_Check(pObj)) {
    v8::Local<v8::Array> arr = v8::Array::New(isolate, PyTuple_Size(pObj));

    for (int i = 0; i < PyTuple_Size(pObj); i++) {
      arr->Set(context, i, ResolveToV8(PyTuple_GetItem(pObj, i), isolate));
    }

    return v8::Local<v8::Value>::Cast(arr);
  } else if (PyDict_Check(pObj)) {
    v8::Local<v8::Object> obj = v8::Object::New(isolate);

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pObj, &pos, &key, &value)) {
      obj->Set(context, ResolveToV8(key, isolate), ResolveToV8(value, isolate));
    }

    return v8::Local<v8::Value>::Cast(obj);
  } else if (PyCallable_Check(pObj)) {
    v8::Local<v8::Function> func = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value>& args) {}).ToLocalChecked();

    return v8::Local<v8::Value>::Cast(func);
  } else {
    return v8::Local<v8::Value>::Cast(v8::Undefined(isolate));
  }
}

PyObject *ResolveToPy(v8::Local<v8::Value> val) {
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
    return PyUnicode_FromString(*utf8);
  } else if (val->IsArray()) {
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(val);
    PyObject *pList = PyList_New(arr->Length());

    for (unsigned int i = 0; i < arr->Length(); i++) {
      PyList_SetItem(pList, i, ResolveToPy(arr->Get(i)));
    }

    return pList;
  } else if (val->IsObject()) {
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(val);
    v8::Local<v8::Array> propertyNames = obj->GetOwnPropertyNames();

    PyObject *pDict = PyDict_New();

    for (unsigned int i = 0; i < propertyNames->Length(); i++) {
      v8::Local<v8::Value> key = propertyNames->Get(i);
      v8::Local<v8::Value> val = obj->Get(key);

      PyDict_SetItem(pDict, ResolveToPy(key), ResolveToPy(val));
    }

    return pDict;
  } else {
    return Py_None;
  }
}