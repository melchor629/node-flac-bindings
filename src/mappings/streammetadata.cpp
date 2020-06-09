#include <FLAC/metadata.h>
#include "mappings.hpp"
#include "../utils/pointer.hpp"

namespace flac_bindings {

    using namespace Napi;

    template<>
    Mapping<FLAC__StreamMetadata>& Mapping<FLAC__StreamMetadata>::fromJs(const Value&);
    template<>
    Value Mapping<FLAC__StreamMetadata>::toJs(const Env&, FLAC__StreamMetadata*, bool);

    Metadata::Metadata(const CallbackInfo& info, FLAC__MetadataType type): Mapping<FLAC__StreamMetadata>(info) {
        auto self = info.This().As<Object>();

        if(data == nullptr) {
            data = FLAC__metadata_object_new(type);
            shouldBeDeleted = true;
            if(data == nullptr) {
                throw Error::New(info.Env(), "No memory left - Could not create new metadata object");
            }
        }

        self.DefineProperties({
            PropertyDescriptor::Accessor(info.Env(), self, "type", &Metadata::getType, napi_property_attributes::napi_enumerable, this),
            PropertyDescriptor::Accessor(info.Env(), self, "isLast", &Metadata::getIsLast, napi_property_attributes::napi_enumerable, this),
            PropertyDescriptor::Accessor(info.Env(), self, "length", &Metadata::getLength, napi_property_attributes::napi_enumerable, this),
            PropertyDescriptor::Function(info.Env(), self, "clone", &Metadata::clone, napi_property_attributes::napi_default, this),
            PropertyDescriptor::Function(info.Env(), self, "isEqual", &Metadata::isEqual, napi_property_attributes::napi_default, this),
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
        if(newMetadata == nullptr) {
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
        if(!value.IsObject()) {
            throw Napi::TypeError::New(value.Env(), "Expected "s + value.ToString().Utf8Value() + " to be object"s);
        }

        auto obj = value.As<Object>();
        if(obj.InstanceOf(StreamInfoMetadata::getConstructor())) {
            return *StreamInfoMetadata::Unwrap(obj);
        } else if(obj.InstanceOf(PaddingMetadata::getConstructor())) {
            return *PaddingMetadata::Unwrap(obj);
        } else if(obj.InstanceOf(ApplicationMetadata::getConstructor())) {
            return *ApplicationMetadata::Unwrap(obj);
        } else if(obj.InstanceOf(SeekTableMetadata::getConstructor())) {
            return *SeekTableMetadata::Unwrap(obj);
        } else if(obj.InstanceOf(VorbisCommentMetadata::getConstructor())) {
            return *VorbisCommentMetadata::Unwrap(obj);
        } else if(obj.InstanceOf(CueSheetMetadata::getConstructor())) {
            return *CueSheetMetadata::Unwrap(obj);
        } else if(obj.InstanceOf(PictureMetadata::getConstructor())) {
            return *PictureMetadata::Unwrap(obj);
        } else if(obj.InstanceOf(UnknownMetadata::getConstructor())) {
            return *UnknownMetadata::Unwrap(obj);
        } else {
            throw TypeError::New(value.Env(), "Expected "s + value.ToString().Utf8Value() + " to be an instance of Metadata");
        }
    }

    template<>
    Value Mapping<FLAC__StreamMetadata>::toJs(const Env& env, FLAC__StreamMetadata* metadata, bool deleteHint) {
        if(metadata == nullptr) {
            return env.Null();
        }

        EscapableHandleScope scope(env);
        Function constructor;

        switch(metadata->type) {
            case FLAC__MetadataType::FLAC__METADATA_TYPE_STREAMINFO:
                constructor = StreamInfoMetadata::getConstructor();
                break;
            case FLAC__MetadataType::FLAC__METADATA_TYPE_PADDING:
                constructor = PaddingMetadata::getConstructor();
                break;
            case FLAC__MetadataType::FLAC__METADATA_TYPE_APPLICATION:
                constructor = ApplicationMetadata::getConstructor();
                break;
            case FLAC__MetadataType::FLAC__METADATA_TYPE_SEEKTABLE:
                constructor = SeekTableMetadata::getConstructor();
                break;
            case FLAC__MetadataType::FLAC__METADATA_TYPE_VORBIS_COMMENT:
                constructor = VorbisCommentMetadata::getConstructor();
                break;
            case FLAC__MetadataType::FLAC__METADATA_TYPE_CUESHEET:
                constructor = CueSheetMetadata::getConstructor();
                break;
            case FLAC__MetadataType::FLAC__METADATA_TYPE_PICTURE:
                constructor = PictureMetadata::getConstructor();
                break;
            default:
                constructor = UnknownMetadata::getConstructor();
                break;
        }

        auto object = constructor.New({pointer::wrap(env, metadata), booleanToJs(env, deleteHint)});
        return scope.Escape(object);
    }

    Metadata::~Metadata() {
        if(shouldBeDeleted) {
            FLAC__metadata_object_delete(data);
        }
    }

}
