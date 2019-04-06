#include "mappings.hpp"

namespace flac_bindings {

    NAN_METHOD(PaddingMetadata::create) {
        PaddingMetadata* self = new PaddingMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            Local<Value> args[] = { Nan::New<Number>(FLAC__MetadataType::FLAC__METADATA_TYPE_PADDING) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
            if(info.Length() > 0 && info[0]->IsNumber()) self->metadata->length += numberFromJs<uint32_t>(info[0]).FromMaybe(0);
        }

        info.GetReturnValue().Set(info.This());
    }

    Nan::Persistent<Function> PaddingMetadata::paddingMetadataJs;
    NAN_MODULE_INIT(PaddingMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("PaddingMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        paddingMetadataJs.Reset(metadata);
        Nan::Set(target, Nan::New("PaddingMetadata").ToLocalChecked(), metadata);
    }

}
