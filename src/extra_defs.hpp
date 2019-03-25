#ifndef EXTRA_DEFS_H
#define EXTRA_DEFS_H

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

#endif //EXTRA_DEFS_H