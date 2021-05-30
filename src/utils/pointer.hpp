#pragma once

#include <cassert>
#include <napi.h>

namespace flac_bindings {

  namespace pointer {
    template<typename Type = void>
    inline static Napi::External<Type> wrap(Napi::Env env, Type* ptr) {
      assert(ptr != nullptr);
      return Napi::External<Type>::New(env, ptr);
    }

    template<typename Type>
    inline static Napi::Buffer<Type> wrap(Napi::Env env, Type* ptr, size_t size) {
      assert(size > 0);
      assert(ptr != nullptr);
      return Napi::Buffer<Type>::New(env, ptr, size);
    }

    template<typename Type>
    inline static void detach(Napi::Buffer<Type> buffer) {
#if NAPI_VERSION >= 7
      auto arrayBuffer = buffer.ArrayBuffer();
      if (!arrayBuffer.IsDetached()) {
        arrayBuffer.Detach();
      }
#else
      (void) buffer;
#endif
    }

    template<typename Type = void>
    inline static Type* unwrap(Napi::Env env, Napi::Value maybeExternal) {
      if (!maybeExternal.IsExternal()) {
        throw Napi::TypeError::New(env, "Expected type to be External object");
      }

      auto external = maybeExternal.As<Napi::External<Type>>();
      return external.Data();
    }

    template<typename T>
    static inline std::tuple<T*, size_t> fromBuffer(const Napi::Value& value) {
      using namespace std::literals;
      if (!value.IsBuffer()) {
        throw Napi::TypeError::New(
          value.Env(),
          "Expected "s + value.ToString().Utf8Value() + " to be Buffer"s);
      }

      Napi::Buffer<T> buffer = value.As<Napi::Buffer<T>>();
      return std::make_tuple(buffer.Data(), buffer.Length());
    }

    template<typename T>
    struct BufferReference {
      Napi::Reference<Napi::Buffer<T>> ref;

      bool isEmpty() const {
        return ref.IsEmpty();
      }

      void clear() {
        sharedClear();
        ref.Reset();
      }

      void setFromWrap(Napi::Env env, T* ptr, size_t size) {
        sharedClear();
        ref.Reset(wrap(env, ptr, size), 1);
      }

      Napi::Buffer<T> value() {
        return ref.Value();
      }

      ~BufferReference() {
        clear();
      }

    private:
      void sharedClear() {
        if (!ref.IsEmpty()) {
          ref.Unref();
          detach(ref.Value());
        }
      }
    };
  }

}
