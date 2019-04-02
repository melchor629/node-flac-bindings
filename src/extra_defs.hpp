#ifndef EXTRA_DEFS_H
#define EXTRA_DEFS_H

#include <tuple>

namespace Nan {

#define X(type) \
    template<> \
    struct ToFactory< type > : ValueFactoryBase< type > { \
        static inline return_t convert(v8::Local<v8::Value> val); \
    }; \

    namespace imp {

        X(long)
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

    X(long)
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

#endif //EXTRA_DEFS_H