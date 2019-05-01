#ifndef _MAPPINGS_DEFS_
#define _MAPPINGS_DEFS_

#include <tuple>
#include <type_traits>
#include "nan.h"

#include "pointer.hpp"
#include "../format/format.h"

#define __stringify__(f) #f
#define _stringify(f) __stringify__(f)

#define checkValue(type) MaybeLocal<type> _newValue = Nan::To<type>(value); \
if(_newValue.IsEmpty() || !_newValue.ToLocalChecked()->Is##type ()) { \
    Nan::ThrowError(Nan::Error("Invalid value: Has to be a " #type)); \
} else

#define checkValueIsBuffer() \
    if(!node::Buffer::HasInstance(value)) { \
        Nan::ThrowTypeError("Expected argument to be Buffer"); \
    } else

#define checkValueIsNumber(type) \
    auto _newValue = numberFromJs<type>(value); \
    auto newValue = _newValue.FromMaybe(0); \
    if(_newValue.IsNothing()) { \
        Nan::ThrowTypeError("Expected argument to be number or bigint"); \
    } else

#define getValue(type) Nan::To<type>(_newValue.ToLocalChecked()).FromJust()

#define assertThrowing(cond, msg) \
    if(!(cond)) { \
        Nan::ThrowError(msg " - the following condition was not meet: " #cond " at " __FILE__ ":" _stringify(__LINE__)); \
        return; \
    }

#define assertThrowing2(cond, msg, errorReturnValue) \
    if(!(cond)) { \
        Nan::ThrowError(msg " - the following condition was not meet: " #cond " at " __FILE__ ":" _stringify(__LINE__)); \
        return errorReturnValue; \
    }

#define SetGetter(name) Nan::SetAccessor(obj, Nan::New( #name ).ToLocalChecked(), name)
#define SetGetterSetter(name) Nan::SetAccessor(obj, Nan::New( #name ).ToLocalChecked(), name, name)

#define V8_GETTER(methodName) void methodName(v8::Local<Name> property, const PropertyCallbackInfo<Value> &info)
#define V8_SETTER(methodName) void methodName(v8::Local<Name> property, v8::Local<Value> value, const v8::PropertyCallbackInfo<void>& info)

#define unwrap(type) if(info.This()->InternalFieldCount() == 0) { Nan::ThrowTypeError("Unknown this object type"); return; } \
type* self = Nan::ObjectWrap::Unwrap<type>(info.This());

#define nativeReadOnlyProperty(obj, name, fnName) \
(void) obj->SetNativeDataProperty(info.GetIsolate()->GetCurrentContext(), Nan::New(name).ToLocalChecked(), fnName, 0, v8::Local<Value>(), (v8::PropertyAttribute) (v8::PropertyAttribute::ReadOnly | v8::PropertyAttribute::DontDelete))

#define nativeProperty(obj, name, fnName) \
(void) obj->SetNativeDataProperty(info.GetIsolate()->GetCurrentContext(), Nan::New(name).ToLocalChecked(), fnName, fnName, v8::Local<Value>(), v8::PropertyAttribute::DontDelete)


#define WARN_SYNC_FUNCTION(name) \
    static bool __has_ben_called = false; \
    if(!__has_ben_called) { \
        __has_ben_called = true; \
        printf("(flac-bindings) Warning: The sync function %s will be replaced by the async version in the next major version\n", name); \
    }


#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
template<typename T,
         typename std::enable_if_t<std::is_signed<T>::value, int> = 0>
static inline Nan::Maybe<T> numberFromJs(v8::Local<v8::BigInt> bigNum) {
    bool lossless = false;
    int64_t num = bigNum->Int64Value(&lossless);
    if(!lossless) return Nan::Nothing<T>();
    return Nan::Just((T) num);
}

template<typename T,
         typename std::enable_if_t<std::is_unsigned<T>::value, unsigned> = 0>
static inline Nan::Maybe<T> numberFromJs(v8::Local<v8::BigInt> bigNum) {
    bool lossless = false;
    uint64_t num = bigNum->Uint64Value(&lossless);
    if(!lossless) return Nan::Nothing<T>();
    return Nan::Just((T) num);
}
#endif

template<typename T,
         typename std::enable_if_t<std::is_unsigned<T>::value || std::is_signed<T>::value, unsigned> = 0>
static inline Nan::Maybe<T> numberFromJs(v8::Local<v8::Value> num) {
    if(num.IsEmpty()) return Nan::Nothing<T>();
    if(num->IsNumber()) {
        return Nan::To<T>(num);
    }
#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
    else if(num->IsBigInt()) {
        v8::Local<v8::BigInt> bigNum = num.As<v8::BigInt>();
        return numberFromJs<T>(bigNum);
    }
#endif
    return Nan::Nothing<T>();
}

template<typename T,
         typename std::enable_if_t<std::is_enum<T>::value, unsigned> = 0>
static inline Nan::Maybe<T> numberFromJs(v8::Local<v8::Value> num) {
    auto e = numberFromJs<int>(num);
    if(e.IsJust()) return Nan::Just<T>((T) e.FromJust());
    return Nan::Nothing<T>();
}

template<typename T>
static inline Nan::Maybe<T> numberFromJs(Nan::MaybeLocal<v8::Value> maybeNum) {
    if(maybeNum.IsEmpty()) return Nan::Nothing<T>();
    else return numberFromJs<T>(maybeNum.ToLocalChecked());
}

template<typename T,
         typename std::enable_if_t<std::is_unsigned<T>::value, unsigned> = 0>
static inline v8::Local<v8::Value> numberToJs(T number, bool forceBigInt = false) {
#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
    if(!forceBigInt && number <= 9007199254740992) {
        return Nan::New<v8::Number>((int64_t) number);
    }
    return v8::BigInt::NewFromUnsigned(v8::Isolate::GetCurrent(), (uint64_t) number);
#else
    return Nan::New<v8::Number>((int64_t) number);
#endif
}

template<typename T,
         typename std::enable_if_t<std::is_signed<T>::value, int> = 0>
static inline v8::Local<v8::Value> numberToJs(T number, bool forceBigInt = false) {
#if NODE_MODULE_VERSION >= NODE_10_0_MODULE_VERSION
    if(!forceBigInt && -9007199254740992 <= number && number <= 9007199254740992) {
        return Nan::New<v8::Number>((int64_t) number);
    }
    return v8::BigInt::New(v8::Isolate::GetCurrent(), (int64_t) number);
#else
    return Nan::New<Number>((int64_t) number);
#endif
}

template<typename T,
         typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
static inline v8::Local<v8::Value> numberToJs(T enumValue, bool forceBigInt = false) {
    return numberToJs<int>((int) enumValue, forceBigInt);
}


template<typename T,
         typename std::enable_if_t<std::is_integral<T>::value, bool> = 0>
static inline Nan::Maybe<T> booleanFromJs(v8::Local<v8::Value> boolean) {
    if(boolean.IsEmpty() || !boolean->IsBoolean()) {
        return Nan::Nothing<T>();
    } else {
        auto mayb = boolean->BooleanValue(v8::Isolate::GetCurrent()->GetCurrentContext());
        if(mayb.IsNothing()) return Nan::Nothing<T>();
        return Nan::Just<T>((T) mayb.FromJust());
    }
}

template<typename T,
         typename std::enable_if_t<std::is_integral<T>::value, bool> = 0>
static inline Nan::Maybe<T> booleanFromJs(Nan::MaybeLocal<v8::Value> boolean) {
    if(boolean.IsEmpty()) return Nan::Nothing<T>();
    return booleanFromJs<T>(boolean.ToLocalChecked());
}

template<typename T,
         typename std::enable_if_t<std::is_integral<T>::value, bool> = 0>
static inline v8::Local<v8::Boolean> booleanToJs(T boolean) {
    return Nan::New<v8::Boolean>((bool) boolean);
}



namespace Nan {

#define X(type) \
    template<> \
    struct ToFactory< type > : ValueFactoryBase< type > { \
        static inline return_t convert(v8::Local<v8::Value> val); \
    }; \

    namespace imp {

#if !defined(__GNUC__) || defined(__clang__)
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

#if !defined(__GNUC__) || defined(__clang__)
    X(long)
#endif
    X(uint64_t)
    X(FLAC__MetadataType)

#undef X

}


typedef std::tuple<v8::Local<v8::Object>, v8::Local<v8::Object>> FlacEnumDefineReturnType;

static inline void flacEnum_defineValue(v8::Local<v8::Object> &enumObject, v8::Local<v8::Object> &reverseEnumObject, const char* name, int value) {
    Nan::DefineOwnProperty(
        enumObject,
        Nan::New(name).ToLocalChecked(),
        Nan::New<v8::Number>(value),
        (v8::PropertyAttribute) (v8::PropertyAttribute::ReadOnly | v8::PropertyAttribute::DontDelete)
    );
    Nan::DefineOwnProperty(
        reverseEnumObject,
        Nan::To<v8::String>(Nan::New<v8::Number>(value)).ToLocalChecked(),
        Nan::New(name).ToLocalChecked(),
        (v8::PropertyAttribute) (v8::PropertyAttribute::ReadOnly | v8::PropertyAttribute::DontDelete)
    );
}

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
