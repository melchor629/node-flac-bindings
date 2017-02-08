#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(type) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->type));
        }
    }

    static NAN_GETTER(isLast) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New<Boolean>(m->is_last));
        }
    }

    static NAN_GETTER(length) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(Nan::New(m->length));
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata* i, Local<Object> &obj) {
        switch(i->type) {
            case FLAC__METADATA_TYPE_STREAMINFO: obj = structToJs(&i->data.stream_info); break;
            case FLAC__METADATA_TYPE_PADDING: obj = structToJs(&i->data.padding); break;
            case FLAC__METADATA_TYPE_APPLICATION: obj = structToJs(&i->data.application); break;
            case FLAC__METADATA_TYPE_SEEKTABLE: obj = structToJs(&i->data.seek_table); break;
            case FLAC__METADATA_TYPE_VORBIS_COMMENT: obj = structToJs(&i->data.vorbis_comment); break;
            case FLAC__METADATA_TYPE_CUESHEET: obj = structToJs(&i->data.cue_sheet); break;
            case FLAC__METADATA_TYPE_PICTURE: obj = structToJs(&i->data.picture); break;
            case FLAC__METADATA_TYPE_UNDEFINED: obj = structToJs(&i->data.unknown); break;
            default: obj = Nan::New<Object>();
        }

        SetGetter(type);
        SetGetter(isLast);
        SetGetter(length);
        Nan::Set(obj, Nan::New("_ptr").ToLocalChecked(), WrapPointer(i, sizeof(FLAC__StreamMetadata)).ToLocalChecked());
    }

}
