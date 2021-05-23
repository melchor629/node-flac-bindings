#pragma once

#include <napi.h>
#include "../flac_addon.hpp"

namespace flac_bindings {

    static inline Napi::Object objectFreeze(Napi::Object& obj) {
#if NAPI_VERSION < 8
        auto Object = obj.Env().Global().Get("Object").As<Napi::Object>();
        auto freeze = Object.Get("freeze").As<Napi::Function>();
        return freeze.Call({obj}).As<Napi::Object>();
        return obj;
#else
        obj.Freeze();
        return obj;
#endif
    }

}
