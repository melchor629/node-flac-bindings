#include <napi.h>
#include "decoder/decoder.hpp"
#include "encoder/encoder.hpp"
#include "mappings/mappings.hpp"
#include "mappings/native_iterator.hpp"

namespace flac_bindings {

    using namespace Napi;

    extern Promise testAsync(const CallbackInfo& info);
    extern Object initFormat(const Env& env);
    extern Object initMetadata0(const Env& env);
    extern Function initMetadata1(const Env& env);
    extern void initMetadata2(const Env& env, Object& exports);

    ObjectReference module;

    static Object initMetadata(const Env& env) {
        EscapableHandleScope scope(env);
        auto metadata = Object::New(env);
        metadata["StreamInfoMetadata"] = StreamInfoMetadata::init(env);
        metadata["PaddingMetadata"] = PaddingMetadata::init(env);
        metadata["ApplicationMetadata"] = ApplicationMetadata::init(env);
        metadata["SeekTableMetadata"] = SeekTableMetadata::init(env);
        metadata["SeekPoint"] = SeekPoint::init(env);
        metadata["VorbisCommentMetadata"] = VorbisCommentMetadata::init(env);
        metadata["CueSheetMetadata"] = CueSheetMetadata::init(env);
        metadata["CueSheetIndex"] = CueSheetIndex::init(env);
        metadata["CueSheetTrack"] = CueSheetTrack::init(env);
        metadata["PictureMetadata"] = PictureMetadata::init(env);
        metadata["UnknownMetadata"] = UnknownMetadata::init(env);
        return scope.Escape(objectFreeze(metadata)).As<Object>();
    }

    static void fillExports(const Env& env, Object exports) {
        HandleScope scope(env);
        NativeIterator::init(env);

        exports["napiVersion"] = Number::New(env, NAPI_VERSION);
        exports["testAsync"] = Function::New(env, testAsync);
        exports["Encoder"] = StreamEncoder::init(env);
        exports["Decoder"] = StreamDecoder::init(env);
        exports["format"] = initFormat(env);
        exports["metadata"] = initMetadata(env);
        exports["metadata0"] = initMetadata0(env);
        exports["SimpleIterator"] = initMetadata1(env);
        initMetadata2(env, exports);

        objectFreeze(exports);
    }

    Object init(Env env, Object exports) {
        module = Persistent(exports);
        module.SuppressDestruct();

        exports["_helpers"] = Object::New(env);

        fillExports(env, exports);

        return exports;
    }

}

static napi_value napi_init_module(napi_env env, napi_value exports) {
    return Napi::RegisterModule(env, exports, flac_bindings::init);
}

NAPI_MODULE(flac, napi_init_module)
