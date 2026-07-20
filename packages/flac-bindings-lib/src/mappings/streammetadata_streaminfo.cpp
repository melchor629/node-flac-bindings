#include "../flac_addon.hpp"
#include "mappings.hpp"

namespace flac_bindings {

  using namespace Napi;

  Function StreamInfoMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto attributes = napi_property_attributes::napi_enumerable;
    Function constructor = DefineClass(
      env,
      "StreamInfoMetadata",
      {
        InstanceAccessor(
          "minBlocksize",
          &StreamInfoMetadata::getMinBlocksize,
          &StreamInfoMetadata::setMinBlocksize,
          attributes),
        InstanceAccessor(
          "maxBlocksize",
          &StreamInfoMetadata::getMaxBlocksize,
          &StreamInfoMetadata::setMaxBlocksize,
          attributes),
        InstanceAccessor(
          "minFramesize",
          &StreamInfoMetadata::getMinFramesize,
          &StreamInfoMetadata::setMinFramesize,
          attributes),
        InstanceAccessor(
          "maxFramesize",
          &StreamInfoMetadata::getMaxFramesize,
          &StreamInfoMetadata::setMaxFramesize,
          attributes),
        InstanceAccessor(
          "channels",
          &StreamInfoMetadata::getChannels,
          &StreamInfoMetadata::setChannels,
          attributes),
        InstanceAccessor(
          "bitsPerSample",
          &StreamInfoMetadata::getBitsPerSample,
          &StreamInfoMetadata::setBitsPerSample,
          attributes),
        InstanceAccessor(
          "sampleRate",
          &StreamInfoMetadata::getSampleRate,
          &StreamInfoMetadata::setSampleRate,
          attributes),
        InstanceAccessor(
          "totalSamples",
          &StreamInfoMetadata::getTotalSamples,
          &StreamInfoMetadata::setTotalSamples,
          attributes),
        InstanceAccessor(
          "md5sum",
          &StreamInfoMetadata::getMd5sum,
          &StreamInfoMetadata::setMd5sum,
          attributes),
      });

    addon.streamInfoMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  StreamInfoMetadata::StreamInfoMetadata(const CallbackInfo& info):
      ObjectWrap<StreamInfoMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_STREAMINFO) {}

  Napi::Value StreamInfoMetadata::getMinBlocksize(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.min_blocksize);
  }

  void StreamInfoMetadata::setMinBlocksize(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint32_t>(value);
    data->data.stream_info.min_blocksize = newValue;
  }

  Napi::Value StreamInfoMetadata::getMaxBlocksize(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.max_blocksize);
  }

  void StreamInfoMetadata::setMaxBlocksize(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint32_t>(value);
    data->data.stream_info.max_blocksize = newValue;
  }

  Napi::Value StreamInfoMetadata::getMinFramesize(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.min_framesize);
  }

  void StreamInfoMetadata::setMinFramesize(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint32_t>(value);
    data->data.stream_info.min_framesize = newValue;
  }

  Napi::Value StreamInfoMetadata::getMaxFramesize(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.max_framesize);
  }

  void StreamInfoMetadata::setMaxFramesize(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint32_t>(value);
    data->data.stream_info.max_framesize = newValue;
  }

  Napi::Value StreamInfoMetadata::getChannels(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.channels);
  }

  void StreamInfoMetadata::setChannels(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint32_t>(value);
    data->data.stream_info.channels = newValue;
  }

  Napi::Value StreamInfoMetadata::getBitsPerSample(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.bits_per_sample);
  }

  void StreamInfoMetadata::setBitsPerSample(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint32_t>(value);
    data->data.stream_info.bits_per_sample = newValue;
  }

  Napi::Value StreamInfoMetadata::getSampleRate(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.sample_rate);
  }

  void StreamInfoMetadata::setSampleRate(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint32_t>(value);
    data->data.stream_info.sample_rate = newValue;
  }

  Napi::Value StreamInfoMetadata::getTotalSamples(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.stream_info.total_samples);
  }

  void StreamInfoMetadata::setTotalSamples(const CallbackInfo&, const Napi::Value& value) {
    auto newValue = numberFromJs<uint64_t>(value);
    data->data.stream_info.total_samples = newValue;
  }

  Napi::Value StreamInfoMetadata::getMd5sum(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    if (md5SumBuffer.isEmpty()) {
      md5SumBuffer.setFromWrap(info.Env(), data->data.stream_info.md5sum, 16);
    }

    return scope.Escape(md5SumBuffer.value());
  }

  void StreamInfoMetadata::setMd5sum(const CallbackInfo& info, const Napi::Value& value) {
    FLAC__byte* ptr;
    size_t length;
    std::tie(ptr, length) = pointer::fromBuffer<FLAC__byte>(value);

    if (length < 16) {
      throw RangeError::New(info.Env(), "Data length is less than 16 bytes");
    }

    memcpy(data->data.stream_info.md5sum, ptr, 16 * sizeof(FLAC__byte));
  }

}
