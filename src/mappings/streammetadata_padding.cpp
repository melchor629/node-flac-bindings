#include "../flac_addon.hpp"
#include "mappings.hpp"

namespace flac_bindings {

  using namespace Napi;

  Function PaddingMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    Function constructor = DefineClass(env, "PaddingMetadata", {});

    addon.paddingMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  PaddingMetadata::PaddingMetadata(const CallbackInfo& info):
      ObjectWrap<PaddingMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_PADDING) {
    const bool isNumber = info[0].IsNumber() || info[0].IsBigInt();
    if (info.Length() > 0 && isNumber) {
      data->length += numberFromJs<uint32_t>(info[0]);
    }
  }

}
