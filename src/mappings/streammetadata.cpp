#include "../flac_addon.hpp"
#include "../utils/pointer.hpp"
#include "mappings.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  template<>
  Mapping<FLAC__StreamMetadata>& Mapping<FLAC__StreamMetadata>::fromJs(const Value&);
  template<>
  Value Mapping<FLAC__StreamMetadata>::toJs(const Env&, FLAC__StreamMetadata*, bool);

  Metadata::Metadata(const CallbackInfo& info, FLAC__MetadataType type):
      Mapping<FLAC__StreamMetadata>(info) {
    auto self = info.This().As<Object>();

    if (data == nullptr) {
      data = FLAC__metadata_object_new(type);
      shouldBeDeleted = true;
      if (data == nullptr) {
        throw Error::New(info.Env(), "No memory left - Could not create new metadata object");
      }
    }

    self.DefineProperties({
      PropertyDescriptor::Accessor(
        info.Env(),
        self,
        "type",
        &Metadata::getType,
        napi_property_attributes::napi_enumerable,
        this),
      PropertyDescriptor::Accessor(
        info.Env(),
        self,
        "isLast",
        &Metadata::getIsLast,
        napi_property_attributes::napi_enumerable,
        this),
      PropertyDescriptor::Accessor(
        info.Env(),
        self,
        "length",
        &Metadata::getLength,
        napi_property_attributes::napi_enumerable,
        this),
      PropertyDescriptor::Function(
        info.Env(),
        self,
        "clone",
        &Metadata::clone,
        napi_property_attributes::napi_default,
        this),
      PropertyDescriptor::Function(
        info.Env(),
        self,
        "isEqual",
        &Metadata::isEqual,
        napi_property_attributes::napi_default,
        this),
    });
  }

  Value Metadata::getType(const CallbackInfo& info) {
    Metadata* self = (Metadata*) info.Data();
    return numberToJs(info.Env(), self->data->type);
  }

  Value Metadata::getIsLast(const CallbackInfo& info) {
    Metadata* self = (Metadata*) info.Data();
    return booleanToJs(info.Env(), self->data->is_last);
  }

  Value Metadata::getLength(const CallbackInfo& info) {
    Metadata* self = (Metadata*) info.Data();
    return numberToJs(info.Env(), self->data->length);
  }

  Value Metadata::clone(const CallbackInfo& info) {
    Metadata* self = (Metadata*) info.Data();
    auto newMetadata = FLAC__metadata_object_clone(self->data);
    if (newMetadata == nullptr) {
      throw Error::New(info.Env(), "No memory left - Could not clone metadata object");
    }

    return Mapping::toJs(info.Env(), newMetadata, true);
  }

  Value Metadata::isEqual(const CallbackInfo& info) {
    Metadata* self = (Metadata*) info.Data();
    auto other = fromJs(info[0]);
    auto areEqual = FLAC__metadata_object_is_equal(self->data, other);
    return booleanToJs(info.Env(), areEqual);
  }

  template<>
  Mapping<FLAC__StreamMetadata>& Mapping<FLAC__StreamMetadata>::fromJs(const Value& value) {
    HandleScope scope(value.Env());
    if (!value.IsObject()) {
      throw Napi::TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be object"s);
    }

    auto obj = value.As<Object>();
    auto addon = value.Env().GetInstanceData<FlacAddon>();
    if (obj.InstanceOf(addon->streamInfoMetadataConstructor.Value())) {
      return *StreamInfoMetadata::Unwrap(obj);
    } else if (obj.InstanceOf(addon->paddingMetadataConstructor.Value())) {
      return *PaddingMetadata::Unwrap(obj);
    } else if (obj.InstanceOf(addon->applicationMetadataConstructor.Value())) {
      return *ApplicationMetadata::Unwrap(obj);
    } else if (obj.InstanceOf(addon->seekTableMetadataConstructor.Value())) {
      return *SeekTableMetadata::Unwrap(obj);
    } else if (obj.InstanceOf(addon->vorbisCommentMetadataConstructor.Value())) {
      return *VorbisCommentMetadata::Unwrap(obj);
    } else if (obj.InstanceOf(addon->cueSheetMetadataConstructor.Value())) {
      return *CueSheetMetadata::Unwrap(obj);
    } else if (obj.InstanceOf(addon->pictureMetadataConstructor.Value())) {
      return *PictureMetadata::Unwrap(obj);
    } else if (obj.InstanceOf(addon->unknownMetadataConstructor.Value())) {
      return *UnknownMetadata::Unwrap(obj);
    } else {
      throw TypeError::New(
        value.Env(),
        "Expected "s + value.ToString().Utf8Value() + " to be an instance of Metadata");
    }
  }

  template<>
  Value Mapping<FLAC__StreamMetadata>::toJs(
    const Env& env,
    FLAC__StreamMetadata* metadata,
    bool deleteHint) {
    if (metadata == nullptr) {
      return env.Null();
    }

    EscapableHandleScope scope(env);
    FunctionReference* constructor;
    auto addon = Napi::Env(env).GetInstanceData<FlacAddon>();

    switch (metadata->type) {
      case FLAC__MetadataType::FLAC__METADATA_TYPE_STREAMINFO:
        constructor = &addon->streamInfoMetadataConstructor;
        break;
      case FLAC__MetadataType::FLAC__METADATA_TYPE_PADDING:
        constructor = &addon->paddingMetadataConstructor;
        break;
      case FLAC__MetadataType::FLAC__METADATA_TYPE_APPLICATION:
        constructor = &addon->applicationMetadataConstructor;
        break;
      case FLAC__MetadataType::FLAC__METADATA_TYPE_SEEKTABLE:
        constructor = &addon->seekTableMetadataConstructor;
        break;
      case FLAC__MetadataType::FLAC__METADATA_TYPE_VORBIS_COMMENT:
        constructor = &addon->vorbisCommentMetadataConstructor;
        break;
      case FLAC__MetadataType::FLAC__METADATA_TYPE_CUESHEET:
        constructor = &addon->cueSheetMetadataConstructor;
        break;
      case FLAC__MetadataType::FLAC__METADATA_TYPE_PICTURE:
        constructor = &addon->pictureMetadataConstructor;
        break;
      default:
        constructor = &addon->unknownMetadataConstructor;
        break;
    }

    auto object = constructor->New({pointer::wrap(env, metadata), booleanToJs(env, deleteHint)});
    return scope.Escape(object);
  }

  Metadata::~Metadata() {
    if (shouldBeDeleted) {
      FLAC__metadata_object_delete(data);
    }
  }

}
