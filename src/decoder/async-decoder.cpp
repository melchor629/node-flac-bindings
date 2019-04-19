#include <functional>
#include <memory>

#define MAKE_FRIENDS
#include "decoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/pointer.hpp"
#include "../utils/encoder-decoder-async.hpp"

using namespace v8;
using namespace Nan;
using namespace std::placeholders;

namespace flac_bindings {

    DecoderWorkRequest::DecoderWorkRequest(): SyncronizableWorkRequest() {}

    DecoderWorkRequest::DecoderWorkRequest(DecoderWorkRequest::Type type): DecoderWorkRequest() {
        this->type = type;
    }

    DecoderWorkRequest::DecoderWorkRequest(const DecoderWorkRequest &d): SyncronizableWorkRequest(d) {
        type = d.type;
        buffer = d.buffer;
        bytes = d.bytes;
        offset = d.offset;
        frame = d.frame;
        samples = d.samples;
        metadata = d.metadata;
        errorCode = d.errorCode;
        returnValue = d.returnValue;
    }


    static std::function<void (
        AsyncDecoderWorkBase::ResolveCallback resolve,
        const AsyncDecoderWorkBase::RejectCallbacks &reject,
        const AsyncDecoderWorkBase::ExecutionProgress &progress
    )>
    decorate(StreamDecoder* dec, std::function<bool()> func) {
        return [dec, func] (auto resolve, const auto &reject, const auto &progress) {
            if(dec->progress != nullptr) {
                reject.withMessage("There's already an asynchronous operation on the decoder");
                return;
            }

            dec->progress = &progress;
            dec->reject = &reject;
            bool ok = func();
            if(!ok) reject.withMessage(FLAC__stream_decoder_get_resolved_state_string(dec->dec));
            else resolve(ok);
            dec->progress = nullptr;
            dec->reject = nullptr;
        };
    }


    AsyncDecoderWork::AsyncDecoderWork(
        std::function<bool()> function,
        Callback* callback,
        const char* name,
        StreamDecoder* dec
    ): AsyncBackgroundTask<bool, DecoderWorkRequest>(
        decorate(dec, function),
        std::bind(decoderDoWork, dec, _1, _2, _3),
        callback,
        name,
        booleanToJs<bool>
    ) {}

    AsyncDecoderWorkBase* AsyncDecoderWork::forFinish(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] () { return FLAC__stream_decoder_finish(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::finishAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::finishAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forFlush(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] () { return FLAC__stream_decoder_flush(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::flushAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::flushAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forProcessSingle(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] () { return FLAC__stream_decoder_process_single(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::processSingleAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::processSingleAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forProcessUntilEndOfMetadata(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] () { return FLAC__stream_decoder_process_until_end_of_metadata(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::processUntilEndOfMetadataAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::processUntilEndOfMetadataAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forProcessUntilEndOfStream(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] () { return FLAC__stream_decoder_process_until_end_of_stream(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::processUntilEndOfStreamAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::processUntilEndOfStreamAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forSkipSingleFrame(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] () { return FLAC__stream_decoder_skip_single_frame(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::skipSingleFrameAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::skipSingleFrameAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forSeekAbsolute(uint64_t value, StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec, value] () { return FLAC__stream_decoder_seek_absolute(dec->dec, value); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::seekAbsoluteAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::seekAbsoluteAsync", dec);
    }


    PromisifiedAsyncDecoderWork::PromisifiedAsyncDecoderWork(
        std::function<bool()> function,
        const char* name,
        StreamDecoder* dec
    ): PromisifiedAsyncDecoderWorkBase(
        decorate(dec, function),
        std::bind(decoderDoWork, dec, _1, _2, _3),
        name,
        booleanToJs<bool>
    ) {}



    void decoderDoWork(const StreamDecoder* dec, const AsyncDecoderWorkBase* w, const DecoderWorkRequest *data, size_t size) {
        using namespace v8;
        using namespace Nan;
        using namespace node;

        auto async = (Nan::AsyncResource*) w->getAsyncResource();
        std::function<void (Local<Value> result)> processResult;
        Nan::MaybeLocal<Value> result;
        Nan::TryCatch tryCatch;

        auto functionForReturnNumber = functionGeneratorForReturnNumber(dec, data);
        auto functionForReturnObject = functionGeneratorForReturnObject(dec, data);

        switch(data->type) {
            case DecoderWorkRequest::Type::Eof: {
                result = dec->eofCbk->Call(0, nullptr, async);
                processResult = [data, dec] (auto res) {
                    auto maybeBoolean = booleanFromJs<int>(res);
                    if(maybeBoolean.IsJust()) {
                        *data->returnValue = maybeBoolean.FromJust();
                    } else {
                        dec->reject->withException(Nan::TypeError("Decoder::EofCallback - Expected boolean as return value"));
                    }
                };
                break;
            }

            case DecoderWorkRequest::Type::Error: {
                Local<Value> args[] { numberToJs(data->errorCode) };
                dec->errorCbk->Call(1, args, async);
                break;
            }

            case DecoderWorkRequest::Type::Length: {
                result = dec->lengthCbk->Call(0, nullptr, async);
                processResult = functionForReturnObject("Decoder::LengthCallback", "length", [data] (auto v) { *data->offset = v; });
                break;
            }

            case DecoderWorkRequest::Type::Metadata: {
                Local<Value> elstru = structToJs(data->metadata);
                Local<Value> args[] { elstru };
                dec->metadataCbk->Call(1, args, async);
                break;
            }

            case DecoderWorkRequest::Type::Read: {
                Local<Value> buffer = WrapPointer(data->buffer, *data->bytes).ToLocalChecked();
                Local<Value> args[] { buffer };
                result = dec->readCbk->Call(1, args, async);
                processResult = functionForReturnObject("Decoder::ReadCallback", "bytes", [data] (auto v) { *data->bytes = (size_t) v; });
                break;
            }

            case DecoderWorkRequest::Type::Seek: {
                Local<Value> args[] { numberToJs(*data->offset) };
                result = dec->seekCbk->Call(1, args, async);
                processResult = functionForReturnNumber("Decoder::SeekCallback");
                break;
            }

            case DecoderWorkRequest::Type::Tell: {
                result = dec->tellCbk->Call(0, nullptr, async);
                processResult = functionForReturnObject("Decoder::TellCallback", "offset", [data] (auto v) { *data->offset = v; });
                break;
            }

            case DecoderWorkRequest::Type::Write: {
                Local<Array> buffers = Nan::New<Array>();
                unsigned channels = FLAC__stream_decoder_get_channels(dec->dec);
                for(uint32_t i = 0; i < channels; i++) {
                    auto leb = WrapPointer(data->samples[i], data->frame->header.blocksize * sizeof(int32_t)).ToLocalChecked();
                    Nan::Set(buffers, i, leb);
                }

                Local<Value> args[] { structToJs(data->frame), buffers };
                result = dec->writeCbk->Call(2, args, async);
                processResult = functionForReturnNumber("Decoder::WriteCallback");
                break;
            }
        }

        if(tryCatch.HasCaught()) {
            dec->reject->withException(tryCatch.Exception());
            data->notifyWorkDone();
        } else {
            auto theGoodResult = result.ToLocalChecked();
            if(theGoodResult->IsPromise()) {
                auto promise = theGoodResult.As<Promise>();
                (void) promise;
                //TODO
                //promise->Then(Isolate::GetCurrent()->GetCurrentContext(), Nan::Undefined());
                //promise->Catch(Isolate::GetCurrent()->GetCurrentContext(), Nan::Undefined());
                //defer data->notifyWorkDone();
            } else {
                if(processResult && !result.IsEmpty()) processResult(result.ToLocalChecked());
                data->notifyWorkDone();
            }
        }
    }


}