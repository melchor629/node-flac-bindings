#ifndef POINTER
#define POINTER

#ifndef NAN_METHOD
#include <nan.h>

using namespace v8;
using namespace node;
#endif

inline void nop(char*,void*) { ; }

template<typename Type = void>
inline static Nan::MaybeLocal<Object> WrapPointer(Type* ptr, size_t length = 0) {
    return Nan::NewBuffer((char*) (void*) ptr, length, nop, nullptr);
}

template<typename Type = void>
inline static Type* UnwrapPointer(Handle<Value> buffer) {
    void* ptr = nullptr;
    if(Buffer::HasInstance(buffer)) {
        ptr = Buffer::Data(buffer.As<Object>());
    }
    return reinterpret_cast<Type*>(ptr);
}

#endif
