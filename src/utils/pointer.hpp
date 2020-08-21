#pragma once

#include <napi.h>
#include <cassert>

namespace flac_bindings {

    namespace pointer {
        template<typename Type = void>
        inline static Napi::External<Type> wrap(Napi::Env env, Type* ptr) {
            assert(ptr != nullptr);
            return Napi::External<Type>::New(env, ptr);
        }

        template<typename Type>
        inline static Napi::Buffer<Type> wrap(const Napi::Env& env, Type* ptr, size_t size) {
            assert(size > 0);
            assert(ptr != nullptr);
            return Napi::Buffer<Type>::New(env, ptr, size);
        }

        template<typename Type = void>
        inline static Type* unwrap(Napi::Env env, Napi::Value maybeExternal) {
            if(!maybeExternal.IsExternal()) {
                throw Napi::TypeError::New(env, "Expected type to be External object");
            }

            auto external = maybeExternal.As<Napi::External<Type>>();
            return external.Data();
        }

        template<typename T>
        static inline std::tuple<T*, size_t> fromBuffer(const Napi::Value& value) {
            using namespace std::literals;
            if(!value.IsBuffer()) {
                throw Napi::TypeError::New(value.Env(), "Expected "s + value.ToString().Utf8Value() + " to be Buffer"s);
            }

            Napi::Buffer<T> buffer = value.As<Napi::Buffer<T>>();
            return std::make_tuple(buffer.Data(), buffer.Length());
        }
    }

}