#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(id) {
        getPointer(FLAC__StreamMetadata) {
            Local<Array> array = Nan::New<Array>();
            for(int i = 0; i < 4; i++) {
                Nan::Set(array, i, Nan::New(m->data.application.id[i]));
            }
            info.GetReturnValue().Set(array);
        }
    }

    static NAN_SETTER(id) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Object) {
                Local<Array> array = _newValue.ToLocalChecked().As<Array>();
                if(array->Length() != 4) {
                    Nan::ThrowError(Nan::Error("Array must have 4 bytes"));
                } else {
                    for(int i = 0; i < 4; i++) {
                        Local<Value> val = Nan::Get(array, i).ToLocalChecked();
                        if(val->IsNumber()) {
                            m->data.application.id[i] = Nan::To<uint32_t>(val).FromJust() & 0xFF;
                        } else {
                            m->data.application.id[i] = 0;
                        }
                    }
                    info.GetReturnValue().Set(array);
                }
            }
        }
    }

    static NAN_GETTER(data) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(WrapPointer(m->data.application.data, m->length - 4).ToLocalChecked());
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_Application* i, Local<Object> &obj) {
        SetGetterSetter(id);
        SetGetter(data);
    }

}
