#include "defs.hpp"
#include "mappings.hpp"
#include "../metadata.hpp"

namespace flac_bindings {

    V8_GETTER(PictureMetadata::pictureType) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(Nan::New((uint32_t) self->metadata->data.picture.type));
    }

    V8_SETTER(PictureMetadata::pictureType) {
        unwrap(PictureMetadata);
        checkValue(Number) {
            self->metadata->data.picture.type = (FLAC__StreamMetadata_Picture_Type) getValue(uint32_t);
        }
    }

    V8_GETTER(PictureMetadata::mimeType) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.picture.mime_type).ToLocalChecked());
    }

    V8_SETTER(PictureMetadata::mimeType) {
        unwrap(PictureMetadata);
        checkValue(String) {
            Nan::Utf8String mime(value);
            bool ret = FLAC__metadata_object_picture_set_mime_type(self->metadata, *mime, true);
            if(!ret) {
                Nan::ThrowError("Could not allocate memory to store the string");
            }
        }
    }

    V8_GETTER(PictureMetadata::description) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(Nan::New((char*) self->metadata->data.picture.description).ToLocalChecked());
    }

    V8_SETTER(PictureMetadata::description) {
        unwrap(PictureMetadata);
        checkValue(String) {
            Nan::Utf8String descr(value);
            bool ret = FLAC__metadata_object_picture_set_description(self->metadata, (FLAC__byte*) *descr, true);
            if(!ret) {
                Nan::ThrowError("Could not allocate memory to store the string");
            }
        }
    }

    V8_GETTER(PictureMetadata::width) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.picture.width));
    }

    V8_SETTER(PictureMetadata::width) {
        unwrap(PictureMetadata);
        checkValue(Number) {
            self->metadata->data.picture.width = getValue(uint32_t);
        }
    }

    V8_GETTER(PictureMetadata::height) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.picture.height));
    }

    V8_SETTER(PictureMetadata::height) {
        unwrap(PictureMetadata);
        checkValue(Number) {
            self->metadata->data.picture.height = getValue(uint32_t);
        }
    }

    V8_GETTER(PictureMetadata::depth) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.picture.depth));
    }

    V8_SETTER(PictureMetadata::depth) {
        unwrap(PictureMetadata);
        checkValue(Number) {
            self->metadata->data.picture.depth = getValue(uint32_t);
        }
    }

    V8_GETTER(PictureMetadata::colors) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.picture.colors));
    }

    V8_SETTER(PictureMetadata::colors) {
        unwrap(PictureMetadata);
        checkValue(Number) {
            self->metadata->data.picture.colors = getValue(uint32_t);
        }
    }

    V8_GETTER(PictureMetadata::data) {
        unwrap(PictureMetadata);
        info.GetReturnValue().Set(WrapPointer(self->metadata->data.picture.data, self->metadata->data.picture.data_length).ToLocalChecked());
    }

    V8_SETTER(PictureMetadata::data) {
        unwrap(PictureMetadata);
        checkValueIsBuffer() {
            FLAC__byte* data = (FLAC__byte*) node::Buffer::Data(value);
            size_t dataLength = node::Buffer::Length(value);
            FLAC__metadata_object_picture_set_data(self->metadata, data, dataLength, true);
        }
    }

    NAN_METHOD(PictureMetadata::create) {
        PictureMetadata* self = new PictureMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            Local<Value> args[] = { Nan::New<Number>(FLAC__MetadataType::FLAC__METADATA_TYPE_PICTURE) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
        }

        nativeProperty(info.This(), "pictureType", pictureType);
        nativeProperty(info.This(), "mimeType", mimeType);
        nativeProperty(info.This(), "description", description);
        nativeProperty(info.This(), "width", width);
        nativeProperty(info.This(), "height", height);
        nativeProperty(info.This(), "depth", depth);
        nativeProperty(info.This(), "colors", colors);
        nativeProperty(info.This(), "data", data);

        info.GetReturnValue().Set(info.This());
    }

    NAN_METHOD(PictureMetadata::isLegal) {
        unwrap(PictureMetadata);
        const char* violation = nullptr;
        bool legal = FLAC__metadata_object_picture_is_legal(self->metadata, &violation);
        if(legal) {
            info.GetReturnValue().Set(Nan::True());
        } else {
            info.GetReturnValue().Set(Nan::New(violation).ToLocalChecked());
        }
    }

    Nan::Persistent<Function> PictureMetadata::jsFunction;
    NAN_MODULE_INIT(PictureMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("PictureMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Nan::SetPrototypeMethod(tpl, "isLegal", isLegal);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("PictureMetadata").ToLocalChecked(), metadata);
    }

}
