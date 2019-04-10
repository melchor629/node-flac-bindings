#include "mappings.hpp"

namespace flac_bindings {

    V8_GETTER(SeekPoint::sampleNumber) {
        unwrap(SeekPoint);
        info.GetReturnValue().Set(numberToJs(self->point.sample_number));
    }

    V8_SETTER(SeekPoint::sampleNumber) {
        unwrap(SeekPoint);
        Maybe<uint64_t> maybeNumber = numberFromJs<uint64_t>(value);
        if(maybeNumber.IsJust()) {
            self->point.sample_number = maybeNumber.FromJust();
        } else {
            Nan::ThrowTypeError("Expected type to be number or BigInt");
        }
    }

    V8_GETTER(SeekPoint::streamOffset) {
        unwrap(SeekPoint);
        info.GetReturnValue().Set(numberToJs(self->point.stream_offset));
    }

    V8_SETTER(SeekPoint::streamOffset) {
        unwrap(SeekPoint);
        Maybe<uint64_t> maybeNumber = numberFromJs<uint64_t>(value);
        if(maybeNumber.IsJust()) {
            self->point.stream_offset = maybeNumber.FromJust();
        } else {
            Nan::ThrowTypeError("Expected type to be number or BigInt");
        }
    }

    V8_GETTER(SeekPoint::frameSamples) {
        unwrap(SeekPoint);
        info.GetReturnValue().Set(numberToJs(self->point.frame_samples));
    }

    V8_SETTER(SeekPoint::frameSamples) {
        unwrap(SeekPoint);
        checkValue(Number) {
            self->point.frame_samples = getValue(uint32_t);
        }
    }

    NAN_METHOD(SeekPoint::create) {
        if(throwIfNotConstructorCall(info)) return;
        SeekPoint* seekPoint = new SeekPoint;
        seekPoint->Wrap(info.This());

        if(!info[0].IsEmpty() && info[0]->IsObject() && Buffer::HasInstance(info[0].As<Object>())) {
            memcpy(
                &seekPoint->point,
                UnwrapPointer<FLAC__StreamMetadata_SeekPoint>(info[0]),
                sizeof(FLAC__StreamMetadata_SeekPoint)
            );
        } else if(!info[0].IsEmpty()) {
            auto maybeSampleNumber = numberFromJs<uint64_t>(info[0]);
            auto maybeStreamoffset = numberFromJs<uint64_t>(info[1]);
            auto maybeframeSamples = numberFromJs<uint32_t>(info[2]);
            seekPoint->point.sample_number = maybeSampleNumber.FromMaybe(0ul);
            seekPoint->point.stream_offset = maybeStreamoffset.FromMaybe(0ul);
            seekPoint->point.frame_samples = maybeframeSamples.FromMaybe(0u);
        }

        nativeProperty(info.This(), "sampleNumber", sampleNumber);
        nativeProperty(info.This(), "streamOffset", streamOffset);
        nativeProperty(info.This(), "frameSamples", frameSamples);

        info.GetReturnValue().Set(info.This());
    }

    Nan::Persistent<Function> SeekPoint::jsFunction;
    NAN_MODULE_INIT(SeekPoint::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("SeekPoint").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("SeekPoint").ToLocalChecked(), metadata);
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_SeekPoint* point) {
        Local<Value> args[] = { WrapPointer(point).ToLocalChecked(), Nan::False() };
        auto metadata = Nan::NewInstance(SeekPoint::getFunction(), 2, args);
        return metadata.ToLocalChecked();
    }

}
