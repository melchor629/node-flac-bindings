#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(offset) {
        getPointer(FLAC__StreamMetadata_CueSheet_Index) {
            info.GetReturnValue().Set(Nan::New<Number>(m->offset));
        }
    }

    static NAN_SETTER(offset) {
        getPointer(FLAC__StreamMetadata_CueSheet_Index) {
            checkValue(Number) {
                m->offset = getValue(int64_t);
                info.GetReturnValue().Set(Nan::New<Number>(m->offset));
            }
        }
    }

    static NAN_GETTER(number) {
        getPointer(FLAC__StreamMetadata_CueSheet_Index) {
            info.GetReturnValue().Set(Nan::New(m->number));
        }
    }

    static NAN_SETTER(number) {
        getPointer(FLAC__StreamMetadata_CueSheet_Index) {
            checkValue(Number) {
                m->number = getValue(uint32_t) & 0xFF;
                info.GetReturnValue().Set(Nan::New(m->number));
            }
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_CueSheet_Index* i, Local<Object> &obj) {
        SetGetterSetter(offset);
        SetGetterSetter(number);

        Nan::Set(obj, Nan::New("_ptr").ToLocalChecked(), WrapPointer(i, sizeof(FLAC__StreamMetadata_CueSheet_Index)).ToLocalChecked());
    }

}
