#ifndef POINTER
#define POINTER

#include <nan.h>

inline void nop(char*,void*) { ; }

template<typename Type = void>
inline static Nan::MaybeLocal<v8::Object> WrapPointer(Type* ptr, size_t length = 0) {
    return Nan::NewBuffer((char*) (void*) ptr, length, nop, nullptr);
}

template<typename Type = void>
inline static Type* UnwrapPointer(v8::Handle<v8::Value> buffer) {
    void* ptr = nullptr;
    if(node::Buffer::HasInstance(buffer)) {
        ptr = node::Buffer::Data(buffer.As<v8::Object>());
    }
    return reinterpret_cast<Type*>(ptr);
}

#endif
