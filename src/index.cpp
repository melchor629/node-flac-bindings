#include "decoder/decoder.hpp"
#include "encoder/encoder.hpp"
#include "flac_addon.hpp"
#include "mappings/mappings.hpp"
#include "mappings/native_iterator.hpp"
#include <napi.h>

namespace flac_bindings {

  using namespace Napi;

  extern Promise testAsync(const CallbackInfo& info);
  extern Object initFormat(const Env& env);
  extern Object initMetadata0(const Env& env);
  extern Function initMetadata1(Env env, FlacAddon&);
  extern Value initMetadata2Chain(Env env, FlacAddon&);
  extern Value initMetadata2Iterator(Env env, FlacAddon&);
  extern Object initFns(Env env);

  FlacAddon::FlacAddon(Env env, Object exports) {
    NativeIterator::init(env, *this);

    DefineAddon(
      exports,
      {
        InstanceValue("_testAsync", Function::New(env, testAsync, "_testAsync")),
        InstanceValue("napiVersion", Number::New(env, NAPI_VERSION), napi_enumerable),
        InstanceValue("EncoderBuilder", StreamEncoderBuilder::init(env, *this), napi_enumerable),
        InstanceValue("Encoder", StreamEncoder::init(env, *this), napi_enumerable),
        InstanceValue("DecoderBuilder", StreamDecoderBuilder::init(env, *this), napi_enumerable),
        InstanceValue("Decoder", StreamDecoder::init(env, *this), napi_enumerable),
        InstanceValue("format", initFormat(env), napi_enumerable),
        InstanceValue(
          "metadata",
          DefineProperties(
            Object::New(env),
            {
              InstanceValue(
                "StreamInfoMetadata",
                StreamInfoMetadata::init(env, *this),
                napi_enumerable),
              InstanceValue("PaddingMetadata", PaddingMetadata::init(env, *this), napi_enumerable),
              InstanceValue(
                "ApplicationMetadata",
                ApplicationMetadata::init(env, *this),
                napi_enumerable),
              InstanceValue(
                "SeekTableMetadata",
                SeekTableMetadata::init(env, *this),
                napi_enumerable),
              InstanceValue("SeekPoint", SeekPoint::init(env, *this), napi_enumerable),
              InstanceValue(
                "VorbisCommentMetadata",
                VorbisCommentMetadata::init(env, *this),
                napi_enumerable),
              InstanceValue(
                "CueSheetMetadata",
                CueSheetMetadata::init(env, *this),
                napi_enumerable),
              InstanceValue("CueSheetIndex", CueSheetIndex::init(env, *this), napi_enumerable),
              InstanceValue("CueSheetTrack", CueSheetTrack::init(env, *this), napi_enumerable),
              InstanceValue("PictureMetadata", PictureMetadata::init(env, *this), napi_enumerable),
              InstanceValue("UnknownMetadata", UnknownMetadata::init(env, *this), napi_enumerable),
            }),
          napi_enumerable),
        InstanceValue("metadata0", initMetadata0(env), napi_enumerable),
        InstanceValue("SimpleIterator", initMetadata1(env, *this), napi_enumerable),
        InstanceValue("Chain", initMetadata2Chain(env, *this), napi_enumerable),
        InstanceValue("Iterator", initMetadata2Iterator(env, *this), napi_enumerable),
        InstanceValue("fns", initFns(env), napi_enumerable),
      });

    objectFreeze(exports);

    module = Persistent(exports);
  }

}

using namespace flac_bindings;
NODE_API_NAMED_ADDON(flac_bindings, FlacAddon);
