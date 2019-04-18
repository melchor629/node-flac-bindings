#include "mappings.hpp"
#include "../metadata/metadata.hpp"

namespace flac_bindings {

    V8_GETTER(ApplicationMetadata::id) {
        unwrap(ApplicationMetadata);
        info.GetReturnValue().Set(WrapPointer(self->metadata->data.application.id, 4).ToLocalChecked());
    }

    V8_SETTER(ApplicationMetadata::id) {
        unwrap(ApplicationMetadata);
        checkValueIsBuffer() {
            char* buffer = Buffer::Data(value);
            size_t bufferLength = Buffer::Length(value);
            if(bufferLength < 4) {
                Nan::ThrowError(Nan::Error("Buffer must have at least 4 bytes length"));
            } else {
                for(int i = 0; i < 4; i++) {
                    self->metadata->data.application.id[i] = buffer[i];
                }
            }
        }
    }

    V8_GETTER(ApplicationMetadata::data) {
        unwrap(ApplicationMetadata);
        info.GetReturnValue().Set(WrapPointer(self->metadata->data.application.data, self->metadata->length - 4).ToLocalChecked());
    }

    V8_SETTER(ApplicationMetadata::data) {
        unwrap(ApplicationMetadata);
        auto o1 = info.Data();
        Nan::Utf8String str(Nan::ToDetailString(o1).ToLocalChecked());
        printf("%s\n", *str);
        checkValueIsBuffer() {
            FLAC__byte* buffer = (FLAC__byte*) Buffer::Data(value);
            size_t bufferLength = Buffer::Length(value);
            bool res = FLAC__metadata_object_application_set_data(self->metadata, buffer, bufferLength, true);
            if(!res) {
                Nan::ThrowError("Could not allocate memory for copy");
            }
        }
    }

    NAN_METHOD(ApplicationMetadata::create) {
        ApplicationMetadata* self = new ApplicationMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            Local<Value> args[] = { numberToJs<int>(FLAC__MetadataType::FLAC__METADATA_TYPE_APPLICATION) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
        }

        nativeProperty(info.This(), "id", id);
        nativeProperty(info.This(), "data", data);

        info.GetReturnValue().Set(info.This());
    }

    Nan::Persistent<Function> ApplicationMetadata::applicationMetadataJs;
    NAN_MODULE_INIT(ApplicationMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("ApplicationMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        applicationMetadataJs.Reset(metadata);
        Nan::Set(target, Nan::New("ApplicationMetadata").ToLocalChecked(), metadata);
    }

}
