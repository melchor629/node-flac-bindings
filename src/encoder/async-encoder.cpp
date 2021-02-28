#include "encoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"

namespace flac_bindings {

    using namespace Napi;
    using namespace std::placeholders;

    EncoderWorkRequest::EncoderWorkRequest() {}

    EncoderWorkRequest::EncoderWorkRequest(EncoderWorkRequest::Type type) {
        this->type = type;
    }

    EncoderWorkRequest::EncoderWorkRequest(const EncoderWorkRequest &d) {
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


    AsyncEncoderWork::FunctionCallback AsyncEncoderWork::decorate(
        EncoderWorkContext* ctx,
        const std::function<int()>& func
    ) {
        return [ctx, func] (ExecutionProgress& c) {
            ctx->enc->asyncExecutionProgress = &c;

            DEFER(ctx->enc->runLocked([&] () {
                ctx->enc->busy = false;
                ctx->enc->asyncExecutionProgress = nullptr;
            }));

            int ok = func();
            if(!c.isCompleted()) {
                c.resolve(ok);
            }
        };
    }

    AsyncEncoderWork::AsyncEncoderWork(
        const StoreList& list,
        std::function<int()> function,
        const char* name,
        EncoderWorkContext* ctx,
        std::function<Napi::Value(const Napi::Env&, int)> convertFunction
    ): AsyncEncoderWorkBase(
        list.begin()->Env(),
        decorate(ctx, function),
        std::bind(&AsyncEncoderWork::onProgress, this, ctx, _1, _2, _3),
        name,
        convertFunction
    ) {
        this->Receiver().Set("this", list.begin()->Env());
        for(auto it = list.begin() + 1; it != list.end(); it += 1) {
            this->Receiver().Set(it - list.begin() - 1, *it);
        }
    }

    AsyncEncoderWork* AsyncEncoderWork::forFinish(const StoreList& list, EncoderWorkContext* ctx) {
        auto workFunction = [ctx] () {
            return FLAC__stream_encoder_finish(ctx->enc->enc);
        };

        return new AsyncEncoderWork(list, workFunction, "flac_bindings::StreamEncoder::finishAsync", ctx);
    }

    AsyncEncoderWork* AsyncEncoderWork::forProcess(const StoreList& list, const std::vector<int32_t*>& buffers, uint64_t samples, EncoderWorkContext* ctx) {
        auto workFunction = [ctx, buffers, samples] () {
            return FLAC__stream_encoder_process(ctx->enc->enc, buffers.data(), samples);
        };

        return new AsyncEncoderWork(list, workFunction, "flac_bindings::StreamEncoder::processAsync", ctx);
    }

    AsyncEncoderWork* AsyncEncoderWork::forProcessInterleaved(const StoreList& list, int32_t* buffer, uint64_t samples, EncoderWorkContext* ctx) {
        auto workFunction = [ctx, buffer, samples] () {
            return FLAC__stream_encoder_process_interleaved(ctx->enc->enc, buffer, samples);
        };

        return new AsyncEncoderWork(list, workFunction, "flac_bindings::StreamEncoder::processInterleavedAsync", ctx);
    }

    AsyncEncoderWork* AsyncEncoderWork::forInitStream(const StoreList& list, EncoderWorkContext* ctx) {
        auto workFunction = [ctx] () {
            return FLAC__stream_encoder_init_stream(
                ctx->enc->enc,
                ctx->writeCbk.IsEmpty() ? nullptr : StreamEncoder::writeCallback,
                ctx->seekCbk.IsEmpty() ? nullptr : StreamEncoder::seekCallback,
                ctx->tellCbk.IsEmpty() ? nullptr : StreamEncoder::tellCallback,
                ctx->metadataCbk.IsEmpty() ? nullptr : StreamEncoder::metadataCallback,
                ctx
            );
        };
        auto convertFunction = [ctx] (const Napi::Env& env, int value) {
            ctx->enc->checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
            return env.Undefined();
        };

        return new AsyncEncoderWork(list, workFunction, "flac_bindings::StreamEncoder::initStreamAsync", ctx, convertFunction);
    }

    AsyncEncoderWork* AsyncEncoderWork::forInitOggStream(const StoreList& list, EncoderWorkContext* ctx) {
        auto workFunction = [ctx] () {
            return FLAC__stream_encoder_init_ogg_stream(
                ctx->enc->enc,
                ctx->readCbk.IsEmpty() ? nullptr : StreamEncoder::readCallback,
                ctx->writeCbk.IsEmpty() ? nullptr : StreamEncoder::writeCallback,
                ctx->seekCbk.IsEmpty() ? nullptr : StreamEncoder::seekCallback,
                ctx->tellCbk.IsEmpty() ? nullptr : StreamEncoder::tellCallback,
                ctx->metadataCbk.IsEmpty() ? nullptr : StreamEncoder::metadataCallback,
                ctx
            );
        };
        auto convertFunction = [ctx] (const Napi::Env& env, int value) {
            ctx->enc->checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
            return env.Undefined();
        };

        return new AsyncEncoderWork(list, workFunction, "flac_bindings::StreamEncoder::initOggStreamAsync", ctx, convertFunction);
    }

    AsyncEncoderWork* AsyncEncoderWork::forInitFile(const StoreList& list, const std::string& path, EncoderWorkContext* ctx) {
        auto workFunction = [ctx, path] () {
            return FLAC__stream_encoder_init_file(
                ctx->enc->enc,
                path.c_str(),
                ctx->progressCbk.IsEmpty() ? nullptr : StreamEncoder::progressCallback,
                ctx
            );
        };
        auto convertFunction = [ctx] (const Napi::Env& env, int value) {
            ctx->enc->checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
            return env.Undefined();
        };

        return new AsyncEncoderWork(list, workFunction, "flac_bindings::StreamEncoder::initFileAsync", ctx, convertFunction);
    }

    AsyncEncoderWork* AsyncEncoderWork::forInitOggFile(const StoreList& list, const std::string& path, EncoderWorkContext* ctx) {
        auto workFunction = [ctx, path] () {
            return FLAC__stream_encoder_init_ogg_file(
                ctx->enc->enc,
                path.c_str(),
                ctx->progressCbk.IsEmpty() ? nullptr : StreamEncoder::progressCallback,
                ctx
            );
        };
        auto convertFunction = [ctx] (const Napi::Env& env, int value) {
            ctx->enc->checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
            return env.Undefined();
        };

        return new AsyncEncoderWork(list, workFunction, "flac_bindings::StreamEncoder::initOggFileAsync", ctx, convertFunction);
    }

    void AsyncEncoderWork::onProgress(
        const EncoderWorkContext* ctx,
        Napi::Env& env,
        ExecutionProgress& prog,
        const std::shared_ptr<EncoderWorkRequest>& req
    ) {
        std::function<void(Napi::Value result)> processResult;
        Napi::Value result;

        switch(req->type) {
            case EncoderWorkRequest::Type::Read: {
                sharedBufferRef.setFromWrap(env, req->buffer, *req->bytes);
                result = ctx->readCbk.MakeCallback(env.Global(), {sharedBufferRef.value()});
                processResult = generateParseObjectResult(req->returnValue, "Encoder:ReadCallback", "bytes", *req->bytes);
                break;
            }

            case EncoderWorkRequest::Type::Write: {
                sharedBufferRef.setFromWrap(env, const_cast<FLAC__byte*>(req->constBuffer), *req->bytes);
                result = ctx->writeCbk.MakeCallback(
                    env.Global(),
                    {sharedBufferRef.value(), numberToJs(env, req->samples), numberToJs(env, req->frame)}
                );
                processResult = generateParseNumberResult(req->returnValue, "Encoder:WriteCallback");
                break;
            }

            case EncoderWorkRequest::Type::Seek: {
                result = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, *req->offset)});
                processResult = generateParseNumberResult(req->returnValue, "Encoder:SeekCallback");
                break;
            }

            case EncoderWorkRequest::Type::Tell: {
                result = ctx->tellCbk.MakeCallback(env.Global(), {});
                processResult = generateParseObjectResult(req->returnValue, "Encoder:TellCallback", "offset", *req->offset);
                break;
            }

            case EncoderWorkRequest::Type::Metadata: {
                auto jsMetadata = Metadata::toJs(env, const_cast<FLAC__StreamMetadata*>(req->metadata));
                result = ctx->metadataCbk.MakeCallback(env.Global(), {jsMetadata});
                break;
            }

            case EncoderWorkRequest::Type::Progress: {
                result = ctx->progressCbk.MakeCallback(env.Global(), {
                    numberToJs(env, req->progress.bytesWritten),
                    numberToJs(env, req->progress.samplesWritten),
                    numberToJs(env, req->progress.framesWritten),
                    numberToJs(env, req->progress.totalFramesEstimate),
                });
                break;
            }
        }

        if(result.IsPromise()) {
            auto promise = result.As<Promise>();
            prog.defer(promise, [processResult] (auto&, auto& info, auto) {
                if(processResult) {
                    processResult(info[0]);
                }
            });
        } else {
            if(processResult) {
                processResult(result);
            }
        }
    }

}
