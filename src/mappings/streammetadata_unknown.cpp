#include "mappings.hpp"

namespace flac_bindings {

    using namespace Napi;

    FunctionReference UnknownMetadata::constructor;

    Function UnknownMetadata::init(const Napi::Env& env) {
        EscapableHandleScope scope(env);

        Function constructor = DefineClass(env, "UnknownMetadata", {
            InstanceAccessor("data", &UnknownMetadata::getData, nullptr, napi_property_attributes::napi_enumerable),
        });

        UnknownMetadata::constructor = Persistent(constructor);
        UnknownMetadata::constructor.SuppressDestruct();

        return scope.Escape(constructor).As<Function>();
    }

    UnknownMetadata::UnknownMetadata(const CallbackInfo& info):
        ObjectWrap<UnknownMetadata>(info),
        Metadata(info, FLAC__METADATA_TYPE_UNDEFINED) {}

    Napi::Value UnknownMetadata::getData(const CallbackInfo& info) {
        if(data->data.unknown.data == nullptr) {
            return info.Env().Null();
        }

        return pointer::wrap(info.Env(), data->data.unknown.data, data->length);
    }

}
