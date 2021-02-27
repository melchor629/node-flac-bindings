#include "decoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"

namespace flac_bindings {

    using namespace Napi;
    using namespace std::placeholders;

    DecoderWorkRequest::DecoderWorkRequest() {}

    DecoderWorkRequest::DecoderWorkRequest(DecoderWorkRequest::Type type): DecoderWorkRequest() {
        this->type = type;
    }

    DecoderWorkRequest::DecoderWorkRequest(const DecoderWorkRequest &d) {
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


    AsyncDecoderWork::FunctionCallback AsyncDecoderWork::decorate(
        DecoderWorkContext* ctx,
        const std::function<int()>& func
    ) {
        return [ctx, func] (AsyncDecoderWork::ExecutionProgress& c) {
            ctx->dec->asyncExecutionProgress = &c;

            DEFER(ctx->dec->runLocked([&] () {
                ctx->dec->busy = false;
                ctx->dec->asyncExecutionProgress = nullptr;
            }));

            int ok = func();
            if(!c.isCompleted()) {
                c.resolve(ok);
            }
        };
    }

    AsyncDecoderWork::AsyncDecoderWork(
        const Object& self,
        std::function<int()> function,
        const char* name,
        DecoderWorkContext* ctx,
        std::function<Napi::Value(const Napi::Env&, int)> convertFunction
    ): AsyncDecoderWorkBase(
        self.Env(),
        decorate(ctx, function),
        std::bind(&AsyncDecoderWork::onProgress, this, ctx, _1, _2, _3),
        name,
        convertFunction
    ) {
        this->Receiver().Set("this", self);
    }

    AsyncDecoderWork* AsyncDecoderWork::forFinish(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () { return FLAC__stream_decoder_finish(ctx->dec->dec); };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::finishAsync", ctx);
    }

    AsyncDecoderWork* AsyncDecoderWork::forFlush(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () { return FLAC__stream_decoder_flush(ctx->dec->dec); };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::flushAsync", ctx);
    }

    AsyncDecoderWork* AsyncDecoderWork::forProcessSingle(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () { return FLAC__stream_decoder_process_single(ctx->dec->dec); };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::processSingleAsync", ctx);
    }

    AsyncDecoderWork* AsyncDecoderWork::forProcessUntilEndOfMetadata(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () { return FLAC__stream_decoder_process_until_end_of_metadata(ctx->dec->dec); };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::processUntilEndOfMetadataAsync", ctx);
    }

    AsyncDecoderWork* AsyncDecoderWork::forProcessUntilEndOfStream(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () { return FLAC__stream_decoder_process_until_end_of_stream(ctx->dec->dec); };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::processUntilEndOfStreamAsync", ctx);
    }

    AsyncDecoderWork* AsyncDecoderWork::forSkipSingleFrame(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () { return FLAC__stream_decoder_skip_single_frame(ctx->dec->dec); };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::skipSingleFrameAsync", ctx);
    }

    AsyncDecoderWork* AsyncDecoderWork::forSeekAbsolute(const Object& self, uint64_t value, DecoderWorkContext* ctx) {
        auto workFunction = [ctx, value] () { return FLAC__stream_decoder_seek_absolute(ctx->dec->dec, value); };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::seekAbsoluteAsync", ctx);
    }

    static auto initConvertFunc = std::bind(numberToJs<int>, _1, _2, false);
    AsyncDecoderWork* AsyncDecoderWork::forInitStream(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () {
            return FLAC__stream_decoder_init_stream(
                ctx->dec->dec,
                ctx->readCbk.IsEmpty() ? nullptr : StreamDecoder::readCallback,
                ctx->seekCbk.IsEmpty() ? nullptr : StreamDecoder::seekCallback,
                ctx->tellCbk.IsEmpty() ? nullptr : StreamDecoder::tellCallback,
                ctx->lengthCbk.IsEmpty() ? nullptr : StreamDecoder::lengthCallback,
                ctx->eofCbk.IsEmpty() ? nullptr : StreamDecoder::eofCallback,
                ctx->writeCbk.IsEmpty() ? nullptr : StreamDecoder::writeCallback,
                ctx->metadataCbk.IsEmpty() ? nullptr : StreamDecoder::metadataCallback,
                ctx->errorCbk.IsEmpty() ? nullptr : StreamDecoder::errorCallback,
                ctx
            );
        };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::initStreamAsync", ctx, initConvertFunc);
    }

    AsyncDecoderWork* AsyncDecoderWork::forInitOggStream(const Object& self, DecoderWorkContext* ctx) {
        auto workFunction = [ctx] () {
            return FLAC__stream_decoder_init_ogg_stream(
                ctx->dec->dec,
                ctx->readCbk.IsEmpty() ? nullptr : StreamDecoder::readCallback,
                ctx->seekCbk.IsEmpty() ? nullptr : StreamDecoder::seekCallback,
                ctx->tellCbk.IsEmpty() ? nullptr : StreamDecoder::tellCallback,
                ctx->lengthCbk.IsEmpty() ? nullptr : StreamDecoder::lengthCallback,
                ctx->eofCbk.IsEmpty() ? nullptr : StreamDecoder::eofCallback,
                ctx->writeCbk.IsEmpty() ? nullptr : StreamDecoder::writeCallback,
                ctx->metadataCbk.IsEmpty() ? nullptr : StreamDecoder::metadataCallback,
                ctx->errorCbk.IsEmpty() ? nullptr : StreamDecoder::errorCallback,
                ctx
            );
        };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::initOggStreamAsync", ctx, initConvertFunc);
    }


    AsyncDecoderWork* AsyncDecoderWork::forInitFile(const Object& self, const std::string &filePath, DecoderWorkContext* ctx) {
        auto workFunction = [ctx, filePath] () {
            return FLAC__stream_decoder_init_file(
                ctx->dec->dec,
                filePath.c_str(),
                ctx->writeCbk.IsEmpty() ? nullptr : StreamDecoder::writeCallback,
                ctx->metadataCbk.IsEmpty() ? nullptr : StreamDecoder::metadataCallback,
                ctx->errorCbk.IsEmpty() ? nullptr : StreamDecoder::errorCallback,
                ctx
            );
        };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::initFileAsync", ctx, initConvertFunc);
    }

    AsyncDecoderWork* AsyncDecoderWork::forInitOggFile(const Object& self, const std::string &filePath, DecoderWorkContext* ctx) {
        auto workFunction = [ctx, filePath] () {
            return FLAC__stream_decoder_init_ogg_file(
                ctx->dec->dec,
                filePath.c_str(),
                ctx->writeCbk.IsEmpty() ? nullptr : StreamDecoder::writeCallback,
                ctx->metadataCbk.IsEmpty() ? nullptr : StreamDecoder::metadataCallback,
                ctx->errorCbk.IsEmpty() ? nullptr : StreamDecoder::errorCallback,
                ctx
            );
        };
        return new AsyncDecoderWork(self, workFunction, "flac_bindings::StreamDecoder::initOggFileAsync", ctx, initConvertFunc);
    }

    void AsyncDecoderWork::onProgress(
        const DecoderWorkContext* ctx,
        Napi::Env& env,
        AsyncDecoderWork::ExecutionProgress& prog,
        const std::shared_ptr<DecoderWorkRequest>& req
    ) {
        auto asyncContext = nullptr;
        std::function<void(Napi::Value result)> processResult;
        Napi::Value result;

        switch(req->type) {
            case DecoderWorkRequest::Type::Eof: {
                result = ctx->eofCbk.MakeCallback(env.Global(), {}, asyncContext);
                processResult = generateParseBooleanResult(req->returnValue, "Decoder:EofCallback");
                break;
            }

            case DecoderWorkRequest::Type::Error: {
                result = ctx->errorCbk.MakeCallback(env.Global(), {numberToJs(env, req->errorCode)}, asyncContext);
                break;
            }

            case DecoderWorkRequest::Type::Length: {
                result = ctx->lengthCbk.MakeCallback(env.Global(), {}, asyncContext);
                processResult = generateParseObjectResult<>(
                    req->returnValue,
                    "Decoder:LengthCallback",
                    "length",
                    *req->offset
                );
                break;
            }

            case DecoderWorkRequest::Type::Metadata: {
                result = ctx->metadataCbk.MakeCallback(
                    env.Global(),
                    {Metadata::toJs(env, const_cast<FLAC__StreamMetadata*>(req->metadata))},
                    asyncContext
                );
                break;
            }

            case DecoderWorkRequest::Type::Read: {
                readSharedBufferRef.setFromWrap(env, req->buffer, *req->bytes);
                result = ctx->readCbk.MakeCallback(env.Global(), {readSharedBufferRef.value()}, asyncContext);
                processResult = generateParseObjectResult(
                    req->returnValue,
                    "Decoder:ReadCallback",
                    "bytes",
                    *req->bytes
                );
                break;
            }

            case DecoderWorkRequest::Type::Seek: {
                result = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, *req->offset)}, asyncContext);
                processResult = generateParseNumberResult(req->returnValue, "Decoder:SeekCallback");
                break;
            }

            case DecoderWorkRequest::Type::Tell: {
                result = ctx->tellCbk.MakeCallback(env.Global(), {}, asyncContext);
                processResult = generateParseObjectResult(
                    req->returnValue,
                    "Decoder:TellCallback",
                    "offset",
                    *req->offset
                );
                break;
            }

            case DecoderWorkRequest::Type::Write: {
                Array buffers = Array::New(env);
                uint32_t channels = FLAC__stream_decoder_get_channels(ctx->dec->dec);
                for(uint32_t ch = 0; ch < channels; ch += 1) {
                    writeSharedBufferRefs[ch].setFromWrap(
                        env,
                        const_cast<int32_t*>(req->samples[ch]),
                        req->frame->header.blocksize
                    );
                    buffers[ch] = writeSharedBufferRefs[ch].value();
                }

                auto jsFrame = frameToJs(env, req->frame);
                result = ctx->writeCbk.MakeCallback(env.Global(), {jsFrame, buffers}, asyncContext);
                processResult = generateParseNumberResult(req->returnValue, "Decoder:WriteCallback");
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