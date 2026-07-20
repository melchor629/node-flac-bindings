#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"
#include "encoder.hpp"

namespace flac_bindings {

  using namespace Napi;
  using namespace std::placeholders;

  AsyncEncoderWork::FunctionCallback
    AsyncEncoderWork::decorate(EncoderWorkContext* ctx, const std::function<int()>& func) {
    return [ctx, func](ExecutionProgress& c) {
      ctx->asyncExecutionProgress = &c;

      DEFER(ctx->runLocked([&]() {
        ctx->workInProgress = false;
        ctx->asyncExecutionProgress = nullptr;
      }));

      int ok = func();
      if (!c.isCompleted()) {
        c.resolve(ok);
      }
    };
  }

  AsyncEncoderWork::AsyncEncoderWork(
    const StoreList& list,
    std::function<int()> function,
    const char* name,
    EncoderWorkContext* ctx,
    std::function<Napi::Value(const Napi::Env&, int)> convertFunction):
      AsyncEncoderWorkBase(
        list.begin()->Env(),
        decorate(ctx, function),
        std::bind(&AsyncEncoderWork::onProgress, this, ctx, _1, _2, _3),
        name,
        convertFunction) {
    this->Receiver().Set("this", *list.begin());
    for (auto it = list.begin() + 1; it != list.end(); it += 1) {
      this->Receiver().Set(it - list.begin() - 1, *it);
    }
  }

  AsyncEncoderWork* AsyncEncoderWork::forFinish(const StoreList& list, StreamEncoder& encoder) {
    auto workFunction = [&encoder]() {
      return FLAC__stream_encoder_finish(encoder.ctx->enc);
    };

    auto convertFunction = [&encoder](auto env, auto value) {
      if (value) {
        EscapableHandleScope scope(env);
        auto builderJs = encoder.builder.Value();
        auto builder = StreamEncoderBuilder::Unwrap(builderJs);
        builder->enc = encoder.enc;
        encoder.enc = nullptr;
        return scope.Escape(builderJs);
      }

      return env.Null();
    };

    return new AsyncEncoderWork(
      list,
      workFunction,
      "flac_bindings::StreamEncoder::finishAsync",
      encoder.ctx.get(),
      convertFunction);
  }

  AsyncEncoderWork* AsyncEncoderWork::forProcess(
    const StoreList& list,
    const std::vector<int32_t*>& buffers,
    uint64_t samples,
    EncoderWorkContext* ctx) {
    auto workFunction = [ctx, buffers, samples]() {
      return FLAC__stream_encoder_process(ctx->enc, buffers.data(), samples);
    };

    return new AsyncEncoderWork(
      list,
      workFunction,
      "flac_bindings::StreamEncoder::processAsync",
      ctx);
  }

  AsyncEncoderWork* AsyncEncoderWork::forProcessInterleaved(
    const StoreList& list,
    int32_t* buffer,
    uint64_t samples,
    EncoderWorkContext* ctx) {
    auto workFunction = [ctx, buffer, samples]() {
      return FLAC__stream_encoder_process_interleaved(ctx->enc, buffer, samples);
    };

    return new AsyncEncoderWork(
      list,
      workFunction,
      "flac_bindings::StreamEncoder::processInterleavedAsync",
      ctx);
  }

  AsyncEncoderWork* AsyncEncoderWork::forInitStream(
    const StoreList& list,
    std::shared_ptr<EncoderWorkContext> ctx,
    StreamEncoderBuilder& builder) {
    auto workFunction = [ctx]() {
      return FLAC__stream_encoder_init_stream(
        ctx->enc,
        ctx->writeCbk.IsEmpty() ? nullptr : AsyncEncoderWork::writeCallback,
        ctx->seekCbk.IsEmpty() ? nullptr : AsyncEncoderWork::seekCallback,
        ctx->tellCbk.IsEmpty() ? nullptr : AsyncEncoderWork::tellCallback,
        ctx->metadataCbk.IsEmpty() ? nullptr : AsyncEncoderWork::metadataCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, int value) {
      builder.checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
      return builder.createEncoder(env, builder.Value(), ctx);
    };

    return new AsyncEncoderWork(
      list,
      workFunction,
      "flac_bindings::StreamEncoderBuilder::buildWithStreamAsync",
      ctx.get(),
      convertFunction);
  }

  AsyncEncoderWork* AsyncEncoderWork::forInitOggStream(
    const StoreList& list,
    std::shared_ptr<EncoderWorkContext> ctx,
    StreamEncoderBuilder& builder) {
    auto workFunction = [ctx]() {
      return FLAC__stream_encoder_init_ogg_stream(
        ctx->enc,
        ctx->readCbk.IsEmpty() ? nullptr : AsyncEncoderWork::readCallback,
        ctx->writeCbk.IsEmpty() ? nullptr : AsyncEncoderWork::writeCallback,
        ctx->seekCbk.IsEmpty() ? nullptr : AsyncEncoderWork::seekCallback,
        ctx->tellCbk.IsEmpty() ? nullptr : AsyncEncoderWork::tellCallback,
        ctx->metadataCbk.IsEmpty() ? nullptr : AsyncEncoderWork::metadataCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, int value) {
      builder.checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
      return builder.createEncoder(env, builder.Value(), ctx);
    };

    return new AsyncEncoderWork(
      list,
      workFunction,
      "flac_bindings::StreamEncoderBuilder::buildWithOggStreamAsync",
      ctx.get(),
      convertFunction);
  }

  AsyncEncoderWork* AsyncEncoderWork::forInitFile(
    const StoreList& list,
    const std::string& path,
    std::shared_ptr<EncoderWorkContext> ctx,
    StreamEncoderBuilder& builder) {
    auto workFunction = [ctx, path]() {
      return FLAC__stream_encoder_init_file(
        ctx->enc,
        path.c_str(),
        ctx->progressCbk.IsEmpty() ? nullptr : AsyncEncoderWork::progressCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, int value) {
      builder.checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
      return builder.createEncoder(env, builder.Value(), ctx);
    };

    return new AsyncEncoderWork(
      list,
      workFunction,
      "flac_bindings::StreamEncoderBuilder::buildWithFileAsync",
      ctx.get(),
      convertFunction);
  }

  AsyncEncoderWork* AsyncEncoderWork::forInitOggFile(
    const StoreList& list,
    const std::string& path,
    std::shared_ptr<EncoderWorkContext> ctx,
    StreamEncoderBuilder& builder) {
    auto workFunction = [ctx, path]() {
      return FLAC__stream_encoder_init_ogg_file(
        ctx->enc,
        path.c_str(),
        ctx->progressCbk.IsEmpty() ? nullptr : AsyncEncoderWork::progressCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, int value) {
      builder.checkInitStatus(env, (FLAC__StreamEncoderInitStatus) value);
      return builder.createEncoder(env, builder.Value(), ctx);
    };

    return new AsyncEncoderWork(
      list,
      workFunction,
      "flac_bindings::StreamEncoderBuilder::buildWithOggFileAsync",
      ctx.get(),
      convertFunction);
  }

  void AsyncEncoderWork::onProgress(
    const EncoderWorkContext* ctx,
    Napi::Env& env,
    ExecutionProgress& prog,
    const std::shared_ptr<EncoderWorkRequest>& req) {
    std::function<void(Napi::Value result)> processResult;
    Napi::Value result;

    if (std::holds_alternative<EncoderWorkRequest::Read>(req->data)) {
      auto& readRequest = std::get<EncoderWorkRequest::Read>(req->data);
      sharedBufferRef.setFromWrap(env, readRequest.buffer, readRequest.bytes);
      result = ctx->readCbk.MakeCallback(env.Global(), {sharedBufferRef.value()});
      processResult = generateParseObjectResult(
        readRequest.returnValue,
        "Encoder:ReadCallback",
        "bytes",
        readRequest.bytes);
    } else if (std::holds_alternative<EncoderWorkRequest::Write>(req->data)) {
      auto& writeRequest = std::get<EncoderWorkRequest::Write>(req->data);
      sharedBufferRef.setFromWrap(
        env,
        const_cast<FLAC__byte*>(writeRequest.buffer),
        writeRequest.bytes);
      result = ctx->writeCbk.MakeCallback(
        env.Global(),
        {sharedBufferRef.value(),
         numberToJs(env, writeRequest.samples),
         numberToJs(env, writeRequest.frame)});
      processResult = generateParseNumberResult(writeRequest.returnValue, "Encoder:WriteCallback");
    } else if (std::holds_alternative<EncoderWorkRequest::Seek>(req->data)) {
      auto& seekRequest = std::get<EncoderWorkRequest::Seek>(req->data);
      result = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, seekRequest.offset)});
      processResult = generateParseNumberResult(seekRequest.returnValue, "Encoder:SeekCallback");
    } else if (std::holds_alternative<EncoderWorkRequest::Tell>(req->data)) {
      auto& tellRequest = std::get<EncoderWorkRequest::Tell>(req->data);
      result = ctx->tellCbk.MakeCallback(env.Global(), {});
      processResult = generateParseObjectResult(
        tellRequest.returnValue,
        "Encoder:TellCallback",
        "offset",
        tellRequest.offset);
    } else if (std::holds_alternative<EncoderWorkRequest::Metadata>(req->data)) {
      auto& metadataRequest = std::get<EncoderWorkRequest::Metadata>(req->data);
      auto jsMetadata =
        Metadata::toJs(env, const_cast<FLAC__StreamMetadata*>(metadataRequest.metadata));
      result = ctx->metadataCbk.MakeCallback(env.Global(), {jsMetadata});
    } else if (std::holds_alternative<EncoderWorkRequest::Progress>(req->data)) {
      auto& progressRequest = std::get<EncoderWorkRequest::Progress>(req->data);
      result = ctx->progressCbk.MakeCallback(
        env.Global(),
        {
          numberToJs(env, progressRequest.bytesWritten),
          numberToJs(env, progressRequest.samplesWritten),
          numberToJs(env, progressRequest.framesWritten),
          numberToJs(env, progressRequest.totalFramesEstimate),
        });
    }

    if (result.IsPromise()) {
      auto promise = result.As<Promise>();
      prog.defer(promise, [processResult](auto&, auto& info, auto) {
        if (processResult) {
          processResult(info[0]);
        }
      });
    } else {
      if (processResult) {
        processResult(result);
      }
    }
  }

  FLAC__StreamEncoderReadStatus AsyncEncoderWork::readCallback(
    const FLAC__StreamEncoder*,
    FLAC__byte buffer[],
    size_t* bytes,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;

    auto request = std::make_shared<EncoderWorkRequest>(EncoderWorkRequest::Read {
      buffer,
      bytes,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<EncoderWorkRequest::Read>(request->data).returnValue;
  }

  FLAC__StreamEncoderWriteStatus AsyncEncoderWork::writeCallback(
    const FLAC__StreamEncoder*,
    const FLAC__byte buffer[],
    size_t bytes,
    unsigned samples,
    unsigned frame,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;

    auto request = std::make_shared<EncoderWorkRequest>(
      EncoderWorkRequest::Write {buffer, bytes, samples, frame});

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<EncoderWorkRequest::Write>(request->data).returnValue;
  }

  FLAC__StreamEncoderSeekStatus
    AsyncEncoderWork::seekCallback(const FLAC__StreamEncoder*, uint64_t offset, void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;

    auto request = std::make_shared<EncoderWorkRequest>(EncoderWorkRequest::Seek(offset));

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<EncoderWorkRequest::Seek>(request->data).returnValue;
  }

  FLAC__StreamEncoderTellStatus
    AsyncEncoderWork::tellCallback(const FLAC__StreamEncoder*, uint64_t* offset, void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;

    auto request = std::make_shared<EncoderWorkRequest>(EncoderWorkRequest::Tell(offset));

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<EncoderWorkRequest::Tell>(request->data).returnValue;
  }

  void AsyncEncoderWork::metadataCallback(
    const FLAC__StreamEncoder*,
    const FLAC__StreamMetadata* metadata,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;

    auto request = std::make_shared<EncoderWorkRequest>(EncoderWorkRequest::Metadata(metadata));

    ctx->asyncExecutionProgress->sendProgressAndWait(request);
  }

  void AsyncEncoderWork::progressCallback(
    const FLAC__StreamEncoder*,
    uint64_t bytesWritten,
    uint64_t samplesWritten,
    unsigned framesWritten,
    unsigned totalFramesEstimate,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;

    auto request = std::make_shared<EncoderWorkRequest>(EncoderWorkRequest::Progress {
      bytesWritten,
      samplesWritten,
      framesWritten,
      totalFramesEstimate,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);
  }
}
