#pragma once

#include <napi.h>
#include "../flac_addon.hpp"

namespace flac_bindings {

    static inline Napi::Object objectFreeze(Napi::Object& obj) {
        auto Object = obj.Env().Global().Get("Object").As<Napi::Object>();
        auto freeze = Object.Get("freeze").As<Napi::Function>();
        return freeze.Call({obj}).As<Napi::Object>();
        return obj;
    }

    static inline Napi::Function getHelper(Napi::Env env, const char* name) {
        using namespace std::literals;
        Napi::EscapableHandleScope scope(env);
        auto& module = env.GetInstanceData<FlacAddon>()->module;
        Napi::Value maybeHelper = module.Get("_helpers").As<Napi::Object>().Get(name);
        if(!maybeHelper.IsFunction()) {
            throw Napi::TypeError::New(env, "Helper "s + name + " is not defined"s);
        }

        return scope.Escape(maybeHelper).As<Napi::Function>();
    }

}
