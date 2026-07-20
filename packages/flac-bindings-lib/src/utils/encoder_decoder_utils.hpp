#pragma once

#include "converters.hpp"
#include <functional>

namespace flac_bindings {

  template<typename ReturnValueType>
  static auto generateParseNumberResult(ReturnValueType& returnValue, const char* op) {
    return [&returnValue, op](const Napi::Value& value) {
      auto maybeReturnValue = maybeNumberFromJs<ReturnValueType>(value);
      if (!maybeReturnValue.has_value()) {
        throw Napi::TypeError::New(value.Env(), op + ": Expected number or bigint as return type"s);
      }

      returnValue = maybeReturnValue.value();
    };
  }

  static inline auto generateParseBooleanResult(int& returnValue, const char* op) {
    return [&returnValue, op](const Napi::Value& value) {
      auto maybeReturnValue = maybeBooleanFromJs<int>(value);
      if (!maybeReturnValue.has_value()) {
        throw Napi::TypeError::New(value.Env(), op + ": Expected boolean as return type"s);
      }

      returnValue = maybeReturnValue.value();
    };
  }

  template<typename ReturnValueType, typename ResultType = uint64_t>
  static auto generateParseObjectResult(
    ReturnValueType& returnValue,
    const char* op,
    const char* attributeName,
    ResultType& valueToSet) {
    return [&returnValue, op, attributeName, &valueToSet](const Napi::Value& value) {
      if (!value.IsObject()) {
        throw Napi::TypeError::New(value.Env(), op + ": Expected object as return value"s);
      }

      auto obj = value.As<Object>();
      auto maybeOpValue = maybeNumberFromJs<ResultType>(obj[attributeName]);
      auto maybeReturnValue = maybeNumberFromJs<ReturnValueType>(obj["returnValue"]);
      if (!maybeOpValue.has_value()) {
        throw Napi::TypeError::New(
          value.Env(),
          op + ": Expected number or bigint for "s + attributeName);
      }
      if (!maybeReturnValue.has_value()) {
        throw Napi::TypeError::New(value.Env(), op + ": Expected number or bigint as return type"s);
      }

      valueToSet = maybeOpValue.value();
      returnValue = maybeReturnValue.value();
    };
  }

}
