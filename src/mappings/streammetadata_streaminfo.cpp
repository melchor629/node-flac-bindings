#include "defs.hpp"
#include "mappings.hpp"

namespace flac_bindings {

    V8_GETTER(StreamInfoMetadata::minBlocksize) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New<Number>(self->metadata->data.stream_info.min_blocksize));
    }

    V8_SETTER(StreamInfoMetadata::minBlocksize) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.min_blocksize = getValue(uint32_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::maxBlocksize) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.stream_info.max_blocksize));
    }

    V8_SETTER(StreamInfoMetadata::maxBlocksize) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.max_blocksize = getValue(uint32_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::minFramesize) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.stream_info.min_framesize));
    }

    V8_SETTER(StreamInfoMetadata::minFramesize) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.min_framesize = getValue(uint32_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::maxFramesize) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.stream_info.max_framesize));
    }

    V8_SETTER(StreamInfoMetadata::maxFramesize) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.max_framesize = getValue(uint32_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::channels) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.stream_info.channels));
    }

    V8_SETTER(StreamInfoMetadata::channels) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.channels = getValue(uint32_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::bitsPerSample) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.stream_info.bits_per_sample));
    }

    V8_SETTER(StreamInfoMetadata::bitsPerSample) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.bits_per_sample = getValue(uint32_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::sampleRate) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.stream_info.sample_rate));
    }

    V8_SETTER(StreamInfoMetadata::sampleRate) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.sample_rate = getValue(uint32_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::totalSamples) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(Nan::New<Number>(self->metadata->data.stream_info.total_samples));
    }

    V8_SETTER(StreamInfoMetadata::totalSamples) {
        unwrap(StreamInfoMetadata);
        checkValue(Number) {
            self->metadata->data.stream_info.total_samples = getValue(int64_t);
        }
    }

    V8_GETTER(StreamInfoMetadata::md5sum) {
        unwrap(StreamInfoMetadata);
        info.GetReturnValue().Set(WrapPointer(self->metadata->data.stream_info.md5sum, 16).ToLocalChecked());
    }

    V8_SETTER(StreamInfoMetadata::md5sum) {
        unwrap(StreamInfoMetadata);
        checkValueIsBuffer() {
            FLAC__byte* data = (FLAC__byte*) node::Buffer::Data(value);
            size_t dataLength = node::Buffer::Length(value);
            if(dataLength >= 16) {
                memcpy(self->metadata->data.stream_info.md5sum, data, 16);
            } else {
                Nan::ThrowError("Buffer must be of 16 bytes length");
            }
        }
    }

    NAN_METHOD(StreamInfoMetadata::create) {
        StreamInfoMetadata* self = new StreamInfoMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            Local<Value> args[] = { Nan::New<Number>(FLAC__MetadataType::FLAC__METADATA_TYPE_STREAMINFO) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
        }

        nativeProperty(info.This(), "minBlocksize", minBlocksize);
        nativeProperty(info.This(), "maxBlocksize", maxBlocksize);
        nativeProperty(info.This(), "minFramesize", minFramesize);
        nativeProperty(info.This(), "maxFramesize", maxFramesize);
        nativeProperty(info.This(), "channels", channels);
        nativeProperty(info.This(), "bitsPerSample", bitsPerSample);
        nativeProperty(info.This(), "sampleRate", sampleRate);
        nativeProperty(info.This(), "totalSamples", totalSamples);
        nativeProperty(info.This(), "md5sum", md5sum);

        info.GetReturnValue().Set(info.This());
    }

    Nan::Persistent<Function> StreamInfoMetadata::jsFunction;
    NAN_MODULE_INIT(StreamInfoMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("StreamInfoMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("StreamInfoMetadata").ToLocalChecked(), metadata);
    }

}
