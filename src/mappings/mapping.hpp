#pragma once

#include "../utils/converters.hpp"
#include "../utils/pointer.hpp"

namespace flac_bindings {

  using namespace Napi;

  template<typename StorageType>
  class Mapping {
  public:
    Mapping(const CallbackInfo& info) {
      if (info.Length() > 0 && info[0].IsExternal()) {
        data = pointer::unwrap<StorageType>(info.Env(), info[0]);
        if (info.Length() > 1 && info[1].IsBoolean()) {
          shouldBeDeleted = booleanFromJs<bool>(info[1]);
        }
      }
    }

    virtual ~Mapping() {}

    StorageType* get() {
      return data;
    }

    inline operator StorageType*() {
      return data;
    }

    inline bool willBeDeleted() const {
      return shouldBeDeleted;
    }

    inline void setDeletion(bool should) {
      shouldBeDeleted = should;
    }

    // Static methods to implement by each subclass
    static Mapping<StorageType>& fromJs(const Value&);
    static Value toJs(const Env&, StorageType*, bool deleteHint = false);

  protected:
    bool shouldBeDeleted = true;
    StorageType* data = nullptr;
  };

}