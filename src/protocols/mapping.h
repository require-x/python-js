#ifndef MAPPING_H
#define MAPPING_H

#include <node.h>

void MappingGet(const v8::FunctionCallbackInfo<v8::Value> &);
void MappingSet(const v8::FunctionCallbackInfo<v8::Value> &);
void MappingHas(const v8::FunctionCallbackInfo<v8::Value> &);
void MappingKeys(const v8::FunctionCallbackInfo<v8::Value> &);
void MappingDel(const v8::FunctionCallbackInfo<v8::Value> &);

#endif