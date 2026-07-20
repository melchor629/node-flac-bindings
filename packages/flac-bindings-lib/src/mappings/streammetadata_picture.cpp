#include "../flac_addon.hpp"
#include "mappings.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  Function PictureMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    napi_property_attributes attributes = napi_property_attributes::napi_enumerable;
    Function constructor = DefineClass(
      env,
      "PictureMetadata",
      {
        InstanceAccessor(
          "pictureType",
          &PictureMetadata::getPictureType,
          &PictureMetadata::setPictureType,
          attributes),
        InstanceAccessor(
          "mimeType",
          &PictureMetadata::getMimeType,
          &PictureMetadata::setMimeType,
          attributes),
        InstanceAccessor(
          "description",
          &PictureMetadata::getDescription,
          &PictureMetadata::setDescription,
          attributes),
        InstanceAccessor(
          "width",
          &PictureMetadata::getWidth,
          &PictureMetadata::setWidth,
          attributes),
        InstanceAccessor(
          "height",
          &PictureMetadata::getHeight,
          &PictureMetadata::setHeight,
          attributes),
        InstanceAccessor(
          "depth",
          &PictureMetadata::getDepth,
          &PictureMetadata::setDepth,
          attributes),
        InstanceAccessor(
          "colors",
          &PictureMetadata::getColors,
          &PictureMetadata::setColors,
          attributes),
        InstanceAccessor("data", &PictureMetadata::getData, &PictureMetadata::setData, attributes),
        InstanceMethod("isLegal", &PictureMetadata::isLegal),
      });

    addon.pictureMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  PictureMetadata::PictureMetadata(const CallbackInfo& info):
      ObjectWrap<PictureMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_PICTURE) {}

  Napi::Value PictureMetadata::getPictureType(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.picture.type);
  }

  void PictureMetadata::setPictureType(const CallbackInfo&, const Napi::Value& value) {
    auto type = numberFromJs<FLAC__StreamMetadata_Picture_Type>(value);
    data->data.picture.type = type;
  }

  Napi::Value PictureMetadata::getMimeType(const CallbackInfo& info) {
    return String::New(info.Env(), data->data.picture.mime_type);
  }

  void PictureMetadata::setMimeType(const CallbackInfo& info, const Napi::Value& value) {
    auto mimeType = stringFromJs(value);
    auto ret =
      FLAC__metadata_object_picture_set_mime_type(data, const_cast<char*>(mimeType.c_str()), true);
    if (!ret) {
      throw Error::New(info.Env(), "Could not allocate memory to store the string");
    }
  }

  Napi::Value PictureMetadata::getDescription(const CallbackInfo& info) {
    return String::New(info.Env(), (const char*) data->data.picture.description);
  }

  void PictureMetadata::setDescription(const CallbackInfo& info, const Napi::Value& value) {
    auto description = stringFromJs(value);
    auto ret =
      FLAC__metadata_object_picture_set_description(data, (FLAC__byte*) description.c_str(), true);
    if (!ret) {
      throw Error::New(info.Env(), "Could not allocate memory to store the string");
    }
  }

  Napi::Value PictureMetadata::getWidth(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.picture.width);
  }

  void PictureMetadata::setWidth(const CallbackInfo&, const Napi::Value& value) {
    auto width = numberFromJs<uint32_t>(value);
    data->data.picture.width = width;
  }

  Napi::Value PictureMetadata::getHeight(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.picture.height);
  }

  void PictureMetadata::setHeight(const CallbackInfo&, const Napi::Value& value) {
    auto height = numberFromJs<uint32_t>(value);
    data->data.picture.height = height;
  }

  Napi::Value PictureMetadata::getDepth(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.picture.depth);
  }

  void PictureMetadata::setDepth(const CallbackInfo&, const Napi::Value& value) {
    auto depth = numberFromJs<uint32_t>(value);
    data->data.picture.depth = depth;
  }

  Napi::Value PictureMetadata::getColors(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.picture.colors);
  }

  void PictureMetadata::setColors(const CallbackInfo&, const Napi::Value& value) {
    auto colors = numberFromJs<uint32_t>(value);
    data->data.picture.colors = colors;
  }

  Napi::Value PictureMetadata::getData(const CallbackInfo& info) {
    if (data->data.picture.data == nullptr) {
      return info.Env().Null();
    }

    EscapableHandleScope scope(info.Env());
    if (dataBuffer.isEmpty()) {
      dataBuffer.setFromWrap(info.Env(), data->data.picture.data, data->data.picture.data_length);
    }

    return scope.Escape(dataBuffer.value());
  }

  void PictureMetadata::setData(const CallbackInfo& info, const Napi::Value& value) {
    FLAC__byte* ptr = nullptr;
    size_t length = 0;
    if (!value.IsNull() && !value.IsUndefined()) {
      std::tie(ptr, length) = pointer::fromBuffer<FLAC__byte>(value);
    }
    if (length == 0) {
      FLAC__metadata_object_picture_set_data(data, (uint8_t*) nullptr, 0, false);
    } else {
      auto ret = FLAC__metadata_object_picture_set_data(data, ptr, length, true);
      if (!ret) {
        throw Error::New(info.Env(), "Could not allocate memory to copy the data");
      }
    }

    if (!dataBuffer.isEmpty()) {
      dataBuffer.clear();
    }
  }

  Napi::Value PictureMetadata::isLegal(const CallbackInfo& info) {
    const char* sadness = nullptr;
    FLAC__bool ret = FLAC__metadata_object_picture_is_legal(data, &sadness);
    if (ret) {
      return info.Env().Null();
    } else {
      return String::New(info.Env(), sadness);
    }
  }

}
