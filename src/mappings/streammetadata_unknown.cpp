#include "../flac_addon.hpp"
#include "mappings.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  Function UnknownMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    Function constructor = DefineClass(
      env,
      "UnknownMetadata",
      {
        InstanceAccessor(
          "data",
          &UnknownMetadata::getData,
          nullptr,
          napi_property_attributes::napi_enumerable),
      });

    addon.unknownMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  UnknownMetadata::UnknownMetadata(const CallbackInfo& info):
      ObjectWrap<UnknownMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_UNDEFINED) {}

  Napi::Value UnknownMetadata::getData(const CallbackInfo& info) {
    if (data->data.unknown.data == nullptr) {
      return info.Env().Null();
    }

    EscapableHandleScope scope(info.Env());
    if (dataBuffer.isEmpty()) {
      dataBuffer.setFromWrap(info.Env(), data->data.unknown.data, data->length);
    }

    return scope.Escape(dataBuffer.value());
  }

}
