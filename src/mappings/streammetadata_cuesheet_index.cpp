#include "../flac_addon.hpp"
#include "mappings.hpp"

namespace flac_bindings {

  using namespace Napi;

  Function CueSheetIndex::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    Function constructor = DefineClass(
      env,
      "CueSheetIndex",
      {
        InstanceAccessor(
          "offset",
          &CueSheetIndex::getOffset,
          &CueSheetIndex::setOffset,
          napi_property_attributes::napi_enumerable),
        InstanceAccessor(
          "number",
          &CueSheetIndex::getNumber,
          &CueSheetIndex::setNumber,
          napi_property_attributes::napi_enumerable),
      });

    addon.cueSheetIndexConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  CueSheetIndex::CueSheetIndex(const CallbackInfo& info):
      ObjectWrap<CueSheetIndex>(info), Mapping<FLAC__StreamMetadata_CueSheet_Index>(info) {
    if (data == nullptr) {
      data = new FLAC__StreamMetadata_CueSheet_Index;
      shouldBeDeleted = true;
      memset(data, 0, sizeof(*data));
    }

    if (info.Length() > 0 && !info[0].IsExternal()) {
      data->offset = numberFromJs<uint64_t>(info[0]);
      if (info.Length() > 1) {
        data->number = numberFromJs<uint8_t>(info[1]);
      }
    }
  }

  CueSheetIndex::~CueSheetIndex() {
    if (shouldBeDeleted) {
      delete data;
    }
  }

  Napi::Value CueSheetIndex::getOffset(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->offset);
  }

  void CueSheetIndex::setOffset(const CallbackInfo&, const Napi::Value& value) {
    auto offset = numberFromJs<uint64_t>(value);
    data->offset = offset;
  }

  Napi::Value CueSheetIndex::getNumber(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->number);
  }

  void CueSheetIndex::setNumber(const CallbackInfo&, const Napi::Value& value) {
    auto number = numberFromJs<uint8_t>(value);
    data->number = number;
  }

  template<>
  Mapping<FLAC__StreamMetadata_CueSheet_Index>&
    Mapping<FLAC__StreamMetadata_CueSheet_Index>::fromJs(const Value& value) {
    if (!value.IsObject()) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be object"s);
    }

    auto object = value.As<Object>();
    auto addon = value.Env().GetInstanceData<FlacAddon>();
    if (!object.InstanceOf(addon->cueSheetIndexConstructor.Value())) {
      throw Napi::TypeError::New(value.Env(), "Object is not an instance of CueSheetIndex");
    }

    return *CueSheetIndex::Unwrap(value.As<Object>());
  }

  template<>
  Value Mapping<FLAC__StreamMetadata_CueSheet_Index>::toJs(
    const Env& env,
    FLAC__StreamMetadata_CueSheet_Index* point,
    bool deleteHint) {
    EscapableHandleScope scope(env);
    auto addon = Env(env).GetInstanceData<FlacAddon>();
    Function constructor = addon->cueSheetIndexConstructor.Value();
    auto object = constructor.New({pointer::wrap(env, point), booleanToJs(env, deleteHint)});
    return scope.Escape(object);
  }

}
