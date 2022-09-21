#include "decoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"
#include <memory>

namespace flac_bindings {

  Function StreamDecoder::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto constructor = DefineClass(
      env,
      "StreamDecoder",
      {
        InstanceMethod("getMd5Checking", &StreamDecoder::getMd5Checking),
        InstanceMethod("getTotalSamples", &StreamDecoder::getTotalSamples),
        InstanceMethod("getChannels", &StreamDecoder::getChannels),
        InstanceMethod("getChannelAssignment", &StreamDecoder::getChannelAssignment),
        InstanceMethod("getBitsPerSample", &StreamDecoder::getBitsPerSample),
        InstanceMethod("getSampleRate", &StreamDecoder::getSampleRate),
        InstanceMethod("getBlocksize", &StreamDecoder::getBlocksize),

        InstanceMethod("finish", &StreamDecoder::finish),
        InstanceMethod("flush", &StreamDecoder::flush),
        InstanceMethod("reset", &StreamDecoder::reset),
        InstanceMethod("processSingle", &StreamDecoder::processSingle),
        InstanceMethod("processUntilEndOfMetadata", &StreamDecoder::processUntilEndOfMetadata),
        InstanceMethod("processUntilEndOfStream", &StreamDecoder::processUntilEndOfStream),
        InstanceMethod("skipSingleFrame", &StreamDecoder::skipSingleFrame),
        InstanceMethod("seekAbsolute", &StreamDecoder::seekAbsolute),
        InstanceMethod("getDecodePosition", &StreamDecoder::getDecodePosition),

        InstanceMethod("getState", &StreamDecoder::getState),
        InstanceMethod("getResolvedStateString", &StreamDecoder::getResolvedStateString),

        InstanceMethod("finishAsync", &StreamDecoder::finishAsync),
        InstanceMethod("flushAsync", &StreamDecoder::flushAsync),
        InstanceMethod("processSingleAsync", &StreamDecoder::processSingleAsync),
        InstanceMethod(
          "processUntilEndOfMetadataAsync",
          &StreamDecoder::processUntilEndOfMetadataAsync),
        InstanceMethod(
          "processUntilEndOfStreamAsync",
          &StreamDecoder::processUntilEndOfStreamAsync),
        InstanceMethod("skipSingleFrameAsync", &StreamDecoder::skipSingleFrameAsync),
        InstanceMethod("seekAbsoluteAsync", &StreamDecoder::seekAbsoluteAsync),
        InstanceMethod("getDecodePositionAsync", &StreamDecoder::getDecodePositionAsync),
      });
    c_enum::declareInObject(constructor, "State", createStateEnum);
    c_enum::declareInObject(constructor, "InitStatus", createInitStatusEnum);
    c_enum::declareInObject(constructor, "ReadStatus", createReadStatusEnum);
    c_enum::declareInObject(constructor, "SeekStatus", createSeekStatusEnum);
    c_enum::declareInObject(constructor, "TellStatus", createTellStatusEnum);
    c_enum::declareInObject(constructor, "LengthStatus", createLengthStatusEnum);
    c_enum::declareInObject(constructor, "WriteStatus", createWriteStatusEnum);
    c_enum::declareInObject(constructor, "ErrorStatus", createErrorStatusEnum);

    addon.decoderConstructor = Persistent(constructor);

    return scope.Escape(objectFreeze(constructor)).As<Function>();
  }

  StreamDecoder::StreamDecoder(const CallbackInfo& info): ObjectWrap<StreamDecoder>(info) {
    auto addon = info.Env().GetInstanceData<FlacAddon>();
    auto checks = info.Length() != 1 || !info[0].IsObject()
                  || !info[0].As<Object>().InstanceOf(addon->decoderBuilderConstructor.Value());
    if (checks) {
      throw Error::New(
        info.Env(),
        "StreamDecoder constructor cannot be called directly, use StreamDecoderBuilder instead");
    }

    builder = Napi::Persistent(info[0].As<Object>());
  }

  StreamDecoder::~StreamDecoder() {
    if (dec != nullptr) {
      FLAC__stream_decoder_delete(dec);
    }

    builder.Unref();
  }

  // -- getters --

  Napi::Value StreamDecoder::getMd5Checking(const CallbackInfo& info) {
    auto md5Checking = FLAC__stream_decoder_get_md5_checking(dec);
    return booleanToJs(info.Env(), md5Checking);
  }

  Napi::Value StreamDecoder::getTotalSamples(const CallbackInfo& info) {
    auto totalSamples = FLAC__stream_decoder_get_total_samples(dec);
    return numberToJs(info.Env(), totalSamples);
  }

  Napi::Value StreamDecoder::getChannels(const CallbackInfo& info) {
    auto channels = FLAC__stream_decoder_get_channels(dec);
    return numberToJs(info.Env(), channels);
  }

  Napi::Value StreamDecoder::getChannelAssignment(const CallbackInfo& info) {
    auto channelAssignment = FLAC__stream_decoder_get_channel_assignment(dec);
    return numberToJs(info.Env(), channelAssignment);
  }

  Napi::Value StreamDecoder::getBitsPerSample(const CallbackInfo& info) {
    auto bitsPerSample = FLAC__stream_decoder_get_bits_per_sample(dec);
    return numberToJs(info.Env(), bitsPerSample);
  }

  Napi::Value StreamDecoder::getSampleRate(const CallbackInfo& info) {
    auto sampleRate = FLAC__stream_decoder_get_sample_rate(dec);
    return numberToJs(info.Env(), sampleRate);
  }

  Napi::Value StreamDecoder::getBlocksize(const CallbackInfo& info) {
    auto blocksize = FLAC__stream_decoder_get_blocksize(dec);
    return numberToJs(info.Env(), blocksize);
  }

  // -- sync operations --

  Napi::Value StreamDecoder::finish(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_decoder_finish(dec);
    if (info.Env().IsExceptionPending()) {
      return Napi::Value();
    }

    if (ret) {
      EscapableHandleScope scope(info.Env());
      auto builder = StreamDecoderBuilder::Unwrap(this->builder.Value());
      builder->dec = dec;
      dec = nullptr;
      return scope.Escape(this->builder.Value());
    }

    return info.Env().Null();
  }

  Napi::Value StreamDecoder::flush(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_decoder_flush(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::reset(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_decoder_reset(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::processSingle(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_decoder_process_single(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::processUntilEndOfMetadata(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_decoder_process_until_end_of_metadata(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::processUntilEndOfStream(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_decoder_process_until_end_of_stream(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::skipSingleFrame(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_decoder_skip_single_frame(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::seekAbsolute(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    auto offset = numberFromJs<uint64_t>(info[0]);
    auto ret = FLAC__stream_decoder_seek_absolute(dec, offset);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::getDecodePosition(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Sync);

    uint64_t pos;
    auto ret = FLAC__stream_decoder_get_decode_position(dec, &pos);
    if (ret && !info.Env().IsExceptionPending()) {
      return numberToJs(info.Env(), pos);
    }

    return info.Env().IsExceptionPending() ? Napi::Value() : info.Env().Null();
  }

  // -- state getters --

  Napi::Value StreamDecoder::getState(const CallbackInfo& info) {
    auto state = FLAC__stream_decoder_get_state(dec);
    return numberToJs(info.Env(), state);
  }

  Napi::Value StreamDecoder::getResolvedStateString(const CallbackInfo& info) {
    auto stateString = FLAC__stream_decoder_get_resolved_state_string(dec);
    return String::New(info.Env(), stateString);
  }

  // -- async operations --

  Napi::Value StreamDecoder::finishAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    AsyncDecoderWork* work = AsyncDecoderWork::forFinish({info.This()}, *this);
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::flushAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    AsyncDecoderWork* work = AsyncDecoderWork::forFlush({info.This()}, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::processSingleAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    AsyncDecoderWork* work = AsyncDecoderWork::forProcessSingle({info.This()}, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::processUntilEndOfMetadataAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    AsyncDecoderWork* work =
      AsyncDecoderWork::forProcessUntilEndOfMetadata({info.This()}, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::processUntilEndOfStreamAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    AsyncDecoderWork* work = AsyncDecoderWork::forProcessUntilEndOfStream({info.This()}, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::skipSingleFrameAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    AsyncDecoderWork* work = AsyncDecoderWork::forSkipSingleFrame({info.This()}, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::seekAbsoluteAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    auto sample = numberFromJs<uint64_t>(info[0]);
    AsyncDecoderWork* work = AsyncDecoderWork::forSeekAbsolute({info.This()}, sample, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::getDecodePositionAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), DecoderWorkContext::ExecutionMode::Async);

    auto work = AsyncDecoderWork::forGetDecoderPosition({info.This()}, ctx.get());
    return enqueueWork(work);
  }

  // -- enums --

  c_enum::DefineReturnType StreamDecoder::createStateEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(
      obj1,
      obj2,
      "SEARCH_FOR_METADATA",
      FLAC__STREAM_DECODER_SEARCH_FOR_METADATA);
    c_enum::defineValue(obj1, obj2, "READ_METADATA", FLAC__STREAM_DECODER_READ_METADATA);
    c_enum::defineValue(
      obj1,
      obj2,
      "SEARCH_FOR_FRAME_SYNC",
      FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC);
    c_enum::defineValue(obj1, obj2, "READ_FRAME", FLAC__STREAM_DECODER_READ_FRAME);
    c_enum::defineValue(obj1, obj2, "END_OF_STREAM", FLAC__STREAM_DECODER_END_OF_STREAM);
    c_enum::defineValue(obj1, obj2, "OGG_ERROR", FLAC__STREAM_DECODER_OGG_ERROR);
    c_enum::defineValue(obj1, obj2, "SEEK_ERROR", FLAC__STREAM_DECODER_SEEK_ERROR);
    c_enum::defineValue(obj1, obj2, "ABORTED", FLAC__STREAM_DECODER_ABORTED);
    c_enum::defineValue(
      obj1,
      obj2,
      "MEMORY_ALLOCATION_ERROR",
      FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR);
    c_enum::defineValue(obj1, obj2, "UNINITIALIZED", FLAC__STREAM_DECODER_UNINITIALIZED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamDecoder::createInitStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_DECODER_INIT_STATUS_OK);
    c_enum::defineValue(
      obj1,
      obj2,
      "UNSUPPORTED_CONTAINER",
      FLAC__STREAM_DECODER_INIT_STATUS_UNSUPPORTED_CONTAINER);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_CALLBACKS",
      FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS);
    c_enum::defineValue(
      obj1,
      obj2,
      "MEMORY_ALLOCATION_ERROR",
      FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR);
    c_enum::defineValue(
      obj1,
      obj2,
      "ERROR_OPENING_FILE",
      FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE);
    c_enum::defineValue(
      obj1,
      obj2,
      "ALREADY_INITIALIZED",
      FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamDecoder::createReadStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "CONTINUE", FLAC__STREAM_DECODER_READ_STATUS_CONTINUE);
    c_enum::defineValue(
      obj1,
      obj2,
      "END_OF_STREAM",
      FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM);
    c_enum::defineValue(obj1, obj2, "ABORT", FLAC__STREAM_DECODER_READ_STATUS_ABORT);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamDecoder::createSeekStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_DECODER_SEEK_STATUS_OK);
    c_enum::defineValue(obj1, obj2, "ERROR", FLAC__STREAM_DECODER_SEEK_STATUS_ERROR);
    c_enum::defineValue(obj1, obj2, "UNSUPPORTED", FLAC__STREAM_DECODER_SEEK_STATUS_UNSUPPORTED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamDecoder::createTellStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_DECODER_TELL_STATUS_OK);
    c_enum::defineValue(obj1, obj2, "ERROR", FLAC__STREAM_DECODER_TELL_STATUS_ERROR);
    c_enum::defineValue(obj1, obj2, "UNSUPPORTED", FLAC__STREAM_DECODER_TELL_STATUS_UNSUPPORTED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamDecoder::createLengthStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_DECODER_LENGTH_STATUS_OK);
    c_enum::defineValue(obj1, obj2, "ERROR", FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR);
    c_enum::defineValue(obj1, obj2, "UNSUPPORTED", FLAC__STREAM_DECODER_LENGTH_STATUS_UNSUPPORTED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamDecoder::createWriteStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "CONTINUE", FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE);
    c_enum::defineValue(obj1, obj2, "ABORT", FLAC__STREAM_DECODER_WRITE_STATUS_ABORT);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamDecoder::createErrorStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "LOST_SYNC", FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC);
    c_enum::defineValue(obj1, obj2, "BAD_HEADER", FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER);
    c_enum::defineValue(
      obj1,
      obj2,
      "FRAME_CRC_MISMATCH",
      FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH);
    c_enum::defineValue(
      obj1,
      obj2,
      "UNPARSEABLE_STREAM",
      FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM);
#if FLAC_API_VERSION_CURRENT >= 12
    c_enum::defineValue(obj1, obj2, "BAD_METADATA", FLAC__STREAM_DECODER_ERROR_STATUS_BAD_METADATA);
#endif
    return std::make_tuple(obj1, obj2);
  }

  // -- helpers --

  Promise StreamDecoder::enqueueWork(AsyncDecoderWorkBase* work) {
    return ctx->runLocked<Promise>([this, work]() {
      ctx->workInProgress = true;
      work->Queue();
      return work->getPromise();
    });
  }

  // -- C callbacks --

  FLAC__StreamDecoderReadStatus StreamDecoder::readCallback(
    const FLAC__StreamDecoder*,
    FLAC__byte buffer[],
    size_t* bytes,
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto returnValue = FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    auto env = ctx->readCbk.Env();
    HandleScope scope(env);
    Buffer<FLAC__byte> jsBuffer;
    try {
      jsBuffer = pointer::wrap(env, buffer, *bytes);
      auto ret = ctx->readCbk.MakeCallback(env.Global(), {jsBuffer});
      generateParseObjectResult(returnValue, "Decoder:ReadCallback", "bytes", *bytes)(ret);
      pointer::detach(jsBuffer);
    } catch (const Error& error) {
      pointer::detach(jsBuffer);
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__StreamDecoderSeekStatus
    StreamDecoder::seekCallback(const FLAC__StreamDecoder*, uint64_t offset, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto returnValue = FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    auto env = ctx->seekCbk.Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, offset)});
      generateParseNumberResult(returnValue, "Decoder:SeekCallback")(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__StreamDecoderTellStatus
    StreamDecoder::tellCallback(const FLAC__StreamDecoder*, uint64_t* offset, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto returnValue = FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
    auto env = ctx->tellCbk.Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->tellCbk.MakeCallback(env.Global(), {numberToJs(env, *offset)});
      generateParseObjectResult(returnValue, "Decoder:TellCallback", "offset", *offset)(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__StreamDecoderLengthStatus
    StreamDecoder::lengthCallback(const FLAC__StreamDecoder*, uint64_t* length, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto returnValue = FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
    auto env = ctx->lengthCbk.Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->lengthCbk.MakeCallback(env.Global(), {numberToJs(env, *length)});
      generateParseObjectResult(returnValue, "Decoder:LengthCallback", "length", *length)(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__bool StreamDecoder::eofCallback(const FLAC__StreamDecoder*, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto returnValue = (FLAC__bool) true;
    auto env = ctx->eofCbk.Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->eofCbk.MakeCallback(env.Global(), {});
      generateParseBooleanResult(returnValue, "Decoder:EofCallback")(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__StreamDecoderWriteStatus StreamDecoder::writeCallback(
    const FLAC__StreamDecoder* dec,
    const FLAC__Frame* frame,
    const int32_t* const samples[],
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto returnValue = FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    auto env = ctx->writeCbk.Env();
    HandleScope scope(env);
    auto buffers = Array::New(env);
    try {
      uint32_t channels = FLAC__stream_decoder_get_channels(const_cast<FLAC__StreamDecoder*>(dec));
      for (uint32_t ch = 0; ch < channels; ch += 1) {
        auto buffer =
          pointer::wrap(env, const_cast<int32_t*>(samples[ch]), frame->header.blocksize);
        buffers[ch] = buffer;
      }

      auto ret = ctx->writeCbk.MakeCallback(env.Global(), {frameToJs(env, frame), buffers});
      generateParseNumberResult(returnValue, "Decoder:WriteCallback")(ret);

      for (uint32_t ch = 0; ch < channels; ch += 1) {
        pointer::detach(((Napi::Value) buffers[ch]).As<Buffer<int32_t>>());
      }
    } catch (const Error& error) {
      for (uint32_t ch = 0; ch < buffers.Length(); ch += 1) {
        pointer::detach(((Napi::Value) buffers[ch]).As<Buffer<int32_t>>());
      }
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  void StreamDecoder::metadataCallback(
    const FLAC__StreamDecoder*,
    const FLAC__StreamMetadata* metadata,
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto env = ctx->metadataCbk.Env();
    HandleScope scope(env);
    try {
      auto jsMetadata = Metadata::toJs(env, const_cast<FLAC__StreamMetadata*>(metadata));
      ctx->metadataCbk.MakeCallback(env.Global(), {jsMetadata});
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }
  }

  void StreamDecoder::errorCallback(
    const FLAC__StreamDecoder*,
    FLAC__StreamDecoderErrorStatus errorCode,
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    auto env = ctx->errorCbk.Env();
    HandleScope scope(env);
    try {
      ctx->metadataCbk.MakeCallback(env.Global(), {numberToJs(env, errorCode)});
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }
  }

}
