#include <node.h>
#include <Python.h>

#include "../globals.h"
#include "../resolver.h"

void ObjectGet(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  PyObject *pValue = PyObject_GetAttrString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked()));
  if (pValue == NULL) {
    HandleError(isolate);
    return;
  }

  args.GetReturnValue().Set(ResolveToV8(pValue, isolate));
}

void ObjectSet(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PyObject_SetAttrString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked()), ResolveToPy(args[2], isolate)) == -1) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(v8::True(isolate));
  }
}

void ObjectHas(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PyObject_HasAttrString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked()))) {
    args.GetReturnValue().Set(v8::True(isolate));
  } else {
    args.GetReturnValue().Set(v8::False(isolate));
  }
}

#if PY_MAJOR_VERSION == 3
void ObjectKeys(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());
  PyObject *pDict = PyObject_GenericGetDict(pObj, NULL);

  if (pDict == NULL) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(ResolveToV8(PyDict_Keys(pDict), isolate));
  }
}
#endif

void ObjectDel(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  PyObject *pObj = reinterpret_cast<PyObject *>((intptr_t)v8::Local<v8::Number>::Cast(args[0])->Value());

  if (PyObject_DelAttrString(pObj, *v8::String::Utf8Value(args[1]->ToString(context).ToLocalChecked())) == -1) {
    HandleError(isolate);
  } else {
    args.GetReturnValue().Set(v8::True(isolate));
  }
}