#include <functional>
#include <memory>

#define MAKE_FRIENDS
#include "encoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/pointer.hpp"
#include "../utils/encoder-decoder-async.hpp"

using namespace v8;
using namespace Nan;
using namespace std::placeholders;

namespace flac_bindings {

    EncoderWorkRequest::EncoderWorkRequest(): SyncronizableWorkRequest() {}

    EncoderWorkRequest::EncoderWorkRequest(EncoderWorkRequest::Type type): EncoderWorkRequest() {
        this->type = type;
    }

    EncoderWorkRequest::EncoderWorkRequest(const EncoderWorkRequest &d): SyncronizableWorkRequest(d) {
        type = d.type;
        returnValue = d.returnValue;
        buffer = d.buffer;
        bytes = d.bytes;
        constBuffer = d.constBuffer;
        samples = d.samples;
        frame = d.frame;
        offset = d.offset;
        metadata = d.metadata;
        progress = d.progress;
    }


    static inline Nan::Maybe<std::pair<int32_t**, uint32_t>> convertArgsForProcess(Local<Value> &buffers_, Local<Value> &samples, uint32_t ch) {
        if(!buffers_->IsArray()) {
            Nan::ThrowTypeError("Expected first argument to be an array");
            return Nan::Nothing<std::pair<int32_t**, uint32_t>>();
        }

        auto buffers = buffers_.template As<Array>();
        if(buffers->Length() < ch) {
            std::string err = "The array must have " + std::to_string(ch) + " buffers";
            Nan::ThrowError(err.c_str());
            return Nan::Nothing<std::pair<int32_t**, uint32_t>>();
        }

        auto samples2 = numberFromJs<uint32_t>(samples);
        if(samples2.IsNothing()) {
            Nan::ThrowTypeError("Expected second argument to be number or bigint");
            return Nan::Nothing<std::pair<int32_t**, uint32_t>>();
        }

        int32_t** _buffers = new int32_t*[ch];
        for(uint32_t i = 0; i < ch; i++) {
            Local<Value> buff = Nan::Get(buffers, i).ToLocalChecked();
            _buffers[i] = UnwrapPointer<int32_t>(buff);
            if(_buffers[i] == nullptr) {
                std::string err = "Expected element at " + std::to_string(i) + " to be a Buffer";
                Nan::ThrowTypeError(err.c_str());
                delete[] _buffers;
                return Nan::Nothing<std::pair<int32_t**, uint32_t>>();
            }

            size_t buffLen = node::Buffer::Length(buff);
            if(buffLen < samples2.FromJust() * ch * 4) {
                std::string errorMessage = "Buffer at position " + std::to_string(i) + " has not enough bytes: " +
                    "expected " + std::to_string(samples2.FromJust() * ch * 4) + " bytes (" +
                    std::to_string(samples2.FromJust()) + " [int32_t] samples * " + std::to_string(ch) +
                    " channels * 4 bytes per sample) but got " + std::to_string(node::Buffer::Length(buff)) + " bytes";
                Nan::ThrowError(errorMessage.c_str());
                delete[] _buffers;
                return Nan::Nothing<std::pair<int32_t**, uint32_t>>();
            }
        }

        return Nan::Just(std::make_pair(_buffers, samples2.FromJust()));
    }

    static inline Nan::Maybe<std::pair<const int32_t*, uint32_t>> convertArgsForProcessInterleaved(Local<Value> &_buffer, Local<Value> &_samples, unsigned c) {
        const int32_t* buffer = UnwrapPointer<const int32_t>(_buffer);
        auto samples = numberFromJs<uint32_t>(_samples);

        if(buffer == nullptr) {
            Nan::ThrowTypeError("Expected first argument to be a Buffer");
            return Nan::Nothing<std::pair<const int32_t*, uint32_t>>();
        } else if(samples.IsNothing()) {
            if(!_samples->IsUndefined() && !_samples->IsNull()) {
                Nan::ThrowTypeError("Expected second argument to be a number");
                return Nan::Nothing<std::pair<const int32_t*, uint32_t>>();
            } else {
                samples = Nan::Just<uint32_t>(node::Buffer::Length(_buffer) / c / 4);
            }
        }

        if(node::Buffer::Length(_buffer) < samples.FromJust() * c * 4) {
            std::string errorMessage = "Buffer has not enough bytes: expected " + std::to_string(samples.FromJust() * c * 4) +
                " bytes (" + std::to_string(samples.FromJust()) + " [int32_t] samples * " + std::to_string(c) +
                " channels * 4 bytes per sample) but got " + std::to_string(node::Buffer::Length(_buffer)) + " bytes";
            Nan::ThrowError(errorMessage.c_str());
            return Nan::Nothing<std::pair<const int32_t*, uint32_t>>();
        }

        return Nan::Just(std::make_pair(buffer, samples.FromJust()));
    }

    static bool captureInitErrorAndThrow(StreamEncoder* enc, AsyncEncoderWorkBase::ExecutionContext &c, int r) {
        if(r == 0) {
            return true;
        }

        const char* errorMessage = "Unknwon encoder error";
        if(r == 1) { //ENCODER_ERROR
            errorMessage = FLAC__stream_encoder_get_resolved_state_string(enc->enc);
        } else if(r == 2) { //UNSUPPORTED_CONTAINER
            errorMessage = "The library was not compiled with support for the given container format";
        } else if(r == 3) { //INVALID_CALLBACKS
            errorMessage = "A required callback was not supplied";
        } else if(r == 4) { //INVALID_NUMBER_OF_CHANNELS
            errorMessage = "Invalid number of channels";
        } else if(r == 5) { //INVALID_BITS_PER_SAMPLE
            errorMessage = "Invalid bits per sample (the reference encoder only supports up to 24 bit)";
        } else if(r == 6) { //INVALID_SAMPLE_RATE
            errorMessage = "Invalid sample rate";
        } else if(r == 7) { //INVALID_BLOCK_SIZE
            errorMessage = "Invalid block size";
        } else if(r == 8) { //INVALID_MAX_LPC_ORDER
            errorMessage = "Invalid maximum LPC order";
        } else if(r == 9) { //INVALID_QLP_COEFF_PRECISION
            errorMessage = "Invalid precision of the quantized linear predictor coefficient";
        } else if(r == 10) { //BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER
            errorMessage = "The specified block size is less than the maximum LPC order";
        } else if(r == 11) { //NOT_STREAMABLE
            errorMessage = "The encoder is bound to the Subset but other settings violate it";
        } else if(r == 12) { //INVALID_METADATA
            errorMessage = "Any of the metadata blocks given are invalid or there are duplicated metadata blocks";
        } else if(r == 13) { //ALREADY_INITIALIZED
            errorMessage = "init*() method called when the encoder has already been initialized";
        }

        c.reject(errorMessage);
        return true;
    }


    AsyncEncoderWork::AsyncEncoderWork(
        std::function<bool(AsyncEncoderWorkBase::ExecutionContext &)> function,
        const char* name,
        StreamEncoder* enc,
        Callback* callback
    ): AsyncEncoderWorkBase(
        decorate(enc, function, [enc] () { return FLAC__stream_encoder_get_resolved_state_string(enc->enc); }),
        std::bind(encoderDoWork, enc, _1, _2),
        name,
        booleanToJs<bool>,
        callback
    ) {}

    AsyncEncoderWorkBase* AsyncEncoderWork::forFinish(StreamEncoder* enc, Callback* cbk) {
        auto workFunction = [enc] (AsyncEncoderWorkBase::ExecutionContext &c) { return FLAC__stream_encoder_finish(enc->enc); };
        return newWorker<AsyncEncoderWorkBase>(workFunction, "flac_bindings::encoder::finishAsync", enc, cbk);
    }

    AsyncEncoderWorkBase* AsyncEncoderWork::forProcess(Local<Value> &buffers_, Local<Value> &samples, StreamEncoder* enc, Callback* cbk) {
        assertThrowing2(FLAC__stream_encoder_get_state(enc->enc) == 0, "The encoder must be in OK state", nullptr);
        auto result = convertArgsForProcess(buffers_, samples, FLAC__stream_encoder_get_channels(enc->enc));
        if(result.IsNothing()) {
            return nullptr;
        }

        auto _buffers = std::get<0>(result.FromJust());
        auto samples2 = std::get<1>(result.FromJust());
        auto workFunction = [enc, _buffers, samples2] (AsyncEncoderWorkBase::ExecutionContext &c) {
            bool ret = FLAC__stream_encoder_process(enc->enc, _buffers, samples2);
            delete[] _buffers;
            return ret;
        };
        auto work = newWorker<AsyncEncoderWorkBase>(workFunction, "flac_bindings::encoder::processAsync", enc, cbk);
        work->SaveToPersistent("buffers", buffers_);
        work->SaveToPersistent("samples", samples);
        return work;
    }

    AsyncEncoderWorkBase* AsyncEncoderWork::forProcessInterleaved(Local<Value> &_buffer, Local<Value> &_samples, StreamEncoder* enc, Callback* cbk) {
        assertThrowing2(FLAC__stream_encoder_get_state(enc->enc) == 0, "The encoder must be in OK state", nullptr);
        auto result = convertArgsForProcessInterleaved(_buffer, _samples, FLAC__stream_encoder_get_channels(enc->enc));
        if(result.IsNothing()) {
            return nullptr;
        }

        auto buffer = std::get<0>(result.FromJust());
        auto samples = std::get<1>(result.FromJust());
        auto workFunction = [enc, buffer, samples] (AsyncEncoderWorkBase::ExecutionContext &c) { return FLAC__stream_encoder_process_interleaved(enc->enc, buffer, samples); };
        auto work = newWorker<AsyncEncoderWorkBase>(workFunction, "flac_bindings::encoder::processInterleavedAsync", enc, cbk);
        work->SaveToPersistent("buffer", _buffer);
        work->SaveToPersistent("samples", _samples);
        return work;
    }

    AsyncEncoderWorkBase* AsyncEncoderWork::forInitStream(StreamEncoder* enc, Nan::Callback* cbk) {
        auto workFunction = [enc] (AsyncEncoderWorkBase::ExecutionContext &c) {
            return captureInitErrorAndThrow(enc, c, FLAC__stream_encoder_init_stream(
                enc->enc,
                !enc->writeCbk ? nullptr : encoder_write_callback,
                !enc->seekCbk ? nullptr : encoder_seek_callback,
                !enc->tellCbk ? nullptr : encoder_tell_callback,
                !enc->metadataCbk ? nullptr : encoder_metadata_callback,
                enc
            ));
        };

        return newWorker<AsyncEncoderWorkBase>(workFunction, "flac_bindings::encoder::initStreamAsync", enc, cbk);
    }

    AsyncEncoderWorkBase* AsyncEncoderWork::forInitOggStream(StreamEncoder* enc, Nan::Callback* cbk) {
        auto workFunction = [enc] (AsyncEncoderWorkBase::ExecutionContext &c) {
            return captureInitErrorAndThrow(enc, c, FLAC__stream_encoder_init_ogg_stream(
                enc->enc,
                !enc->readCbk ? nullptr : encoder_read_callback,
                !enc->writeCbk ? nullptr : encoder_write_callback,
                !enc->seekCbk ? nullptr : encoder_seek_callback,
                !enc->tellCbk ? nullptr : encoder_tell_callback,
                !enc->metadataCbk ? nullptr : encoder_metadata_callback,
                enc
            ));
        };

        return newWorker<AsyncEncoderWorkBase>(workFunction, "flac_bindings::encoder::initOggStreamAsync", enc, cbk);
    }

    AsyncEncoderWorkBase* AsyncEncoderWork::forInitFile(Local<Value> &path, StreamEncoder* enc, Nan::Callback* cbk) {
        if(!path->IsString()) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return nullptr;
        }

        Nan::Utf8String str(path);
        std::string string(*str);
        auto workFunction = [enc, string] (AsyncEncoderWorkBase::ExecutionContext &c) {
            return captureInitErrorAndThrow(enc, c, FLAC__stream_encoder_init_file(
                enc->enc,
                string.c_str(),
                enc->progressCbk ? encoder_progress_callback : nullptr,
                enc
            ));
        };

        return newWorker<AsyncEncoderWorkBase>(workFunction, "flac_bindings::encoder::initFileAsync", enc, cbk);
    }

    AsyncEncoderWorkBase* AsyncEncoderWork::forInitOggFile(Local<Value> &path, StreamEncoder* enc, Nan::Callback* cbk) {
        if(!path->IsString()) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return nullptr;
        }

        Nan::Utf8String str(path);
        std::string string(*str);
        auto workFunction = [enc, string] (AsyncEncoderWorkBase::ExecutionContext &c) {
            return captureInitErrorAndThrow(enc, c, FLAC__stream_encoder_init_ogg_file(
                enc->enc,
                string.c_str(),
                enc->progressCbk ? encoder_progress_callback : nullptr,
                enc
            ));
        };

        return newWorker<AsyncEncoderWorkBase>(workFunction, "flac_bindings::encoder::initOggFileAsync", enc, cbk);
    }



    PromisifiedAsyncEncoderWork::PromisifiedAsyncEncoderWork(
        std::function<bool(AsyncEncoderWorkBase::ExecutionContext &)> function,
        const char* name,
        StreamEncoder* enc
    ): PromisifiedAsyncEncoderWorkBase(
        decorate(enc, function, [enc] () { return FLAC__stream_encoder_get_resolved_state_string(enc->enc); }),
        std::bind(encoderDoWork, enc, _1, _2),
        name,
        booleanToJs<bool>
    ) {}



    static void encoderDoWork(const StreamEncoder* enc, AsyncEncoderWorkBase::ExecutionContext &c, const EncoderWorkRequest *data) {
        using namespace v8;
        using namespace Nan;
        using namespace node;

        auto async = (Nan::AsyncResource*) c.getTask()->getAsyncResource();
        std::function<void (Local<Value> result)> processResult;
        Nan::MaybeLocal<Value> result;
        Nan::TryCatch tryCatch;

        auto functionForReturnNumber = functionGeneratorForReturnNumber(enc, data);
        auto functionForReturnObject = functionGeneratorForReturnObject(enc, data);

        switch(data->type) {
            case EncoderWorkRequest::Type::Read: {
                Local<Value> args[] { WrapPointer(data->buffer, *data->bytes).ToLocalChecked() };
                result = enc->readCbk->Call(1, args, async);
                processResult = functionForReturnObject("Encoder::ReadCallback", "bytes", [data] (auto v) { *data->bytes = v; });
                break;
            }

            case EncoderWorkRequest::Type::Write: {
                Local<Value> args[] {
                    WrapPointer(data->constBuffer, *data->bytes).ToLocalChecked(),
                    numberToJs(data->samples),
                    numberToJs(data->frame)
                };
                result = enc->writeCbk->Call(3, args, async);
                processResult = functionForReturnNumber("Encoder::WriteCallback");
                break;
            }

            case EncoderWorkRequest::Type::Seek: {
                Local<Value> args[] { numberToJs(*data->offset) };
                result = enc->seekCbk->Call(1, args, async);
                processResult = functionForReturnNumber("Encoder::SeekCallback");
                break;
            }

            case EncoderWorkRequest::Type::Tell: {
                result = enc->tellCbk->Call(0, nullptr, async);
                processResult = functionForReturnObject("Encoder::TellCallback", "offset", [data] (auto v) { *data->offset = v; });
                break;
            }

            case EncoderWorkRequest::Type::Metadata: {
                Local<Value> args[] { structToJs(data->metadata) };
                enc->metadataCbk->Call(1, args, async);
                break;
            }

            case EncoderWorkRequest::Type::Progress: {
                Local<Value> args[] {
                    numberToJs(data->progress.bytesWritten),
                    numberToJs(data->progress.samplesWritten),
                    numberToJs(data->progress.framesWritten),
                    numberToJs(data->progress.totalFramesEstimate)
                };
                enc->progressCbk->Call(4, args, async);
                break;
            }
        }

        if(tryCatch.HasCaught()) {
            c.reject(tryCatch.Exception());
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