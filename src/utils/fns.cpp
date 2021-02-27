#include "converters.hpp"
#include "js_utils.hpp"
#include "pointer.hpp"

namespace flac_bindings {

    static Napi::Value zipAudio(const Napi::CallbackInfo& info) {
        using namespace Napi;
        EscapableHandleScope scope(info.Env());

        if (!info[0].IsObject()) {
            throw TypeError::New(info.Env(), "Expected first argument to be object");
        }

        auto obj = info[0].As<Object>();
        auto samples = numberFromJs<uint64_t>(obj.Get("samples"));
        auto inBps = maybeNumberFromJs<uint64_t>(obj.Get("inBps")).value_or(4);
        auto outBps = maybeNumberFromJs<uint64_t>(obj.Get("outBps")).value_or(4);
        auto buffers = arrayFromJs<char*>(obj.Get("buffers"), [&samples, &inBps] (auto val) {
            if(!val.IsBuffer()) {
                throw TypeError::New(val.Env(), "Expected value in buffers to be a Buffer");
            }

            auto buffer = val.template As<Buffer<char>>();
            if(buffer.ByteLength() < samples * inBps) {
                throw RangeError::New(val.Env(), "Buffer has size "s + std::to_string(buffer.ByteLength()) + " but expected to be at least "s + std::to_string(samples * inBps));
            }

            return buffer.Data();
        });
        auto channels = buffers.size();

        // NOTE: if outBps > inBps means that after one sample copy, there is some bytes that need
        //       to be cleared up. To be faster, all memory is cleared when allocated.
        uint64_t outputBufferSize = samples * outBps * channels;
        char* outputBuffer = (char*) (outBps <= inBps ? malloc(outputBufferSize) : calloc(outputBufferSize, 1));
        for(uint64_t sample = 0; sample < samples; sample++) {
            for(uint64_t channel = 0; channel < channels; channel++) {
                char* out = outputBuffer + (sample * channels + channel) * outBps;
                char* in = buffers[channel] + sample * inBps;
                memcpy(out, in, outBps);
            }
        }

        return scope.Escape(
            Buffer<char>::New(
                info.Env(),
                outputBuffer,
                outputBufferSize,
                [] (auto, auto data) { free(data); }
            )
        );
    }

    static Napi::Value convertSampleFormat(const Napi::CallbackInfo& info) {
        using namespace Napi;
        EscapableHandleScope scope(info.Env());

        if (!info[0].IsObject()) {
            throw TypeError::New(info.Env(), "Expected first argument to be object");
        }

        auto obj = info[0].As<Object>();
        auto inBps = maybeNumberFromJs<uint64_t>(obj.Get("inBps")).value_or(4);
        auto outBps = maybeNumberFromJs<uint64_t>(obj.Get("outBps")).value_or(4);

        if (inBps == outBps) {
            return scope.Escape(obj.Get("buffer"));
        }

        auto bufferPair = pointer::fromBuffer<char>(obj.Get("buffer"));
        auto buffer = std::get<0>(bufferPair);
        uint64_t samples = maybeNumberFromJs<uint64_t>(obj.Get("samples"))
            .value_or(std::get<1>(bufferPair) / inBps);

        if(std::get<1>(bufferPair) < samples * inBps) {
            throw RangeError::New(info.Env(), "Buffer has size "s + std::to_string(std::get<1>(bufferPair)) + " but expected to be at least "s + std::to_string(samples * inBps));
        }

        // NOTE: see above function note about the outputBuffer
        uint64_t outputBufferSize = samples * outBps;
        char* outputBuffer = (char*) (outBps <= inBps ? malloc(outputBufferSize) : calloc(outputBufferSize, 1));
        for(uint64_t sample = 0; sample < samples; sample++) {
            char* out = outputBuffer + sample * outBps;
            char* in = buffer + sample * inBps;
            memcpy(out, in, outBps);
        }

        return scope.Escape(
            Buffer<char>::New(
                info.Env(),
                outputBuffer,
                outputBufferSize,
                [] (auto, auto data) { free(data); }
            )
        );
    }

    Napi::Object initFns(Napi::Env env) {
        using namespace Napi;
        EscapableHandleScope scope(env);

        auto obj = Object::New(env);
        obj.DefineProperties({
            PropertyDescriptor::Function(env, obj, "zipAudio", zipAudio, napi_enumerable),
            PropertyDescriptor::Function(env, obj, "convertSampleFormat", convertSampleFormat, napi_enumerable),
        });

        return scope.Escape(objectFreeze(obj)).As<Object>();
    }

}