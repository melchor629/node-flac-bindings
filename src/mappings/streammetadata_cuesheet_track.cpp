#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(offset) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            info.GetReturnValue().Set(Nan::New<Number>(m->offset));
        }
    }

    static NAN_SETTER(offset) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            checkValue(Number) {
                m->offset = getValue(int64_t);
                info.GetReturnValue().Set(Nan::New<Number>(m->offset));
            }
        }
    }

    static NAN_GETTER(number) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            info.GetReturnValue().Set(Nan::New(m->number));
        }
    }

    static NAN_SETTER(number) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            checkValue(Number) {
                m->number = getValue(uint32_t) & 0xFF;
                info.GetReturnValue().Set(Nan::New(m->number));
            }
        }
    }

    static NAN_GETTER(isrc) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            info.GetReturnValue().Set(Nan::New(m->isrc).ToLocalChecked());
        }
    }

    static NAN_SETTER(isrc) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            checkValue(String) {
                Local<String> isrc = _newValue.ToLocalChecked().As<String>();
                if(isrc->Utf8Length() == 12) {
                    isrc->WriteUtf8(m->isrc);
                    info.GetReturnValue().Set(Nan::New(m->isrc).ToLocalChecked());
                } else {
                    Nan::ThrowError(Nan::Error("ISRC string must have 12 alphanumeric characters"));
                }
            }
        }
    }

    static NAN_GETTER(type) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            info.GetReturnValue().Set(Nan::New(m->type));
        }
    }

    static NAN_SETTER(type) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            checkValue(Number) {
                m->type = getValue(uint32_t) & 1;
                info.GetReturnValue().Set(Nan::New<Number>(m->type));
            }
        }
    }

    static NAN_GETTER(pre_emphasis) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            info.GetReturnValue().Set(Nan::New<Boolean>(m->pre_emphasis));
        }
    }

    static NAN_SETTER(pre_emphasis) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            checkValue(Boolean) {
                m->pre_emphasis = getValue(bool);
                info.GetReturnValue().Set(Nan::New<Boolean>(m->pre_emphasis));
            }
        }
    }

    static NAN_GETTER(indices) {
        getPointer(FLAC__StreamMetadata_CueSheet_Track) {
            Local<Array> array = Nan::New<Array>();
            for(int i = 0; i < m->num_indices; i++) {
                Nan::Set(array, i, structToJs(&m->indices[i]));
            }
            info.GetReturnValue().Set(array);
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_CueSheet_Track* i, Local<Object> &obj) {
        SetGetterSetter(offset);
        SetGetterSetter(number);
        SetGetterSetter(isrc);
        SetGetterSetter(type);
        SetGetterSetter(pre_emphasis);
        SetGetter(indices);

        Nan::Set(obj, Nan::New("_ptr").ToLocalChecked(), WrapPointer(i, sizeof(FLAC__StreamMetadata_CueSheet_Track)).ToLocalChecked());
    }

}
