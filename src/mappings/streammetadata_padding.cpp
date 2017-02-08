#include "defs.hpp"

namespace flac_bindings {

    template<>
    void structToJs(const FLAC__StreamMetadata_Padding* i, Local<Object> &obj) {}

}
