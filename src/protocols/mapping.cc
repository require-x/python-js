#include <node.h>
#include <Python.h>

#include "../globals.h"
#include "../resolver.h"

void MappingGet(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  PyObject *pValue = PyMapping_GetItemString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked()));
  if (pValue == NULL) {
    HandleError(isolate);
    return;
  }

  args.GetReturnValue().Set(ResolveToV8(pValue, isolate));
}

void MappingSet(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PyMapping_SetItemString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked()), ResolveToPy(args[2], isolate)) == -1) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(v8::True(isolate));
  }
}

void MappingHas(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PyMapping_HasKeyString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked()))) {
    args.GetReturnValue().Set(v8::True(isolate));
  } else {
    args.GetReturnValue().Set(v8::False(isolate));
  }
}

void MappingKeys(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (!PyMapping_Check(pObj)) {
    return args.GetReturnValue().Set(v8::Array::New(isolate));
  }

  PyObject *pKeys = PyMapping_Keys(pObj);

  v8::Local<v8::Array> arr = v8::Array::New(isolate, (int)PyList_Size(pKeys));

  for (int i = 0; i < PyList_Size(pKeys); i++) {
    arr->Set(context, i, ResolveToV8(PyList_GetItem(pKeys, i), isolate));
  }
  

  if (pKeys == NULL) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(arr);
  }
}

void MappingDel(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PyMapping_DelItemString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked())) == -1) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(v8::True(isolate));
  }
}