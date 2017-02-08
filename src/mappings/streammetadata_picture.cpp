#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(type) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New((uint32_t) m->data.picture.type));
        }
    }

    static NAN_SETTER(type) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.picture.type = (FLAC__StreamMetadata_Picture_Type) getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New((uint32_t) m->data.picture.type));
            }
        }
    }

    static NAN_GETTER(mime_type) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.picture.mime_type).ToLocalChecked());
        }
    }

    static NAN_GETTER(description) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New((char*) m->data.picture.description).ToLocalChecked());
        }
    }

    static NAN_GETTER(width) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.picture.width));
        }
    }

    static NAN_SETTER(width) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.picture.width = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.picture.width));
            }
        }
    }

    static NAN_GETTER(height) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.picture.height));
        }
    }

    static NAN_SETTER(height) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.picture.height = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.picture.height));
            }
        }
    }

    static NAN_GETTER(depth) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.picture.depth));
        }
    }

    static NAN_SETTER(depth) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.picture.depth = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.picture.depth));
            }
        }
    }

    static NAN_GETTER(colors) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.picture.colors));
        }
    }

    static NAN_SETTER(colors) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.picture.colors = getValue(uint32_t);
                info.GetReturnValue().Set(Nan::New(m->data.picture.colors));
            }
        }
    }

    static NAN_GETTER(data) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(WrapPointer(m->data.picture.data, m->data.picture.data_length).ToLocalChecked());
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_Picture* i, Local<Object> &obj) {
        SetGetterSetter(type);
        SetGetter(mime_type);
        SetGetter(description);
        SetGetterSetter(width);
        SetGetterSetter(height);
        SetGetterSetter(depth);
        SetGetterSetter(colors);
        SetGetter(data);
    }

}
