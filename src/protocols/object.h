#ifndef OBJECT_H
#define OBJECT_H

#include <node.h>

void ObjectGet(const v8::FunctionCallbackInfo<v8::Value> &);
void ObjectSet(const v8::FunctionCallbackInfo<v8::Value> &);
void ObjectHas(const v8::FunctionCallbackInfo<v8::Value> &);
void ObjectDel(const v8::FunctionCallbackInfo<v8::Value> &);

#if PY_MAJOR_VERSION == 3
void ObjectKeys(const v8::FunctionCallbackInfo<v8::Value> &);
#endif

#endif