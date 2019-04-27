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
        AsyncDecoderWorkBase::ExecutionContext &c
    )>
    decorate(AsyncDecoderWorkBase* w, StreamDecoder* dec, std::function<bool(AsyncDecoderWorkBase::ExecutionContext &)> func) {
        return [w, dec, func] (auto &c) {
            if(dec->async != nullptr) {
                c.reject("There's already an asynchronous operation on the decoder");
                return;
            }

            dec->async = w->getAsyncResource();
            dec->asyncExecutionContext = &c;
            bool ok = func(c);
            if(!c.isCompleted()) {
                if(!ok) c.reject(FLAC__stream_decoder_get_resolved_state_string(dec->dec));
                else c.resolve(ok);
            }
            dec->async = nullptr;
            dec->asyncExecutionContext = nullptr;
        };
    }

    static bool captureInitErrorAndThrow(StreamDecoder* dec, AsyncDecoderWorkBase::ExecutionContext &c, int r) {
        if(r == 0) { //OK
            return true;
        }

        const char* errorMessage = "Unknwon decoder error";
        if(r == 1) { //UNSUPPORTED_CONTAINER
            errorMessage = "The library was not compiled with support for the given container format";
        } else if(r == 2) { //INVALID_CALLBACKS
            errorMessage = "A required callback was not supplied";
        } else if(r == 3) { //MEMORY_ALLOCATION_ERROR
            errorMessage = "An error occurred allocating memory";
        } else if(r == 4) { //ERROR_OPENING_FILE
            errorMessage = "Could not open the file";
        } else if(r == 5) { //ALREADY_INITIALIZED
            errorMessage = "init*() method called when the decoder has already been initialized";
        }

        c.reject(errorMessage);
        return true;
    }


    AsyncDecoderWork::AsyncDecoderWork(
        std::function<bool(AsyncDecoderWorkBase::ExecutionContext &)> function,
        Callback* callback,
        const char* name,
        StreamDecoder* dec
    ): AsyncBackgroundTask<bool, DecoderWorkRequest>(
        decorate(this, dec, function),
        std::bind(decoderDoWork, dec, _1, _2),
        callback,
        name,
        booleanToJs<bool>
    ) {}

    AsyncDecoderWorkBase* AsyncDecoderWork::forFinish(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] (auto &c) { return FLAC__stream_decoder_finish(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::finishAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::finishAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forFlush(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] (auto &c) { return FLAC__stream_decoder_flush(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::flushAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::flushAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forProcessSingle(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] (auto &c) { return FLAC__stream_decoder_process_single(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::processSingleAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::processSingleAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forProcessUntilEndOfMetadata(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] (auto &c) { return FLAC__stream_decoder_process_until_end_of_metadata(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::processUntilEndOfMetadataAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::processUntilEndOfMetadataAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forProcessUntilEndOfStream(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] (auto &c) { return FLAC__stream_decoder_process_until_end_of_stream(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::processUntilEndOfStreamAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::processUntilEndOfStreamAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forSkipSingleFrame(StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec] (auto &c) { return FLAC__stream_decoder_skip_single_frame(dec->dec); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::skipSingleFrameAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::skipSingleFrameAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forSeekAbsolute(uint64_t value, StreamDecoder* dec, Callback* cbk) {
        auto workFunction = [dec, value] (auto &c) { return FLAC__stream_decoder_seek_absolute(dec->dec, value); };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::seekAbsoluteAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::seekAbsoluteAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forInitStream(StreamDecoder* dec, Nan::Callback* cbk) {
        auto workFunction = [dec] (auto &c) {
            return captureInitErrorAndThrow(dec, c, FLAC__stream_decoder_init_stream(
                dec->dec,
                !dec->readCbk ? nullptr : decoder_read_callback,
                !dec->seekCbk ? nullptr : decoder_seek_callback,
                !dec->tellCbk ? nullptr : decoder_tell_callback,
                !dec->lengthCbk ? nullptr : decoder_length_callback,
                !dec->eofCbk ? nullptr : decoder_eof_callback,
                !dec->writeCbk ? nullptr : decoder_write_callback,
                !dec->metadataCbk ? nullptr : decoder_metadata_callback,
                !dec->errorCbk ? nullptr : decoder_error_callback,
                dec
            ));
        };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::initStreamAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::initStreamAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forInitOggStream(StreamDecoder* dec, Nan::Callback* cbk) {
        auto workFunction = [dec] (auto &c) {
            return captureInitErrorAndThrow(dec, c, FLAC__stream_decoder_init_ogg_stream(
                dec->dec,
                !dec->readCbk ? nullptr : decoder_read_callback,
                !dec->seekCbk ? nullptr : decoder_seek_callback,
                !dec->tellCbk ? nullptr : decoder_tell_callback,
                !dec->lengthCbk ? nullptr : decoder_length_callback,
                !dec->eofCbk ? nullptr : decoder_eof_callback,
                !dec->writeCbk ? nullptr : decoder_write_callback,
                !dec->metadataCbk ? nullptr : decoder_metadata_callback,
                !dec->errorCbk ? nullptr : decoder_error_callback,
                dec
            ));
        };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::initOggStreamAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::initOggStreamAsync", dec);
    }


    AsyncDecoderWorkBase* AsyncDecoderWork::forInitFile(const std::string &filePath, StreamDecoder* dec, Nan::Callback* cbk) {
        auto workFunction = [dec, filePath] (auto &c) {
            return captureInitErrorAndThrow(dec, c, FLAC__stream_decoder_init_file(
                dec->dec,
                filePath.c_str(),
                dec->writeCbk ? decoder_write_callback : nullptr,
                dec->metadataCbk ? decoder_metadata_callback : nullptr,
                dec->errorCbk ? decoder_error_callback : nullptr,
                dec
            ));
        };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::initFileAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::initFileAsync", dec);
    }

    AsyncDecoderWorkBase* AsyncDecoderWork::forInitOggFile(const std::string &filePath, StreamDecoder* dec, Nan::Callback* cbk) {
        auto workFunction = [dec, filePath] (auto &c) {
            return captureInitErrorAndThrow(dec, c, FLAC__stream_decoder_init_ogg_file(
                dec->dec,
                filePath.c_str(),
                dec->writeCbk ? decoder_write_callback : nullptr,
                dec->metadataCbk ? decoder_metadata_callback : nullptr,
                dec->errorCbk ? decoder_error_callback : nullptr,
                dec
            ));
        };
        if(cbk) return new AsyncDecoderWork(workFunction, cbk, "flac_bindings::decoder::initOggFileAsync", dec);
        else return new PromisifiedAsyncDecoderWork(workFunction, "flac_bindings::decoder::initOggFileAsync", dec);
    }



    PromisifiedAsyncDecoderWork::PromisifiedAsyncDecoderWork(
        std::function<bool(AsyncDecoderWorkBase::ExecutionContext &)> function,
        const char* name,
        StreamDecoder* dec
    ): PromisifiedAsyncDecoderWorkBase(
        decorate(this, dec, function),
        std::bind(decoderDoWork, dec, _1, _2),
        name,
        booleanToJs<bool>
    ) {}



    void decoderDoWork(const StreamDecoder* dec, AsyncDecoderWorkBase::ExecutionContext &w, const DecoderWorkRequest *data) {
        using namespace v8;
        using namespace Nan;
        using namespace node;

        auto async = (Nan::AsyncResource*) w.getTask()->getAsyncResource();
        std::function<void (Local<Value> result)> processResult;
        Nan::MaybeLocal<Value> result;
        Nan::TryCatch tryCatch;

        auto functionForReturnNumber = functionGeneratorForReturnNumber(dec, data);
        auto functionForReturnObject = functionGeneratorForReturnObject(dec, data);

        switch(data->type) {
            case DecoderWorkRequest::Type::Eof: {
                result = dec->eofCbk->Call(0, nullptr, async);
                processResult = [data, &w] (auto res) {
                    auto maybeBoolean = booleanFromJs<int>(res);
                    if(maybeBoolean.IsJust()) {
                        *data->returnValue = maybeBoolean.FromJust();
                    } else {
                        w.reject(Nan::TypeError("Decoder::EofCallback - Expected boolean as return value"));
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
            w.reject(tryCatch.Exception());
            data->notifyWorkDone();
        } else if(!result.IsEmpty()) {
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
        } else {
            data->notifyWorkDone();
        }
    }


}