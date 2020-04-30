#include "mappings.hpp"

namespace flac_bindings {

    using namespace Napi;

    FunctionReference PaddingMetadata::constructor;

    Function PaddingMetadata::init(const Napi::Env& env) {
        EscapableHandleScope scope(env);

        Function constructor = DefineClass(env, "PaddingMetadata", {});

        PaddingMetadata::constructor = Persistent(constructor);
        PaddingMetadata::constructor.SuppressDestruct();

        return scope.Escape(constructor).As<Function>();
    }

    PaddingMetadata::PaddingMetadata(const CallbackInfo& info):
        ObjectWrap<PaddingMetadata>(info),
        Metadata(info, FLAC__METADATA_TYPE_PADDING) {
#if NAPI_VERSION > 5
        const bool isNumber = info[0].IsNumber() || info[0].IsBigInt();
#else
        const bool isNumber = info[0].IsNumber();
#endif
        if(info.Length() > 0 && isNumber) {
            data->length += numberFromJs<uint32_t>(info[0]);
        }
    }

}
