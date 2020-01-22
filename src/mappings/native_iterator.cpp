#include "native_iterator.hpp"
#include "native_async_iterator.hpp"
#include "../flac/format.h"

namespace flac_bindings {

    using namespace Napi;

    FunctionReference NativeIterator::constructor;
    void NativeIterator::init(const Napi::Env& env) {
        Function constructor = DefineClass(env, "NativeIterator", {
            InstanceMethod("next", &NativeIterator::next),
        });

        NativeIterator::constructor = Persistent(constructor);
        NativeIterator::constructor.SuppressDestruct();
    }

    Napi::Value NativeIterator::newIterator(const Napi::Env& env, const IteratorFunction& impl) {
        EscapableHandleScope scope(env);
        auto obj = constructor.New({External<IteratorFunction>::New(env, new IteratorFunction(impl))});
        return scope.Escape(obj);
    }

    NativeIterator::NativeIterator(const CallbackInfo& info): ObjectWrap<NativeIterator>(info) {
        impl = info[0].As<External<IteratorFunction>>().Data();
    }

    NativeIterator::~NativeIterator() {
        delete impl;
    }

    Napi::Value NativeIterator::next(const CallbackInfo& info) {
        auto res = (*impl)(info.Env(), pos);
        auto returnObject = Object::New(info.Env());

        if(res.has_value()) {
            returnObject["done"] = booleanToJs(info.Env(), false);
            returnObject["value"] = res.value();
            pos += 1;
        } else {
            returnObject["done"] = booleanToJs(info.Env(), true);
        }

        return returnObject;
    }

}