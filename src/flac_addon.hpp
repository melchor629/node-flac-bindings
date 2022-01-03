#pragma once

#include <napi.h>

namespace flac_bindings {

  class FlacAddon: public Napi::Addon<FlacAddon> {
  public:
    FlacAddon(Napi::Env env, Napi::Object exports);

    Napi::ObjectReference module;
    Napi::FunctionReference decoderBuilderConstructor;
    Napi::FunctionReference decoderConstructor;
    Napi::FunctionReference encoderBuilderConstructor;
    Napi::FunctionReference encoderConstructor;
    Napi::FunctionReference streamInfoMetadataConstructor;
    Napi::FunctionReference paddingMetadataConstructor;
    Napi::FunctionReference applicationMetadataConstructor;
    Napi::FunctionReference seekTableMetadataConstructor;
    Napi::FunctionReference seekPointConstructor;
    Napi::FunctionReference vorbisCommentMetadataConstructor;
    Napi::FunctionReference cueSheetMetadataConstructor;
    Napi::FunctionReference cueSheetIndexConstructor;
    Napi::FunctionReference cueSheetTrackConstructor;
    Napi::FunctionReference pictureMetadataConstructor;
    Napi::FunctionReference unknownMetadataConstructor;
    Napi::FunctionReference simpleIteratorConstructor;
    Napi::FunctionReference chainConstructor;
    Napi::FunctionReference iteratorConstructor;
    Napi::FunctionReference nativeIteratorConstructor;
  };

}
