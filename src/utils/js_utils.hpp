#pragma once

#include <napi.h>

namespace flac_bindings {

    static inline Napi::Object objectFreeze(Napi::Object& obj) {
        auto Object = obj.Env().Global().Get("Object").As<Napi::Object>();
        auto freeze = Object.Get("freeze").As<Napi::Function>();
        return freeze.Call({obj}).As<Napi::Object>();
        return obj;
    }

    static inline Napi::Function getHelper(const Napi::Env& env, const char* name) {
        using namespace std::literals;
        extern Napi::ObjectReference module;
        Napi::EscapableHandleScope scope(env);
        Napi::Value maybeHelper = module.Get("_helpers").As<Napi::Object>().Get(name);
        if(!maybeHelper.IsFunction()) {
            throw Napi::TypeError::New(env, "Helper "s + name + " is not defined"s);
        }

        return scope.Escape(maybeHelper).As<Napi::Function>();
    }

    static inline void handleAsync(Napi::Promise& promise, Napi::Function& resolve, Napi::Function& reject) {
        getHelper(promise.Env(), "handleAsync")({
            promise,
            resolve,
            reject,
        });
    }

}
