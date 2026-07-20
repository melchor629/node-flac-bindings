#pragma once

#include "../utils/converters.hpp"
#include <optional>

namespace flac_bindings {

  using namespace Napi;

  class NativeIterator: public ObjectWrap<NativeIterator> {
  public:
    typedef std::optional<Napi::Value> IterationReturnValue;
    typedef std::function<IterationReturnValue(const Napi::Env&, uint64_t)> IteratorFunction;

    static void init(const Napi::Env& env, class FlacAddon&);
    static Napi::Value newIterator(Napi::Env, const IteratorFunction&);

    NativeIterator(const CallbackInfo& info);
    virtual ~NativeIterator();

    Napi::Value next(const CallbackInfo&);

  private:
    IteratorFunction* impl;
    uint64_t pos = 0;
  };

}
