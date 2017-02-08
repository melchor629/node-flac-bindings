#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(points) {
        getPointer(FLAC__StreamMetadata) {
            Local<Array> arr = Nan::New<Array>();
            for(uint32_t i = 0; i < m->data.seek_table.num_points; i++) {
                Nan::Set(arr, i, structToJs(&m->data.seek_table.points[i]));
            }
            info.GetReturnValue().Set(arr);
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_SeekTable* i, Local<Object> &obj) {
        SetGetter(points);
    }

}
