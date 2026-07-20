#pragma once

#include "../utils/async.hpp"
#include "../utils/converters.hpp"
#include <optional>

namespace flac_bindings {

  using namespace Napi;

  template<typename T>
  class NativeAsyncIterator: public ObjectWrap<NativeAsyncIterator<T>> {
  public:
    typedef std::optional<T> IterationReturnValue;
    typedef AsyncBackgroundTask<IterationReturnValue> AsyncTask;
    typedef std::function<IterationReturnValue(typename AsyncTask::ExecutionProgress&, uint64_t)>
      IteratorFunction;
    typedef std::function<Napi::Value(const Napi::Env&, T)> ToJsFunction;

    static Napi::Value newIterator(
      const Napi::Env& env,
      const char* name,
      const IteratorFunction& impl,
      const ToJsFunction& map) {
      EscapableHandleScope scope(env);

      Function constructor = NativeAsyncIterator::DefineClass(
        env,
        "NativeAsyncIterator",
        {
          NativeAsyncIterator::InstanceMethod("next", &NativeAsyncIterator::next),
        });

      auto obj = constructor.New({
        String::New(env, name),
        External<IteratorFunction>::New(env, new IteratorFunction(impl)),
        External<ToJsFunction>::New(env, new ToJsFunction(map)),
      });
      return scope.Escape(obj);
    }

    NativeAsyncIterator(const CallbackInfo& info): ObjectWrap<NativeAsyncIterator<T>>(info) {
      name = info[0].As<String>().Utf8Value();
      impl = info[1].As<External<IteratorFunction>>().Data();
      map = info[2].As<External<ToJsFunction>>().Data();
    }

    virtual ~NativeAsyncIterator() {
      delete impl;
      delete map;
    }

    Napi::Value next(const CallbackInfo& info) {
      auto worker = new AsyncBackgroundTask<IterationReturnValue>(
        info.Env(),
        [this](auto c) { c.resolve((*impl)(c, pos)); },
        nullptr,
        name.c_str(),
        [this](auto env, auto value) {
          auto returnObject = Object::New(env);

          if (value.has_value()) {
            returnObject["done"] = booleanToJs(env, false);
            returnObject["value"] = (*map)(env, value.value());
            pos += 1;
          } else {
            returnObject["done"] = booleanToJs(env, true);
          }

          return returnObject;
        });

      worker->Queue();
      return worker->getPromise();
    }

  private:
    std::string name;
    IteratorFunction* impl;
    ToJsFunction* map;
    uint64_t pos = 0;
  };

}
