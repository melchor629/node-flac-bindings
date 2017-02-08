#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(min_blocksize) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New<Number>(m->data.stream_info.min_blocksize));
        }
    }

    static NAN_SETTER(min_blocksize) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.min_blocksize = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.stream_info.min_blocksize));
            }
        }
    }

    static NAN_GETTER(max_blocksize) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.stream_info.max_blocksize));
        }
    }

    static NAN_SETTER(max_blocksize) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.max_blocksize = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.stream_info.max_blocksize));
            }
        }
    }

    static NAN_GETTER(min_framesize) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.stream_info.min_framesize));
        }
    }

    static NAN_SETTER(min_framesize) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.min_framesize = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.stream_info.min_framesize));
            }
        }
    }

    static NAN_GETTER(max_framesize) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.stream_info.max_framesize));
        }
    }

    static NAN_SETTER(max_framesize) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.max_framesize = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.stream_info.max_framesize));
            }
        }
    }

    static NAN_GETTER(channels) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.stream_info.channels));
        }
    }

    static NAN_SETTER(channels) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.channels = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.stream_info.channels));
            }
        }
    }

    static NAN_GETTER(bits_per_sample) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.stream_info.bits_per_sample));
        }
    }

    static NAN_SETTER(bits_per_sample) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.bits_per_sample = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.stream_info.bits_per_sample));
            }
        }
    }

    static NAN_GETTER(sample_rate) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.stream_info.sample_rate));
        }
    }

    static NAN_SETTER(sample_rate) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.sample_rate = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.stream_info.sample_rate));
            }
        }
    }

    static NAN_GETTER(total_samples) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New<Number>(m->data.stream_info.total_samples));
        }
    }

    static NAN_SETTER(total_samples) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.stream_info.total_samples = getValue(int64_t);
                info.GetReturnValue().Set(Nan::New<Number>(m->data.stream_info.total_samples));
            }
        }
    }

    static NAN_GETTER(md5sum) {
        getPointer(FLAC__StreamMetadata) {
            Local<Array> md5sum = Nan::New<Array>(16);
            for(uint32_t o = 0; o < 16; o++) {
                Nan::Set(md5sum, o, Nan::New(m->data.stream_info.md5sum[o]));
            }
            info.GetReturnValue().Set(md5sum);
        }
    }

    static NAN_SETTER(md5sum) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Object) {
                Local<Array> arr = _newValue.ToLocalChecked().As<Array>();
                for(int i = 0; i < 16; i++) {
                    FLAC__byte v = 0;
                    MaybeLocal<Value> val = Nan::Get(arr, i);
                    if(!val.IsEmpty()) {
                        Maybe<int> vval = Nan::To<int>(val.ToLocalChecked());
                        if(vval.IsJust()) {
                            v = vval.FromJust();
                        }
                    }
                    m->data.stream_info.md5sum[i] = v;
                    Nan::Set(arr, i, Nan::New(v));
                }
                info.GetReturnValue().Set(arr);
            }
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_StreamInfo* i, Local<Object> &obj) {
        SetGetterSetter(min_blocksize);
        SetGetterSetter(max_blocksize);
        SetGetterSetter(min_framesize);
        SetGetterSetter(max_framesize);
        SetGetterSetter(channels);
        SetGetterSetter(bits_per_sample);
        SetGetterSetter(sample_rate);
        SetGetterSetter(total_samples);
        SetGetterSetter(md5sum);
    }

}
