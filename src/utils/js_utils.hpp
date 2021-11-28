#pragma once

#include "../flac_addon.hpp"
#include <napi.h>

namespace flac_bindings {

  static inline Napi::Object objectFreeze(Napi::Object& obj) {
    obj.Freeze();
    return obj;
  }

}
