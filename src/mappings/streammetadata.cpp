#include "mappings.hpp"
#include "../metadata/metadata.hpp"

namespace flac_bindings {

    using namespace node;

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata* i);

    V8_GETTER(Metadata::type) {
        unwrap(Metadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->type));
    }

    V8_GETTER(Metadata::isLast) {
        unwrap(Metadata);
        info.GetReturnValue().Set(Nan::New<Boolean>(self->metadata->is_last));
    }

    V8_GETTER(Metadata::length) {
        unwrap(Metadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->length));
    }

    NAN_METHOD(Metadata::create) {
        if(info.IsConstructCall()) {
            Nan::ThrowError("The Metadata class is abstract and cannot be instantiated");
            return;
        }

        if(info.Length() >= 1) {
            Metadata* metadata = Nan::ObjectWrap::Unwrap<Metadata>(info.This());
            if(Buffer::HasInstance(info[0])) {
                metadata->metadata = UnwrapPointer<FLAC__StreamMetadata>(info[0]);
                metadata->hasToBeDeleted = Nan::To<bool>(info[1]).FromMaybe(false);
            } else if(info[0]->IsNumber()) {
                int number = numberFromJs<int>(info[0]).FromJust();
                FLAC__MetadataType type = (FLAC__MetadataType) number;
                metadata->metadata = FLAC__metadata_object_new(type);
                metadata->hasToBeDeleted = true;
            } else {
                delete metadata;
                Nan::ThrowTypeError("Expected number of Buffer as parameter");
                return;
            }

            nativeReadOnlyProperty(info.This(), "type", type);
            nativeReadOnlyProperty(info.This(), "isLast", isLast);
            nativeReadOnlyProperty(info.This(), "length", length);

            info.GetReturnValue().Set(info.This());
        } else {
            Nan::ThrowError("Expected one argument, 0 given");
        }
    }

    NAN_METHOD(Metadata::clone) {
        unwrap(Metadata);

        FLAC__StreamMetadata* newMetadata = FLAC__metadata_object_clone(self->metadata);
        if(newMetadata == nullptr) {
            Nan::ThrowError("Could not clone: no enough memory");
        } else {
            auto newMetadataJs = structToJs(newMetadata);
            auto metadataImpl = Nan::ObjectWrap::Unwrap<Metadata>(newMetadataJs);
            metadataImpl->hasToBeDeleted = true;
            info.GetReturnValue().Set(newMetadataJs);
        }
    }

    NAN_METHOD(Metadata::isEqual) {
        unwrap(Metadata);
        if(info.Length() == 0 || info[0].IsEmpty()) {
            Nan::ThrowTypeError("Expected one argument of type Metadata, 0 given");
            return;
        }

        if(!info[0]->IsObject()) {
            Nan::ThrowTypeError("Expected argument to be a Metadata object");
            return;
        }

        Metadata* other = Nan::ObjectWrap::Unwrap<Metadata>(Nan::To<Object>(info[0]).ToLocalChecked());
        info.GetReturnValue().Set(Nan::New<Boolean>(FLAC__metadata_object_is_equal(self->metadata, other->metadata)));
    }

    Nan::Persistent<Function> Metadata::metadataJs;
    Nan::Persistent<FunctionTemplate> Metadata::metadataProtoJs;
    Metadata::~Metadata() {
        if(hasToBeDeleted && metadata != nullptr) {
            FLAC__metadata_object_delete(metadata);
        }
    }

    NAN_MODULE_INIT(Metadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("Metadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetPrototypeMethod(tpl, "clone", clone);
        Nan::SetPrototypeMethod(tpl, "isEqual", isEqual);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        metadataJs.Reset(metadata);
        metadataProtoJs.Reset(tpl);
        Nan::Set(target, Nan::New("Metadata").ToLocalChecked(), metadata);
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata* i) {
        Local<Function> classFunction;
        switch(i->type) {
            case FLAC__METADATA_TYPE_STREAMINFO: classFunction = StreamInfoMetadata::getFunction(); break;
            case FLAC__METADATA_TYPE_PADDING: classFunction = PaddingMetadata::getFunction(); break;
            case FLAC__METADATA_TYPE_APPLICATION: classFunction = ApplicationMetadata::getFunction(); break;
            case FLAC__METADATA_TYPE_SEEKTABLE: classFunction = SeekTableMetadata::getFunction(); break;
            case FLAC__METADATA_TYPE_VORBIS_COMMENT: classFunction = VorbisCommentMetadata::getFunction(); break;
            case FLAC__METADATA_TYPE_CUESHEET: classFunction = CueSheetMetadata::getFunction(); break;
            case FLAC__METADATA_TYPE_PICTURE: classFunction = PictureMetadata::getFunction(); break;
            default: classFunction = UnknownMetadata::getFunction(); break;
        }

        Local<Value> args[] = { WrapPointer(i).ToLocalChecked(), Nan::False() };
        auto metadata = Nan::NewInstance(classFunction, 2, args);
        return metadata.ToLocalChecked();
    }

}
