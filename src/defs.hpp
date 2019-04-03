#ifndef _MAPPINGS_DEFS_
#define _MAPPINGS_DEFS_

#include <tuple>
#include <type_traits>
#include "nan.h"

using namespace v8;
using namespace node;
#include "./pointer.hpp"
#include "./format.h"


#define checkValue(type) MaybeLocal<type> _newValue = Nan::To<type>(value); \
if(_newValue.IsEmpty() || !_newValue.ToLocalChecked()->Is##type ()) { \
    Nan::ThrowError(Nan::Error("Invalid value: Has to be a " #type)); \
} else

#define checkValueIsBuffer() \
    if(!Buffer::HasInstance(value)) { \
        Nan::ThrowTypeError("Expected argument to be Buffer"); \
    } else

#define getValue(type) Nan::To<type>(_newValue.ToLocalChecked()).FromJust()

#define SetGetter(name) Nan::SetAccessor(obj, Nan::New( #name ).ToLocalChecked(), name)
#define SetGetterSetter(name) Nan::SetAccessor(obj, Nan::New( #name ).ToLocalChecked(), name, name)

#define V8_GETTER(methodName) void methodName(Local<Name> property, const PropertyCallbackInfo<Value> &info)
#define V8_SETTER(methodName) void methodName(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info)

#define unwrap(type) if(info.This()->InternalFieldCount() == 0) return; \
type* self = Nan::ObjectWrap::Unwrap<type>(info.This());

#define nativeReadOnlyProperty(obj, name, fnName) \
(void) obj->SetNativeDataProperty(info.GetIsolate()->GetCurrentContext(), Nan::New(name).ToLocalChecked(), fnName, 0, Local<Value>(), (PropertyAttribute) (PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete))

#define nativeProperty(obj, name, fnName) \
(void) obj->SetNativeDataProperty(info.GetIsolate()->GetCurrentContext(), Nan::New(name).ToLocalChecked(), fnName, fnName, Local<Value>(), PropertyAttribute::DontDelete)


#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
template<typename T,
         typename std::enable_if_t<std::is_signed<T>::value, int> = 0>
static inline Nan::Maybe<T> numberFromJs(Local<BigInt> bigNum) {
    if(bigNum->WordCount() > 1) return Nan::Nothing<T>();
    return Nan::Just((T) bigNum->Int64Value());
}

template<typename T,
         typename std::enable_if_t<std::is_unsigned<T>::value, unsigned> = 0>
static inline Nan::Maybe<T> numberFromJs(Local<BigInt> bigNum) {
    if(bigNum->WordCount() > 1) return Nan::Nothing<T>();
    return Nan::Just((T) bigNum->Uint64Value());
}
#endif

template<typename T>
static inline Nan::Maybe<T> numberFromJs(Local<Value> num) {
    if(num.IsEmpty()) return Nan::Nothing<T>();
    if(num->IsNumber()) {
        return Nan::To<T>(num);
    }
#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
    else if(num->IsBigInt()) {
        Local<BigInt> bigNum = num.As<BigInt>();
        return numberFromJs<T>(bigNum);
    }
#endif
    return Nan::Nothing<T>();
}

template<typename T>
static inline Nan::Maybe<T> numberFromJs(MaybeLocal<Value> maybeNum) {
    if(maybeNum.IsEmpty()) return Nan::Nothing<T>();
    else return numberFromJs<T>(maybeNum.ToLocalChecked());
}

template<typename T,
         typename std::enable_if_t<std::is_unsigned<T>::value, unsigned> = 0>
static inline Local<Value> numberToJs(T number, bool forceBigInt = false) {
#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
    if(!forceBigInt && number <= 9007199254740991) {
        return Nan::New<Number>(number);
    }
    return BigInt::NewFromUnsigned(Isolate::GetCurrent(), number);
#else
    return Nan::New<Number>((int64_t) number);
#endif
}

template<typename T,
         typename std::enable_if_t<std::is_signed<T>::value, int> = 0>
static inline Local<Value> numberToJs(T number, bool forceBigInt = false) {
#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
    if(!forceBigInt && -9007199254740991 <= number && number <= 9007199254740991) {
        return Nan::New<Number>(number);
    }
    return BigInt::New(Isolate::GetCurrent(), number);
#else
    return Nan::New<Number>((int64_t) number);
#endif
}



namespace Nan {

#define X(type) \
    template<> \
    struct ToFactory< type > : ValueFactoryBase< type > { \
        static inline return_t convert(v8::Local<v8::Value> val); \
    }; \

    namespace imp {

#if defined(__clang__)
        X(long)
#endif
        X(uint64_t)
        X(FLAC__MetadataType)

    }

#undef X

#define X(type) \
    template<> \
    inline typename imp::ToFactory< type >::return_t To< type >(v8::Local<v8::Value> val) { \
        auto convVal = To<int64_t>(val); \
        if(convVal.IsJust()) { \
            return Just(( type ) convVal.FromJust()); \
        } else { \
            return Nothing< type >(); \
        } \
    }

#if defined(__clang__)
    X(long)
#endif
    X(uint64_t)
    X(FLAC__MetadataType)

#undef X

}


typedef std::tuple<Local<Object>, Local<Object>> FlacEnumDefineReturnType;

#define flacEnum_defineValue(enumObject, reverseEnumObject, name, value) \
Nan::DefineOwnProperty(\
    enumObject, \
    Nan::New(name).ToLocalChecked(), \
    Nan::New<Number>(value), \
    (PropertyAttribute) (PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete) \
); \
Nan::DefineOwnProperty(\
    reverseEnumObject, \
    Nan::To<String>(Nan::New<Number>(value)).ToLocalChecked(), \
    Nan::New(name).ToLocalChecked(), \
    (PropertyAttribute) (PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete) \
);

#define flacEnum_declareInObject(obj, name, tuple) {\
auto impl = tuple; \
Nan::Set(obj, Nan::New(#name).ToLocalChecked(), std::get<0>(impl)); \
Nan::Set(obj, Nan::New(#name "String").ToLocalChecked(), std::get<1>(impl)); \
}

static inline bool throwIfNotConstructorCall(Nan::NAN_METHOD_ARGS_TYPE info) {
    if(info.IsConstructCall()) return false;
    Nan::ThrowError("Illegal invocation: use new to create a new object");
    return true;
}

#endif
