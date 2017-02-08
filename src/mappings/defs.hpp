#ifndef _MAPPINGS_DEFS_
#define _MAPPINGS_DEFS_

#include "nan.h"

using namespace v8;
using namespace node;
#include "../pointer.hpp"
#include "../format.h"

#define getPointer(type) type* m = _getPointer<type>(info.This());\
if(m != nullptr)

#define checkValue(type) MaybeLocal<type> _newValue = Nan::To<type>(info.Data());\
if(_newValue.IsEmpty()) {\
    Nan::ThrowError(Nan::Error("Invalid value: Has to be a " #type));\
} else

#define getValue(type) Nan::To<type>(_newValue.ToLocalChecked()).FromJust()

#define SetGetter(name) Nan::SetAccessor(obj, Nan::New( #name ).ToLocalChecked(), name)
#define SetGetterSetter(name) Nan::SetAccessor(obj, Nan::New( #name ).ToLocalChecked(), name, name)

namespace flac_bindings {
    template<typename T>
    static T* _getPointer(const Local<Object> &thiz) {
        Nan::MaybeLocal<Value> maybeLocal = Nan::Get(thiz, Nan::New("_ptr").ToLocalChecked());
        if(maybeLocal.IsEmpty()) {
            Nan::ThrowError(Nan::Error("Object is not a Metadata Object"));
        } else {
            Local<Value> val = maybeLocal.ToLocalChecked();
            if(!val->IsObject()) {
                Nan::ThrowError(Nan::Error("Corrupted Metadata object: Pointer is not an object"));
            } else {
                Local<Object> obj = val.As<Object>();
                if(!Buffer::HasInstance(obj)) {
                    Nan::ThrowError(Nan::Error("Corrupted Metadata object: Pointer is invalid"));
                } else {
                    return UnwrapPointer<T>(obj);
                }
            }
        }
        return nullptr;
    }
};

#endif
