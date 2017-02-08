#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(vendor_string) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(
                Nan::New((char*) m->data.vorbis_comment.vendor_string.entry).ToLocalChecked()
            );
        }
    }

    static NAN_GETTER(comments) {
        getPointer(FLAC__StreamMetadata) {
            Local<Array> array = Nan::New<Array>();
            for(uint32_t i = 0; i < m->data.vorbis_comment.num_comments; i++) {
                Nan::Set(array, i,
                    Nan::New((char*) m->data.vorbis_comment.comments[i].entry).ToLocalChecked()
                );
            }
            info.GetReturnValue().Set(array);
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_VorbisComment* i, Local<Object> &obj) {
        SetGetter(vendor_string);
        SetGetter(comments);
    }

}
