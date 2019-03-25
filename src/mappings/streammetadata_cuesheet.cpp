#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(media_catalog_number) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->data.cue_sheet.media_catalog_number).ToLocalChecked());
        }
    }

    static NAN_SETTER(media_catalog_number) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(String) {
                Nan::Utf8String str(_newValue.ToLocalChecked());
                if(str.length() >= 129) {
                    Nan::ThrowError(Nan::Error("String is too large, max 128 simple characters (128 bytes)"));
                } else {
                    strcpy(m->data.cue_sheet.media_catalog_number, *str);
                    info.GetReturnValue().Set(_newValue.ToLocalChecked());
                }
            }
        }
    }

    static NAN_GETTER(lead_in) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New<Number>(m->data.cue_sheet.lead_in));
        }
    }

    static NAN_SETTER(lead_in) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Number) {
                m->data.cue_sheet.lead_in = getValue(int64_t);
                info.GetReturnValue().Set(Nan::New<Number>(m->data.cue_sheet.lead_in));
            }
        }
    }

    static NAN_GETTER(is_cd) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New<Boolean>(m->data.cue_sheet.is_cd));
        }
    }

    static NAN_SETTER(is_cd) {
        getPointer(FLAC__StreamMetadata) {
            checkValue(Boolean) {
                m->data.cue_sheet.is_cd = getValue(bool);
                info.GetReturnValue().Set(Nan::New<Boolean>(m->data.cue_sheet.is_cd));
            }
        }
    }

    static NAN_GETTER(tracks) {
        getPointer(FLAC__StreamMetadata) {
            Local<Array> array = Nan::New<Array>();
            for(uint32_t i = 0; i < m->data.cue_sheet.num_tracks; i++) {
                Nan::Set(array, i, structToJs(&m->data.cue_sheet.tracks[i]));
            }
            info.GetReturnValue().Set(array);
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_CueSheet* i, Local<Object> &obj) {
        SetGetterSetter(media_catalog_number);
        SetGetterSetter(lead_in);
        SetGetterSetter(is_cd);
        SetGetter(tracks);
    }

}
