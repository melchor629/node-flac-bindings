#include "../flac_addon.hpp"
#include "mappings.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  Function ApplicationMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    Function constructor = DefineClass(
      env,
      "ApplicationMetadata",
      {
        InstanceAccessor(
          "id",
          &ApplicationMetadata::getId,
          &ApplicationMetadata::setId,
          napi_property_attributes::napi_enumerable),
        InstanceAccessor(
          "data",
          &ApplicationMetadata::getData,
          &ApplicationMetadata::setData,
          napi_property_attributes::napi_enumerable),
      });

    addon.applicationMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  ApplicationMetadata::ApplicationMetadata(const CallbackInfo& info):
      ObjectWrap<ApplicationMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_APPLICATION) {}

  Napi::Value ApplicationMetadata::getId(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    if (idBuffer.isEmpty()) {
      idBuffer.setFromWrap(info.Env(), data->data.application.id, 4);
    }

    return scope.Escape(idBuffer.value());
  }

  void ApplicationMetadata::setId(const CallbackInfo& info, const Napi::Value& value) {
    FLAC__byte* ptr;
    size_t length;
    std::tie(ptr, length) = pointer::fromBuffer<FLAC__byte>(value);

    if (length < 4) {
      throw RangeError::New(info.Env(), "Data length is less than 4");
    }

    memcpy(data->data.application.id, ptr, 4 * sizeof(FLAC__byte));
  }

  Napi::Value ApplicationMetadata::getData(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    if (dataBuffer.isEmpty()) {
      dataBuffer.setFromWrap(info.Env(), data->data.application.data, data->length - 4);
    }

    return scope.Escape(dataBuffer.value());
  }

  void ApplicationMetadata::setData(const CallbackInfo& info, const Napi::Value& value) {
    HandleScope scope(info.Env());
    FLAC__byte* ptr;
    size_t length;
    std::tie(ptr, length) = pointer::fromBuffer<FLAC__byte>(value);

    FLAC__bool ret = FLAC__metadata_object_application_set_data(data, ptr, length, true);

    if (!ret) {
      throw Error::New(info.Env(), "Could not allocate memory to store the data");
    }

    if (!dataBuffer.isEmpty()) {
      dataBuffer.clear();
    }
  }

}
