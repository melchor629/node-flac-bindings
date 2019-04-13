#include "mappings.hpp"

#undef min
#undef max

namespace flac_bindings {

    V8_GETTER(UnknownMetadata::data) {
        unwrap(UnknownMetadata);
        info.GetReturnValue().Set(WrapPointer(self->metadata->data.unknown.data, self->metadata->length).ToLocalChecked());
    }

    NAN_METHOD(UnknownMetadata::create) {
        UnknownMetadata* self = new UnknownMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            FLAC__MetadataType undefinedNum = std::min(FLAC__MetadataType::FLAC__MAX_METADATA_TYPE, std::max(
                FLAC__MetadataType::FLAC__METADATA_TYPE_UNDEFINED,
                numberFromJs<FLAC__MetadataType>(info[0]).FromMaybe(FLAC__MetadataType::FLAC__METADATA_TYPE_UNDEFINED)
            ));
            Local<Value> args[] = { numberToJs<int>(undefinedNum) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
        }

        nativeReadOnlyProperty(info.This(), "data", data);

        info.GetReturnValue().Set(info.This());
    }

    Nan::Persistent<Function> UnknownMetadata::jsFunction;
    NAN_MODULE_INIT(UnknownMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("UnknownMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("UnknownMetadata").ToLocalChecked(), metadata);
    }

}
