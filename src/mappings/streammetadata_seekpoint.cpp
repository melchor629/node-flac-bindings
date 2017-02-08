#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(sample_number) {
        getPointer(FLAC__StreamMetadata_SeekPoint) {
            info.GetReturnValue().Set(Nan::New<Number>(m->sample_number));
        }
    }

    static NAN_SETTER(sample_number) {
        getPointer(FLAC__StreamMetadata_SeekPoint) {
            checkValue(Number) {
                m->sample_number = getValue(int64_t);
                info.GetReturnValue().Set(Nan::New<Number>(m->sample_number));
            }
        }
    }

    static NAN_GETTER(stream_offset) {
        getPointer(FLAC__StreamMetadata_SeekPoint) {
            info.GetReturnValue().Set(Nan::New<Number>(m->stream_offset));
        }
    }

    static NAN_SETTER(stream_offset) {
        getPointer(FLAC__StreamMetadata_SeekPoint) {
            checkValue(Number) {
                m->stream_offset = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New<Number>(m->stream_offset));
            }
        }
    }

    static NAN_GETTER(frame_samples) {
        getPointer(FLAC__StreamMetadata_SeekPoint) {
            info.GetReturnValue().Set(Nan::New(m->frame_samples));
        }
    }

    static NAN_SETTER(frame_samples) {
        getPointer(FLAC__StreamMetadata_SeekPoint) {
            checkValue(Number) {
                m->frame_samples = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->frame_samples));
            }
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_SeekPoint* i, Local<Object> &obj) {
        SetGetterSetter(sample_number);
        SetGetterSetter(stream_offset);
        SetGetterSetter(frame_samples);

        Nan::Set(obj, Nan::New("_ptr").ToLocalChecked(), WrapPointer(i, sizeof(FLAC__StreamMetadata_SeekPoint)).ToLocalChecked());
    }

}
