#include "defs.hpp"

namespace flac_bindings {

    static NAN_GETTER(data) {
        getPointer(FLAC__StreamMetadata) {
            info.GetReturnValue().Set(WrapPointer(m->data.unknown.data, m->length).ToLocalChecked());
        }
    }

    template<>
    void structToJs(const FLAC__StreamMetadata_Unknown* i, Local<Object> &obj) {
        SetGetter(data);
    }

}
