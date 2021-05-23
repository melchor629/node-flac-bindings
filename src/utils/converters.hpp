#pragma once

#include <functional>
#include <napi.h>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

namespace flac_bindings {

  using namespace std::string_literals;

  template<typename T, typename std::enable_if_t<std::is_signed<T>::value, unsigned> = 0>
  static inline std::optional<T> maybeNumberFromJs(const Napi::Value& value) {
    if (value.IsNumber()) {
      return (T) value.As<Napi::Number>().Int64Value();
    } else if (value.IsBigInt()) {
      auto bigNum = value.As<Napi::BigInt>();
      bool lossless = false;
      int64_t num = bigNum.Int64Value(&lossless);
      if (!lossless) {
        throw Napi::TypeError::New(
          bigNum.Env(),
          "Number "s + bigNum.ToString().Utf8Value()
            + " is too big for be converted into native number"s);
      }

      return (T) num;
    }

    return std::nullopt;
  }

  template<typename T, typename std::enable_if_t<std::is_unsigned<T>::value, unsigned> = 0>
  static inline std::optional<T> maybeNumberFromJs(const Napi::Value& value) {
    if (value.IsNumber()) {
      int64_t intValue = value.As<Napi::Number>().Int64Value();
      if (intValue < 0) {
        throw Napi::RangeError::New(
          value.Env(),
          "Number "s + std::to_string(intValue) + " is negative - expected unsigned value");
      }

      return (T) intValue;
    } else if (value.IsBigInt()) {
      auto bigNum = value.As<Napi::BigInt>();
      bool lossless = false;
      uint64_t num = bigNum.Uint64Value(&lossless);
      if (!lossless) {
        throw Napi::TypeError::New(
          bigNum.Env(),
          "Number "s + bigNum.ToString().Utf8Value()
            + " is too big for be converted into native number"s);
      }

      return (T) num;
    }

    return std::nullopt;
  }

  template<typename T, typename std::enable_if_t<std::is_enum<T>::value, unsigned> = 0>
  static inline std::optional<T> maybeNumberFromJs(const Napi::Value& value) {
    auto maybeNumber = maybeNumberFromJs<std::underlying_type_t<T>>(value);
    if (maybeNumber == std::nullopt) {
      return std::nullopt;
    }

    return (T) maybeNumber.value();
  }

  template<typename T>
  static inline T numberFromJs(const Napi::Value& value) {
    auto maybeNumber = maybeNumberFromJs<T>(value);
    if (maybeNumber == std::nullopt) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be number or bigint"s);
    }

    return maybeNumber.value();
  }

  constexpr uint64_t numberHighLimit = 9007199254740992;
  constexpr int64_t numberLowLimit = -9007199254740992;

  template<typename T, typename std::enable_if_t<std::is_unsigned<T>::value, unsigned> = 0>
  static inline Napi::Value numberToJs(const Napi::Env& env, T number, bool forceBigInt = false) {
    if (!forceBigInt && number <= numberHighLimit) {
      return Napi::Number::New(env, number);
    }

    return Napi::BigInt::New(env, (uint64_t) number);
  }

  template<typename T, typename std::enable_if_t<std::is_signed<T>::value, unsigned> = 0>
  static inline Napi::Value numberToJs(const Napi::Env& env, T number, bool forceBigInt = false) {
    if (!forceBigInt && numberLowLimit <= number && number <= int64_t(numberHighLimit)) {
      return Napi::Number::New(env, number);
    }

    return Napi::BigInt::New(env, (int64_t) number);
  }

  template<typename T, typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
  static inline Napi::Value
    numberToJs(const Napi::Env& env, T enumValue, bool forceBigInt = false) {
    return numberToJs<std::underlying_type_t<T>>(env, enumValue, forceBigInt);
  }

  template<typename T, typename std::enable_if_t<std::is_integral<T>::value, bool> = 0>
  static inline T booleanFromJs(const Napi::Value& value) {
    return (T) value.ToBoolean().Value();
  }

  template<typename T>
  static inline std::optional<T> maybeBooleanFromJs(const Napi::Value& value) {
    if (value.IsNull() || value.IsUndefined()) {
      return std::nullopt;
    }

    return booleanFromJs<T>(value);
  }

  template<typename T, typename std::enable_if_t<std::is_integral<T>::value, bool> = 0>
  static inline Napi::Value booleanToJs(const Napi::Env& env, T boolean) {
    return Napi::Boolean::New(env, boolean);
  }

  static inline std::optional<std::string> maybeStringFromJs(const Napi::Value& value) {
    if (!value.IsString()) {
      return std::nullopt;
    }

    return value.As<Napi::String>();
  }

  static inline std::string stringFromJs(const Napi::Value& value) {
    auto maybeString = maybeStringFromJs(value);
    if (maybeString == std::nullopt) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be string"s);
    }

    return maybeString.value();
  }

  template<typename Type>
  static inline std::vector<Type> arrayFromJs(
    const Napi::Value& value,
    const std::function<Type(const Napi::Value&)>& converter) {
    if (!value.IsArray()) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be Array");
    }

    std::vector<Type> values;
    auto array = value.As<Napi::Array>();
    for (uint32_t i = 0; i < array.Length(); i++) {
      values.emplace_back(converter(array[i]));
    }

    return values;
  }

  template<typename Type>
  static inline Napi::Array arrayToJs(const Napi::Env& env, const std::vector<Type>& list) {
    auto array = Napi::Array::New(env, list.size());
    for (size_t i = 0; i < list.size(); i += 1) {
      array[i] = list[i];
    }
    return array;
  }

  template<typename Type, size_t Size>
  static inline Napi::Array arrayToJs(const Napi::Env& env, Type (&arr)[Size]) {
    auto array = Napi::Array::New(env, Size);
    for (size_t i = 0; i < Size; i += 1) {
      array[i] = arr[i];
    }
    return array;
  }

  static inline bool maybeFunctionIntoRef(Napi::FunctionReference& ref, const Napi::Value& value) {
    if (value.IsFunction()) {
      if (!ref.IsEmpty()) {
        ref.Unref();
      }
      ref = Persistent(value.As<Napi::Function>());
      return true;
    }

    return false;
  }

  static inline void functionIntoRef(Napi::FunctionReference& ref, const Napi::Value& value) {
    if (!maybeFunctionIntoRef(ref, value)) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be function");
    }
  }

}
