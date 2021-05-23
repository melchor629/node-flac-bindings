#pragma once

#include "js_utils.hpp"
#include <napi.h>
#include <tuple>

namespace flac_bindings {
  namespace c_enum {
    typedef std::tuple<Napi::Object, Napi::Object> DefineReturnType;

    template<typename T>
    static inline void defineValue(
      Napi::Object& enumObject,
      Napi::Object& reverseEnumObject,
      const char* name,
      T value) {
      enumObject.DefineProperty(Napi::PropertyDescriptor::Value(
        name,
        numberToJs(enumObject.Env(), value),
        napi_property_attributes::napi_enumerable));
      reverseEnumObject.DefineProperty(Napi::PropertyDescriptor::Value(
        numberToJs(reverseEnumObject.Env(), value).ToString(),
        Napi::String::New(reverseEnumObject.Env(), name),
        napi_property_attributes::napi_enumerable));
    }

    static inline void declareInObject(
      Napi::Object& obj,
      const char* name,
      std::function<DefineReturnType(const Napi::Env&)> func) {
      auto tuple = func(obj.Env());
      obj.DefineProperties(
        {Napi::PropertyDescriptor::Value(
           name,
           objectFreeze(std::get<0>(tuple)),
           napi_property_attributes::napi_enumerable),
         Napi::PropertyDescriptor::Value(
           name + "String"s,
           objectFreeze(std::get<1>(tuple)),
           napi_property_attributes::napi_enumerable)});
    }
  }
}