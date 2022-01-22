#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"
#include "decoder.hpp"

namespace flac_bindings {

  using namespace Napi;
  using namespace std::placeholders;

  AsyncDecoderWork::FunctionCallback AsyncDecoderWork::decorate(
    DecoderWorkContext* ctx,
    const std::function<AsyncDecoderWork::ValueType()>& func) {
    return [ctx, func](AsyncDecoderWork::ExecutionProgress& c) {
      ctx->asyncExecutionProgress = &c;

      DEFER(ctx->runLocked([&]() {
        ctx->workInProgress = false;
        ctx->asyncExecutionProgress = nullptr;
      }));

      auto ok = func();
      if (!c.isCompleted()) {
        c.resolve(ok);
      }
    };
  }

  static Napi::Value
    variantIntToJsBoolean(const Napi::Env& env, AsyncDecoderWork::ValueType variant) {
    return booleanToJs(env, std::get<int>(variant));
  }

  AsyncDecoderWork::AsyncDecoderWork(
    const StoreList& list,
    std::function<ValueType()> function,
    const char* name,
    DecoderWorkContext* ctx,
    std::function<Napi::Value(const Napi::Env&, ValueType)> convertFunction):
      AsyncDecoderWorkBase(
        list.begin()->Env(),
        decorate(ctx, function),
        std::bind(&AsyncDecoderWork::onProgress, this, ctx, _1, _2, _3),
        name,
        convertFunction) {
    this->Receiver().Set("this", *list.begin());
    for (auto it = list.begin() + 1; it != list.end(); it += 1) {
      this->Receiver().Set(it - list.begin() - 1, *it);
    }
  }

  AsyncDecoderWork* AsyncDecoderWork::forFinish(const StoreList& list, StreamDecoder& decoder) {
    auto workFunction = [&decoder]() -> int {
      return FLAC__stream_decoder_finish(decoder.ctx->dec);
    };

    auto convertFunction = [&decoder](auto env, auto value) {
      if (std::get<int>(value)) {
        EscapableHandleScope scope(env);
        auto builder = StreamDecoderBuilder::Unwrap(decoder.builder.Value());
        builder->dec = decoder.dec;
        decoder.dec = nullptr;
        return scope.Escape(decoder.builder.Value());
      }

      return env.Null();
    };

    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::finishAsync",
      decoder.ctx.get(),
      convertFunction);
  }

  AsyncDecoderWork* AsyncDecoderWork::forFlush(const StoreList& list, DecoderWorkContext* ctx) {
    auto workFunction = [ctx]() {
      return FLAC__stream_decoder_flush(ctx->dec);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::flushAsync",
      ctx,
      variantIntToJsBoolean);
  }

  AsyncDecoderWork*
    AsyncDecoderWork::forProcessSingle(const StoreList& list, DecoderWorkContext* ctx) {
    auto workFunction = [ctx]() {
      return FLAC__stream_decoder_process_single(ctx->dec);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::processSingleAsync",
      ctx,
      variantIntToJsBoolean);
  }

  AsyncDecoderWork*
    AsyncDecoderWork::forProcessUntilEndOfMetadata(const StoreList& list, DecoderWorkContext* ctx) {
    auto workFunction = [ctx]() {
      return FLAC__stream_decoder_process_until_end_of_metadata(ctx->dec);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::processUntilEndOfMetadataAsync",
      ctx,
      variantIntToJsBoolean);
  }

  AsyncDecoderWork*
    AsyncDecoderWork::forProcessUntilEndOfStream(const StoreList& list, DecoderWorkContext* ctx) {
    auto workFunction = [ctx]() {
      return FLAC__stream_decoder_process_until_end_of_stream(ctx->dec);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::processUntilEndOfStreamAsync",
      ctx,
      variantIntToJsBoolean);
  }

  AsyncDecoderWork*
    AsyncDecoderWork::forSkipSingleFrame(const StoreList& list, DecoderWorkContext* ctx) {
    auto workFunction = [ctx]() {
      return FLAC__stream_decoder_skip_single_frame(ctx->dec);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::skipSingleFrameAsync",
      ctx,
      variantIntToJsBoolean);
  }

  AsyncDecoderWork* AsyncDecoderWork::forSeekAbsolute(
    const StoreList& list,
    uint64_t value,
    DecoderWorkContext* ctx) {
    auto workFunction = [ctx, value]() {
      return FLAC__stream_decoder_seek_absolute(ctx->dec, value);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::seekAbsoluteAsync",
      ctx,
      variantIntToJsBoolean);
  }

  AsyncDecoderWork* AsyncDecoderWork::forInitStream(
    const StoreList& list,
    std::shared_ptr<DecoderWorkContext> ctx,
    StreamDecoderBuilder& builder) {
    auto workFunction = [ctx]() {
      return FLAC__stream_decoder_init_stream(
        ctx->dec,
        ctx->readCbk.IsEmpty() ? nullptr : AsyncDecoderWork::readCallback,
        ctx->seekCbk.IsEmpty() ? nullptr : AsyncDecoderWork::seekCallback,
        ctx->tellCbk.IsEmpty() ? nullptr : AsyncDecoderWork::tellCallback,
        ctx->lengthCbk.IsEmpty() ? nullptr : AsyncDecoderWork::lengthCallback,
        ctx->eofCbk.IsEmpty() ? nullptr : AsyncDecoderWork::eofCallback,
        ctx->writeCbk.IsEmpty() ? nullptr : AsyncDecoderWork::writeCallback,
        ctx->metadataCbk.IsEmpty() ? nullptr : AsyncDecoderWork::metadataCallback,
        ctx->errorCbk.IsEmpty() ? nullptr : AsyncDecoderWork::errorCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, auto value) {
      builder.checkInitStatus(env, std::get<FLAC__StreamDecoderInitStatus>(value));
      return builder.createDecoder(env, builder.Value(), ctx);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoderBuilder::buildWithStreamAsync",
      ctx.get(),
      convertFunction);
  }

  AsyncDecoderWork* AsyncDecoderWork::forInitOggStream(
    const StoreList& list,
    std::shared_ptr<DecoderWorkContext> ctx,
    StreamDecoderBuilder& builder) {
    auto workFunction = [ctx]() {
      return FLAC__stream_decoder_init_ogg_stream(
        ctx->dec,
        ctx->readCbk.IsEmpty() ? nullptr : AsyncDecoderWork::readCallback,
        ctx->seekCbk.IsEmpty() ? nullptr : AsyncDecoderWork::seekCallback,
        ctx->tellCbk.IsEmpty() ? nullptr : AsyncDecoderWork::tellCallback,
        ctx->lengthCbk.IsEmpty() ? nullptr : AsyncDecoderWork::lengthCallback,
        ctx->eofCbk.IsEmpty() ? nullptr : AsyncDecoderWork::eofCallback,
        ctx->writeCbk.IsEmpty() ? nullptr : AsyncDecoderWork::writeCallback,
        ctx->metadataCbk.IsEmpty() ? nullptr : AsyncDecoderWork::metadataCallback,
        ctx->errorCbk.IsEmpty() ? nullptr : AsyncDecoderWork::errorCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, auto value) {
      builder.checkInitStatus(env, std::get<FLAC__StreamDecoderInitStatus>(value));
      return builder.createDecoder(env, builder.Value(), ctx);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoderBuilder::buildWithOggStreamAsync",
      ctx.get(),
      convertFunction);
  }

  AsyncDecoderWork* AsyncDecoderWork::forInitFile(
    const StoreList& list,
    const std::string& filePath,
    std::shared_ptr<DecoderWorkContext> ctx,
    StreamDecoderBuilder& builder) {
    auto workFunction = [ctx, filePath]() {
      return FLAC__stream_decoder_init_file(
        ctx->dec,
        filePath.c_str(),
        ctx->writeCbk.IsEmpty() ? nullptr : AsyncDecoderWork::writeCallback,
        ctx->metadataCbk.IsEmpty() ? nullptr : AsyncDecoderWork::metadataCallback,
        ctx->errorCbk.IsEmpty() ? nullptr : AsyncDecoderWork::errorCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, auto value) {
      builder.checkInitStatus(env, std::get<FLAC__StreamDecoderInitStatus>(value));
      return builder.createDecoder(env, builder.Value(), ctx);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoderBuilder::buildWithFileAsync",
      ctx.get(),
      convertFunction);
  }

  AsyncDecoderWork* AsyncDecoderWork::forInitOggFile(
    const StoreList& list,
    const std::string& filePath,
    std::shared_ptr<DecoderWorkContext> ctx,
    StreamDecoderBuilder& builder) {
    auto workFunction = [ctx, filePath]() {
      return FLAC__stream_decoder_init_ogg_file(
        ctx->dec,
        filePath.c_str(),
        ctx->writeCbk.IsEmpty() ? nullptr : AsyncDecoderWork::writeCallback,
        ctx->metadataCbk.IsEmpty() ? nullptr : AsyncDecoderWork::metadataCallback,
        ctx->errorCbk.IsEmpty() ? nullptr : AsyncDecoderWork::errorCallback,
        ctx.get());
    };
    auto convertFunction = [ctx, &builder](const Napi::Env& env, auto value) {
      builder.checkInitStatus(env, std::get<FLAC__StreamDecoderInitStatus>(value));
      return builder.createDecoder(env, builder.Value(), ctx);
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoderBuilder::buildWithOggFileAsync",
      ctx.get(),
      convertFunction);
  }

  AsyncDecoderWork*
    AsyncDecoderWork::forGetDecoderPosition(const StoreList& list, DecoderWorkContext* ctx) {
    auto workFunction = [ctx]() -> AsyncDecoderWork::ValueType {
      uint64_t pos;
      auto ret = FLAC__stream_decoder_get_decode_position(ctx->dec, &pos);
      if (ret) {
        return pos;
      }

      // well yes that is a bit of a hack :)
      return -1;
    };
    auto convertFunction = [](const Napi::Env& env, AsyncDecoderWork::ValueType value) {
      if (std::holds_alternative<uint64_t>(value)) {
        return numberToJs(env, std::get<uint64_t>(value));
      }

      return env.Null();
    };
    return new AsyncDecoderWork(
      list,
      workFunction,
      "flac_bindings::StreamDecoder::getDecoderPosition",
      ctx,
      convertFunction);
  }

  void AsyncDecoderWork::onProgress(
    const DecoderWorkContext* ctx,
    Napi::Env& env,
    AsyncDecoderWork::ExecutionProgress& prog,
    const std::shared_ptr<DecoderWorkRequest>& req) {
    std::function<void(Napi::Value result)> processResult;
    Napi::Value result;

    if (std::holds_alternative<DecoderWorkRequest::Eof>(req->data)) {
      auto& eofRequest = std::get<DecoderWorkRequest::Eof>(req->data);
      result = ctx->eofCbk.MakeCallback(env.Global(), {});
      processResult = generateParseBooleanResult(eofRequest.returnValue, "Decoder:EofCallback");
    } else if (std::holds_alternative<DecoderWorkRequest::Error>(req->data)) {
      auto& errorRequest = std::get<DecoderWorkRequest::Error>(req->data);
      result = ctx->errorCbk.MakeCallback(env.Global(), {numberToJs(env, errorRequest.error)});
    } else if (std::holds_alternative<DecoderWorkRequest::Length>(req->data)) {
      auto& lengthRequest = std::get<DecoderWorkRequest::Length>(req->data);
      result = ctx->lengthCbk.MakeCallback(env.Global(), {});
      processResult = generateParseObjectResult<>(
        lengthRequest.returnValue,
        "Decoder:LengthCallback",
        "length",
        lengthRequest.length);
    } else if (std::holds_alternative<DecoderWorkRequest::Metadata>(req->data)) {
      auto& metadataRequest = std::get<DecoderWorkRequest::Metadata>(req->data);
      auto metadataObject =
        Metadata::toJs(env, const_cast<FLAC__StreamMetadata*>(metadataRequest.metadata));
      result = ctx->metadataCbk.MakeCallback(env.Global(), {metadataObject});
    } else if (std::holds_alternative<DecoderWorkRequest::Read>(req->data)) {
      auto& readRequest = std::get<DecoderWorkRequest::Read>(req->data);
      readSharedBufferRef.setFromWrap(env, readRequest.buffer, readRequest.bytes);
      result = ctx->readCbk.MakeCallback(env.Global(), {readSharedBufferRef.value()});
      processResult = generateParseObjectResult(
        readRequest.returnValue,
        "Decoder:ReadCallback",
        "bytes",
        readRequest.bytes);
    } else if (std::holds_alternative<DecoderWorkRequest::Seek>(req->data)) {
      auto& seekRequest = std::get<DecoderWorkRequest::Seek>(req->data);
      result = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, seekRequest.offset)});
      processResult = generateParseNumberResult(seekRequest.returnValue, "Decoder:SeekCallback");
    } else if (std::holds_alternative<DecoderWorkRequest::Tell>(req->data)) {
      auto& tellRequest = std::get<DecoderWorkRequest::Tell>(req->data);
      result = ctx->tellCbk.MakeCallback(env.Global(), {});
      processResult = generateParseObjectResult(
        tellRequest.returnValue,
        "Decoder:TellCallback",
        "offset",
        tellRequest.offset);
    } else if (std::holds_alternative<DecoderWorkRequest::Write>(req->data)) {
      auto& writeRequest = std::get<DecoderWorkRequest::Write>(req->data);
      Array buffers = Array::New(env);
      uint32_t channels = FLAC__stream_decoder_get_channels(ctx->dec);
      for (uint32_t ch = 0; ch < channels; ch += 1) {
        writeSharedBufferRefs[ch].setFromWrap(
          env,
          const_cast<int32_t*>(writeRequest.samples[ch]),
          writeRequest.frame->header.blocksize);
        buffers[ch] = writeSharedBufferRefs[ch].value();
      }

      auto jsFrame = frameToJs(env, writeRequest.frame);
      result = ctx->writeCbk.MakeCallback(env.Global(), {jsFrame, buffers});
      processResult = generateParseNumberResult(writeRequest.returnValue, "Decoder:WriteCallback");
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

  FLAC__StreamDecoderReadStatus AsyncDecoderWork::readCallback(
    const FLAC__StreamDecoder*,
    FLAC__byte buffer[],
    size_t* bytes,
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Read {
      buffer,
      bytes,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<DecoderWorkRequest::Read>(request->data).returnValue;
  }

  FLAC__StreamDecoderSeekStatus
    AsyncDecoderWork::seekCallback(const FLAC__StreamDecoder*, uint64_t offset, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Seek {
      offset,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<DecoderWorkRequest::Seek>(request->data).returnValue;
  }

  FLAC__StreamDecoderTellStatus
    AsyncDecoderWork::tellCallback(const FLAC__StreamDecoder*, uint64_t* offset, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Tell {
      offset,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<DecoderWorkRequest::Tell>(request->data).returnValue;
  }

  FLAC__StreamDecoderLengthStatus
    AsyncDecoderWork::lengthCallback(const FLAC__StreamDecoder*, uint64_t* length, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Length {
      length,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<DecoderWorkRequest::Length>(request->data).returnValue;
  }

  FLAC__bool AsyncDecoderWork::eofCallback(const FLAC__StreamDecoder*, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Eof());

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<DecoderWorkRequest::Eof>(request->data).returnValue;
  }

  FLAC__StreamDecoderWriteStatus AsyncDecoderWork::writeCallback(
    const FLAC__StreamDecoder*,
    const FLAC__Frame* frame,
    const int32_t* const buffer[],
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Write {
      frame,
      buffer,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);

    return std::get<DecoderWorkRequest::Write>(request->data).returnValue;
  }

  void AsyncDecoderWork::metadataCallback(
    const FLAC__StreamDecoder*,
    const FLAC__StreamMetadata* metadata,
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Metadata {
      metadata,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);
  }

  void AsyncDecoderWork::errorCallback(
    const FLAC__StreamDecoder*,
    FLAC__StreamDecoderErrorStatus error,
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;

    auto request = std::make_shared<DecoderWorkRequest>(DecoderWorkRequest::Error {
      error,
    });

    ctx->asyncExecutionProgress->sendProgressAndWait(request);
  }
}
