#include "../flac_addon.hpp"
#include "../utils/pointer.hpp"
#include "mappings.hpp"

namespace flac_bindings {

  using namespace Napi;

  Function SeekPoint::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto attributes = napi_property_attributes::napi_enumerable;
    Function constructor = DefineClass(
      env,
      "SeekPoint",
      {
        InstanceAccessor(
          "sampleNumber",
          &SeekPoint::getSampleNumber,
          &SeekPoint::setSampleNumber,
          attributes),
        InstanceAccessor(
          "streamOffset",
          &SeekPoint::getStreamOffset,
          &SeekPoint::setStreamOffset,
          attributes),
        InstanceAccessor(
          "frameSamples",
          &SeekPoint::getFrameSamples,
          &SeekPoint::setFrameSamples,
          attributes),
      });

    addon.seekPointConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  SeekPoint::SeekPoint(const CallbackInfo& info):
      ObjectWrap<SeekPoint>(info), Mapping<FLAC__StreamMetadata_SeekPoint>(info) {
    if (data == nullptr) {
      data = new FLAC__StreamMetadata_SeekPoint;
      shouldBeDeleted = true;
      memset(data, 0, sizeof(*data));
    }

    if (info.Length() > 0 && !info[0].IsExternal()) {
      data->sample_number = numberFromJs<uint64_t>(info[0]);
      if (info.Length() > 1) {
        data->stream_offset = numberFromJs<uint64_t>(info[1]);
      }
      if (info.Length() > 2) {
        data->frame_samples = numberFromJs<uint32_t>(info[2]);
      }
    }
  }

  SeekPoint::~SeekPoint() {
    if (shouldBeDeleted) {
      delete data;
    }
  }

  Napi::Value SeekPoint::getSampleNumber(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->sample_number);
  }

  void SeekPoint::setSampleNumber(const CallbackInfo&, const Napi::Value& value) {
    auto sampleNumber = numberFromJs<uint64_t>(value);
    data->sample_number = sampleNumber;
  }

  Napi::Value SeekPoint::getStreamOffset(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->stream_offset);
  }

  void SeekPoint::setStreamOffset(const CallbackInfo&, const Napi::Value& value) {
    auto sampleOffset = numberFromJs<uint64_t>(value);
    data->stream_offset = sampleOffset;
  }

  Napi::Value SeekPoint::getFrameSamples(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->frame_samples);
  }

  void SeekPoint::setFrameSamples(const CallbackInfo&, const Napi::Value& value) {
    auto frameSamples = numberFromJs<uint32_t>(value);
    data->frame_samples = frameSamples;
  }

  template<>
  Mapping<FLAC__StreamMetadata_SeekPoint>&
    Mapping<FLAC__StreamMetadata_SeekPoint>::fromJs(const Value& value) {
    if (!value.IsObject()) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be object"s);
    }

    auto object = value.As<Object>();
    auto addon = value.Env().GetInstanceData<FlacAddon>();
    if (!object.InstanceOf(addon->seekPointConstructor.Value())) {
      throw Napi::TypeError::New(value.Env(), "Object is not an instance of SeekPoint");
    }

    return *SeekPoint::Unwrap(value.As<Object>());
  }

  template<>
  Value Mapping<FLAC__StreamMetadata_SeekPoint>::toJs(
    const Env& env,
    FLAC__StreamMetadata_SeekPoint* point,
    bool deleteHint) {
    EscapableHandleScope scope(env);
    auto addon = Env(env).GetInstanceData<FlacAddon>();
    Function constructor = addon->seekPointConstructor.Value();
    auto object = constructor.New({pointer::wrap(env, point), booleanToJs(env, deleteHint)});
    return scope.Escape(object);
  }

}
