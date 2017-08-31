#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <node.h>

void SequenceGet(const v8::FunctionCallbackInfo<v8::Value> &);
void SequenceSet(const v8::FunctionCallbackInfo<v8::Value> &);
void SequenceSize(const v8::FunctionCallbackInfo<v8::Value> &);
void SequenceDel(const v8::FunctionCallbackInfo<v8::Value> &);

#endif