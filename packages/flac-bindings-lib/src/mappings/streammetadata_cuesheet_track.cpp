#include "../flac_addon.hpp"
#include "mappings.hpp"
#include "native_iterator.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  template<>
  Value Mapping<FLAC__StreamMetadata_CueSheet_Track>::toJs(
    const Env&,
    FLAC__StreamMetadata_CueSheet_Track*,
    bool);

  Function CueSheetTrack::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    napi_property_attributes attributes = napi_property_attributes::napi_enumerable;
    Function constructor = DefineClass(
      env,
      "CueSheetTrack",
      {
        InstanceAccessor(
          "offset",
          &CueSheetTrack::getOffset,
          &CueSheetTrack::setOffset,
          attributes),
        InstanceAccessor(
          "number",
          &CueSheetTrack::getNumber,
          &CueSheetTrack::setNumber,
          attributes),
        InstanceAccessor("isrc", &CueSheetTrack::getIsrc, &CueSheetTrack::setIsrc, attributes),
        InstanceAccessor("type", &CueSheetTrack::getType, &CueSheetTrack::setType, attributes),
        InstanceAccessor(
          "preEmphasis",
          &CueSheetTrack::getPreEmphasis,
          &CueSheetTrack::setPreEmphasis,
          attributes),
        InstanceAccessor("count", &CueSheetTrack::getCount, nullptr, attributes),
        InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &CueSheetTrack::iterator),
        InstanceMethod("clone", &CueSheetTrack::clone),
      });

    addon.cueSheetTrackConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  CueSheetTrack::CueSheetTrack(const CallbackInfo& info):
      ObjectWrap<CueSheetTrack>(info), Mapping<FLAC__StreamMetadata_CueSheet_Track>(info) {
    if (data == nullptr) {
      data = FLAC__metadata_object_cuesheet_track_new();
      shouldBeDeleted = true;
    }
  }

  CueSheetTrack::~CueSheetTrack() {
    if (shouldBeDeleted) {
      FLAC__metadata_object_cuesheet_track_delete(data);
    }
  }

  Napi::Value CueSheetTrack::getOffset(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->offset);
  }

  void CueSheetTrack::setOffset(const CallbackInfo&, const Napi::Value& value) {
    auto offset = numberFromJs<uint64_t>(value);
    data->offset = offset;
  }

  Napi::Value CueSheetTrack::getNumber(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->number);
  }

  void CueSheetTrack::setNumber(const CallbackInfo&, const Napi::Value& value) {
    auto number = numberFromJs<FLAC__byte>(value);
    data->number = number;
  }

  Napi::Value CueSheetTrack::getIsrc(const CallbackInfo& info) {
    return String::New(info.Env(), data->isrc);
  }

  void CueSheetTrack::setIsrc(const CallbackInfo& info, const Napi::Value& value) {
    auto string = stringFromJs(value);
    if (string.length() != 12) {
      throw RangeError::New(info.Env(), "Expected string to be 12 bytes length");
    }

    strcpy(data->isrc, string.c_str());
  }

  Napi::Value CueSheetTrack::getType(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->type);
  }

  void CueSheetTrack::setType(const CallbackInfo&, const Napi::Value& value) {
    auto type = numberFromJs<uint8_t>(value);
    data->type = type & 0x1;
  }

  Napi::Value CueSheetTrack::getPreEmphasis(const CallbackInfo& info) {
    return booleanToJs(info.Env(), data->pre_emphasis);
  }

  void CueSheetTrack::setPreEmphasis(const CallbackInfo&, const Napi::Value& value) {
    auto preEmphasis = booleanFromJs<FLAC__bool>(value);
    data->pre_emphasis = preEmphasis;
  }

  Napi::Value CueSheetTrack::getCount(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->num_indices);
  }

  Napi::Value CueSheetTrack::iterator(const CallbackInfo& info) {
    return NativeIterator::newIterator(
      info.Env(),
      [this](auto env, auto pos) -> NativeIterator::IterationReturnValue {
        EscapableHandleScope scope(env);

        if (pos >= data->num_indices) {
          return {};
        } else {
          auto object = CueSheetIndex::toJs(env, data->indices + pos);
          return {scope.Escape(object)};
        }
      });
  }

  Napi::Value CueSheetTrack::clone(const CallbackInfo& info) {
    auto newTrack = FLAC__metadata_object_cuesheet_track_clone(data);
    return CueSheetTrack::toJs(info.Env(), newTrack, true);
  }

  template<>
  Mapping<FLAC__StreamMetadata_CueSheet_Track>&
    Mapping<FLAC__StreamMetadata_CueSheet_Track>::fromJs(const Value& value) {
    if (!value.IsObject()) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be object"s);
    }

    auto object = value.As<Object>();
    auto addon = value.Env().GetInstanceData<FlacAddon>();
    if (!object.InstanceOf(addon->cueSheetTrackConstructor.Value())) {
      throw Napi::TypeError::New(value.Env(), "Object is not an instance of CueSheetTrack");
    }

    return *CueSheetTrack::Unwrap(value.As<Object>());
  }

  template<>
  Value Mapping<FLAC__StreamMetadata_CueSheet_Track>::toJs(
    const Env& env,
    FLAC__StreamMetadata_CueSheet_Track* track,
    bool deleteHint) {
    EscapableHandleScope scope(env);
    auto addon = Env(env).GetInstanceData<FlacAddon>();
    Function constructor = addon->cueSheetTrackConstructor.Value();
    auto object = constructor.New({pointer::wrap(env, track), booleanToJs(env, deleteHint)});
    return scope.Escape(object);
  }

}
