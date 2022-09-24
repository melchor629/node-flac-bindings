#include "encoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"

#define DEFER_SYNCHRONIZED(f) DEFER(runLocked([&]() { f; }));

namespace flac_bindings {

  static std::vector<int32_t*> getBuffersFromArray(const Napi::Value&, unsigned, unsigned);
  static int32_t* getInterleavedBufferFromArgs(const CallbackInfo&, unsigned, unsigned&);

  Function StreamEncoder::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto attrs = napi_property_attributes::napi_enumerable;
    auto constructor = DefineClass(
      env,
      "StreamEncoder",
      {
        InstanceAccessor("verify", &StreamEncoder::getVerify, nullptr, attrs),
        InstanceAccessor("streamableSubset", &StreamEncoder::getStreamableSubset, nullptr, attrs),
        InstanceAccessor("channels", &StreamEncoder::getChannels, nullptr, attrs),
        InstanceAccessor("bitsPerSample", &StreamEncoder::getBitsPerSample, nullptr, attrs),
        InstanceAccessor("sampleRate", &StreamEncoder::getSampleRate, nullptr, attrs),
        InstanceAccessor("blocksize", &StreamEncoder::getBlocksize, nullptr, attrs),
        InstanceAccessor("doMidSideStereo", &StreamEncoder::getDoMidSideStereo, nullptr, attrs),
        InstanceAccessor(
          "looseMidSideStereo",
          &StreamEncoder::getLooseMidSideStereo,
          nullptr,
          attrs),
        InstanceAccessor("maxLpcOrder", &StreamEncoder::getMaxLpcOrder, nullptr, attrs),
        InstanceAccessor("qlpCoeffPrecision", &StreamEncoder::getQlpCoeffPrecision, nullptr, attrs),
        InstanceAccessor(
          "doQlpCoeffPrecSearch",
          &StreamEncoder::getDoQlpCoeffPrecSearch,
          nullptr,
          attrs),
        InstanceAccessor("doEscapeCoding", &StreamEncoder::getDoEscapeCoding, nullptr, attrs),
        InstanceAccessor(
          "doExhaustiveModelSearch",
          &StreamEncoder::getDoExhaustiveModelSearch,
          nullptr,
          attrs),
        InstanceAccessor(
          "maxResidualPartitionOrder",
          &StreamEncoder::getMaxResidualPartitionOrder,
          nullptr,
          attrs),
        InstanceAccessor(
          "minResidualPartitionOrder",
          &StreamEncoder::getMinResidualPartitionOrder,
          nullptr,
          attrs),
        InstanceAccessor(
          "riceParameterSearchDist",
          &StreamEncoder::getRiceParameterSearchDist,
          nullptr,
          attrs),
        InstanceAccessor(
          "totalSamplesEstimate",
          &StreamEncoder::getTotalSamplesEstimate,
          nullptr,
          attrs),
        InstanceAccessor("limitMinBitrate", &StreamEncoder::getLimitMinBitrate, nullptr, attrs),
        InstanceMethod("getState", &StreamEncoder::getState),
        InstanceMethod("getVerifyDecoderState", &StreamEncoder::getVerifyDecoderState),
        InstanceMethod("getResolvedStateString", &StreamEncoder::getResolvedStateString),
        InstanceMethod("getVerifyDecoderErrorStats", &StreamEncoder::getVerifyDecoderErrorStats),

        InstanceMethod("finish", &StreamEncoder::finish),
        InstanceMethod("process", &StreamEncoder::process),
        InstanceMethod("processInterleaved", &StreamEncoder::processInterleaved),

        InstanceMethod("finishAsync", &StreamEncoder::finishAsync),
        InstanceMethod("processAsync", &StreamEncoder::processAsync),
        InstanceMethod("processInterleavedAsync", &StreamEncoder::processInterleavedAsync),
      });
    c_enum::declareInObject(constructor, "State", createStateEnum);
    c_enum::declareInObject(constructor, "InitStatus", createInitStatusEnum);
    c_enum::declareInObject(constructor, "ReadStatus", createReadStatusEnum);
    c_enum::declareInObject(constructor, "WriteStatus", createWriteStatusEnum);
    c_enum::declareInObject(constructor, "SeekStatus", createSeekStatusEnum);
    c_enum::declareInObject(constructor, "TellStatus", createTellStatusEnum);

    addon.encoderConstructor = Persistent(constructor);

    return scope.Escape(objectFreeze(constructor)).As<Function>();
  }

  StreamEncoder::StreamEncoder(const CallbackInfo& info): ObjectWrap<StreamEncoder>(info) {
    auto addon = info.Env().GetInstanceData<FlacAddon>();
    auto checks = info.Length() != 2 || !info[0].IsObject()
                  || !(info[1].IsNull() || info[1].IsArray())
                  || !info[0].As<Object>().InstanceOf(addon->encoderBuilderConstructor.Value());
    if (checks) {
      throw Error::New(
        info.Env(),
        "StreamEncoder constructor cannot be called directly, use StreamEncoderBuilder instead");
    }

    info.This().As<Object>().Set("__metadataArrayRef", info[1]);
    builder = Napi::Persistent(info[0].As<Object>());
  }

  StreamEncoder::~StreamEncoder() {
    if (enc != nullptr) {
      FLAC__stream_encoder_delete(enc);
    }

    builder.Unref();
  }

  // -- getters --

  Napi::Value StreamEncoder::getVerify(const CallbackInfo& info) {
    auto verify = FLAC__stream_encoder_get_verify(enc);
    return booleanToJs(info.Env(), verify);
  }

  Napi::Value StreamEncoder::getStreamableSubset(const CallbackInfo& info) {
    auto streamableSubset = FLAC__stream_encoder_get_streamable_subset(enc);
    return booleanToJs(info.Env(), streamableSubset);
  }

  Napi::Value StreamEncoder::getChannels(const CallbackInfo& info) {
    auto channels = FLAC__stream_encoder_get_channels(enc);
    return numberToJs(info.Env(), channels);
  }

  Napi::Value StreamEncoder::getBitsPerSample(const CallbackInfo& info) {
    auto bitsPerSample = FLAC__stream_encoder_get_bits_per_sample(enc);
    return numberToJs(info.Env(), bitsPerSample);
  }

  Napi::Value StreamEncoder::getSampleRate(const CallbackInfo& info) {
    auto sampleRate = FLAC__stream_encoder_get_sample_rate(enc);
    return numberToJs(info.Env(), sampleRate);
  }

  Napi::Value StreamEncoder::getBlocksize(const CallbackInfo& info) {
    auto blocksize = FLAC__stream_encoder_get_blocksize(enc);
    return numberToJs(info.Env(), blocksize);
  }

  Napi::Value StreamEncoder::getDoMidSideStereo(const CallbackInfo& info) {
    auto doMidStereo = FLAC__stream_encoder_get_do_mid_side_stereo(enc);
    return booleanToJs(info.Env(), doMidStereo);
  }

  Napi::Value StreamEncoder::getLooseMidSideStereo(const CallbackInfo& info) {
    auto looseMidStereo = FLAC__stream_encoder_get_loose_mid_side_stereo(enc);
    return booleanToJs(info.Env(), looseMidStereo);
  }

  Napi::Value StreamEncoder::getMaxLpcOrder(const CallbackInfo& info) {
    auto maxLpcOrder = FLAC__stream_encoder_get_max_lpc_order(enc);
    return numberToJs(info.Env(), maxLpcOrder);
  }

  Napi::Value StreamEncoder::getQlpCoeffPrecision(const CallbackInfo& info) {
    auto qlpCoeffPrecision = FLAC__stream_encoder_get_qlp_coeff_precision(enc);
    return numberToJs(info.Env(), qlpCoeffPrecision);
  }

  Napi::Value StreamEncoder::getDoQlpCoeffPrecSearch(const CallbackInfo& info) {
    auto doQlpCoeffPrecSearch = FLAC__stream_encoder_get_do_qlp_coeff_prec_search(enc);
    return booleanToJs(info.Env(), doQlpCoeffPrecSearch);
  }

  Napi::Value StreamEncoder::getDoEscapeCoding(const CallbackInfo& info) {
    auto doEscapeCoding = FLAC__stream_encoder_get_do_escape_coding(enc);
    return booleanToJs(info.Env(), doEscapeCoding);
  }

  Napi::Value StreamEncoder::getDoExhaustiveModelSearch(const CallbackInfo& info) {
    auto doExhaustiveModelSearch = FLAC__stream_encoder_get_do_exhaustive_model_search(enc);
    return booleanToJs(info.Env(), doExhaustiveModelSearch);
  }

  Napi::Value StreamEncoder::getMinResidualPartitionOrder(const CallbackInfo& info) {
    auto minResidualPartitionOrder = FLAC__stream_encoder_get_min_residual_partition_order(enc);
    return numberToJs(info.Env(), minResidualPartitionOrder);
  }

  Napi::Value StreamEncoder::getMaxResidualPartitionOrder(const CallbackInfo& info) {
    auto maxResidualPartitionOrder = FLAC__stream_encoder_get_max_residual_partition_order(enc);
    return numberToJs(info.Env(), maxResidualPartitionOrder);
  }

  Napi::Value StreamEncoder::getRiceParameterSearchDist(const CallbackInfo& info) {
    auto riceParameterSearchDist = FLAC__stream_encoder_get_rice_parameter_search_dist(enc);
    return numberToJs(info.Env(), riceParameterSearchDist);
  }

  Napi::Value StreamEncoder::getLimitMinBitrate(const CallbackInfo& info) {
#if FLAC_API_VERSION_CURRENT >= 12
    auto value = FLAC__stream_encoder_get_limit_min_bitrate(enc);
    return booleanToJs(info.Env(), value);
#else
    return booleanToJs(info.Env(), false);
#endif
  }

  Napi::Value StreamEncoder::getTotalSamplesEstimate(const CallbackInfo& info) {
    auto totalSamplesEstimate = FLAC__stream_encoder_get_total_samples_estimate(enc);
    return numberToJs(info.Env(), totalSamplesEstimate);
  }

  // -- state getters --

  Napi::Value StreamEncoder::getState(const CallbackInfo& info) {
    auto state = FLAC__stream_encoder_get_state(enc);
    return numberToJs(info.Env(), state);
  }

  Napi::Value StreamEncoder::getVerifyDecoderState(const CallbackInfo& info) {
    if (FLAC__stream_encoder_get_state(enc) == FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR) {
      auto verifyDecoderState = FLAC__stream_encoder_get_verify_decoder_state(enc);
      return numberToJs(info.Env(), verifyDecoderState);
    }

    return numberToJs(info.Env(), 0);
  }

  Napi::Value StreamEncoder::getResolvedStateString(const CallbackInfo& info) {
    auto stateString = FLAC__stream_encoder_get_resolved_state_string(enc);
    return String::New(info.Env(), stateString);
  }

  Napi::Value StreamEncoder::getVerifyDecoderErrorStats(const CallbackInfo& info) {
    if (FLAC__stream_encoder_get_state(enc) == FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR) {
      uint64_t absolute_sample;
      unsigned frame_number;
      unsigned channel;
      unsigned sample;
      int32_t expected;
      int32_t got;
      FLAC__stream_encoder_get_verify_decoder_error_stats(
        enc,
        &absolute_sample,
        &frame_number,
        &channel,
        &sample,
        &expected,
        &got);

      Object obj = Object::New(info.Env());
      auto attrs = napi_property_attributes::napi_enumerable;
      obj.DefineProperties({
        Napi::PropertyDescriptor::Value(
          "absoluteSample",
          numberToJs(info.Env(), absolute_sample),
          attrs),
        Napi::PropertyDescriptor::Value("frameNumber", numberToJs(info.Env(), frame_number), attrs),
        Napi::PropertyDescriptor::Value("channel", numberToJs(info.Env(), channel), attrs),
        Napi::PropertyDescriptor::Value("sample", numberToJs(info.Env(), sample), attrs),
        Napi::PropertyDescriptor::Value("expected", numberToJs(info.Env(), expected), attrs),
        Napi::PropertyDescriptor::Value("got", numberToJs(info.Env(), got), attrs),
      });
      return objectFreeze(obj);
    }

    return info.Env().Null();
  }

  // -- operations --

  Napi::Value StreamEncoder::finish(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), EncoderWorkContext::ExecutionMode::Sync);

    auto ret = FLAC__stream_encoder_finish(enc);
    if (info.Env().IsExceptionPending()) {
      return Napi::Value();
    }

    if (ret) {
      EscapableHandleScope scope(info.Env());
      auto builder = StreamEncoderBuilder::Unwrap(this->builder.Value());
      builder->enc = enc;
      enc = nullptr;
      return scope.Escape(this->builder.Value());
    }

    return info.Env().Null();
  }

  Napi::Value StreamEncoder::process(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), EncoderWorkContext::ExecutionMode::Sync);

    auto samples = numberFromJs<unsigned>(info[1]);
    auto channels = FLAC__stream_encoder_get_channels(enc);
    auto buffers = getBuffersFromArray(info[0], samples, channels);

    auto ret = FLAC__stream_encoder_process(enc, buffers.data(), samples);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamEncoder::processInterleaved(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), EncoderWorkContext::ExecutionMode::Sync);

    auto channels = FLAC__stream_encoder_get_channels(enc);
    unsigned samples;
    int32_t* buffer = getInterleavedBufferFromArgs(info, channels, samples);

    auto ret = FLAC__stream_encoder_process_interleaved(enc, buffer, samples);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamEncoder::finishAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), EncoderWorkContext::ExecutionMode::Async);

    AsyncEncoderWork* work = AsyncEncoderWork::forFinish({info.This()}, *this);
    return enqueueWork(work);
  }

  Napi::Value StreamEncoder::processAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), EncoderWorkContext::ExecutionMode::Async);

    auto samples = numberFromJs<unsigned>(info[1]);
    auto channels = FLAC__stream_encoder_get_channels(enc);
    auto buffers = getBuffersFromArray(info[0], samples, channels);

    AsyncEncoderWork* work =
      AsyncEncoderWork::forProcess({info.This(), info[0]}, buffers, samples, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamEncoder::processInterleavedAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env(), EncoderWorkContext::ExecutionMode::Async);

    auto channels = FLAC__stream_encoder_get_channels(enc);
    unsigned samples;
    int32_t* buffer = getInterleavedBufferFromArgs(info, channels, samples);

    AsyncEncoderWork* work =
      AsyncEncoderWork::forProcessInterleaved({info.This(), info[0]}, buffer, samples, ctx.get());
    return enqueueWork(work);
  }

  // -- enums --

  c_enum::DefineReturnType StreamEncoder::createStateEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", 0);
    c_enum::defineValue(obj1, obj2, "UNINITIALIZED", FLAC__STREAM_ENCODER_UNINITIALIZED);
    c_enum::defineValue(obj1, obj2, "OGG_ERROR", FLAC__STREAM_ENCODER_OGG_ERROR);
    c_enum::defineValue(
      obj1,
      obj2,
      "VERIFY_DECODER_ERROR",
      FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR);
    c_enum::defineValue(
      obj1,
      obj2,
      "VERIFY_MISMATCH_IN_AUDIO_DATA",
      FLAC__STREAM_ENCODER_VERIFY_MISMATCH_IN_AUDIO_DATA);
    c_enum::defineValue(obj1, obj2, "CLIENT_ERROR", FLAC__STREAM_ENCODER_CLIENT_ERROR);
    c_enum::defineValue(obj1, obj2, "IO_ERROR", FLAC__STREAM_ENCODER_IO_ERROR);
    c_enum::defineValue(obj1, obj2, "FRAMING_ERROR", FLAC__STREAM_ENCODER_FRAMING_ERROR);
    c_enum::defineValue(
      obj1,
      obj2,
      "MEMORY_ALLOCATION_ERROR",
      FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamEncoder::createInitStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_ENCODER_INIT_STATUS_OK);
    c_enum::defineValue(
      obj1,
      obj2,
      "ENCODER_ERROR",
      FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR);
    c_enum::defineValue(
      obj1,
      obj2,
      "UNSUPPORTED_CONTAINER",
      FLAC__STREAM_ENCODER_INIT_STATUS_UNSUPPORTED_CONTAINER);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_CALLBACKS",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_CALLBACKS);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_NUMBER_OF_CHANNELS",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_NUMBER_OF_CHANNELS);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_BITS_PER_SAMPLE",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BITS_PER_SAMPLE);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_SAMPLE_RATE",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_SAMPLE_RATE);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_BLOCK_SIZE",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BLOCK_SIZE);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_MAX_LPC_ORDER",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_MAX_LPC_ORDER);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_QLP_COEFF_PRECISION",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_QLP_COEFF_PRECISION);
    c_enum::defineValue(
      obj1,
      obj2,
      "BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER",
      FLAC__STREAM_ENCODER_INIT_STATUS_BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER);
    c_enum::defineValue(
      obj1,
      obj2,
      "NOT_STREAMABLE",
      FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE);
    c_enum::defineValue(
      obj1,
      obj2,
      "INVALID_METADATA",
      FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA);
    c_enum::defineValue(
      obj1,
      obj2,
      "ALREADY_INITIALIZED",
      FLAC__STREAM_ENCODER_INIT_STATUS_ALREADY_INITIALIZED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamEncoder::createReadStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "CONTINUE", FLAC__STREAM_ENCODER_READ_STATUS_CONTINUE);
    c_enum::defineValue(
      obj1,
      obj2,
      "END_OF_STREAM",
      FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM);
    c_enum::defineValue(obj1, obj2, "ABORT", FLAC__STREAM_ENCODER_READ_STATUS_ABORT);
    c_enum::defineValue(obj1, obj2, "UNSUPPORTED", FLAC__STREAM_ENCODER_READ_STATUS_UNSUPPORTED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamEncoder::createWriteStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_ENCODER_WRITE_STATUS_OK);
    c_enum::defineValue(obj1, obj2, "FATAL_ERROR", FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamEncoder::createSeekStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_ENCODER_SEEK_STATUS_OK);
    c_enum::defineValue(obj1, obj2, "ERROR", FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR);
    c_enum::defineValue(obj1, obj2, "UNSUPPORTED", FLAC__STREAM_ENCODER_SEEK_STATUS_UNSUPPORTED);
    return std::make_tuple(obj1, obj2);
  }

  c_enum::DefineReturnType StreamEncoder::createTellStatusEnum(const Napi::Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_ENCODER_TELL_STATUS_OK);
    c_enum::defineValue(obj1, obj2, "ERROR", FLAC__STREAM_ENCODER_TELL_STATUS_ERROR);
    c_enum::defineValue(obj1, obj2, "UNSUPPORTED", FLAC__STREAM_ENCODER_TELL_STATUS_UNSUPPORTED);
    return std::make_tuple(obj1, obj2);
  }

  // -- C callbacks --

  FLAC__StreamEncoderReadStatus StreamEncoder::readCallback(
    const FLAC__StreamEncoder*,
    FLAC__byte buffer[],
    size_t* bytes,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;
    auto env = ctx->readCbk.Env();
    auto returnValue = FLAC__STREAM_ENCODER_READ_STATUS_ABORT;
    HandleScope scope(env);
    Buffer<FLAC__byte> jsBuffer;
    try {
      jsBuffer = pointer::wrap(env, buffer, *bytes);
      auto ret = ctx->readCbk.MakeCallback(env.Global(), {jsBuffer});
      generateParseObjectResult(returnValue, "Encoder:ReadCallback", "bytes", *bytes)(ret);
      pointer::detach(jsBuffer);
    } catch (const Error& error) {
      *bytes = 0;
      pointer::detach(jsBuffer);
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__StreamEncoderWriteStatus StreamEncoder::writeCallback(
    const FLAC__StreamEncoder*,
    const FLAC__byte buffer[],
    size_t bytes,
    unsigned samples,
    unsigned frame,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;
    auto env = ctx->writeCbk.Env();
    auto returnValue = FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
    HandleScope scope(env);
    Buffer<FLAC__byte> jsBuffer;
    try {
      jsBuffer = pointer::wrap(env, const_cast<FLAC__byte*>(buffer), bytes);
      auto ret = ctx->writeCbk.MakeCallback(
        env.Global(),
        {jsBuffer, numberToJs(env, samples), numberToJs(env, frame)});
      generateParseNumberResult(returnValue, "Encoder:WriteCallback")(ret);
      pointer::detach(jsBuffer);
    } catch (const Error& error) {
      pointer::detach(jsBuffer);
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__StreamEncoderSeekStatus
    StreamEncoder::seekCallback(const FLAC__StreamEncoder*, uint64_t offset, void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;
    auto env = ctx->seekCbk.Env();
    auto returnValue = FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
    HandleScope scope(env);
    try {
      auto ret = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, offset)});
      generateParseNumberResult(returnValue, "Encoder:SeekCallback")(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  FLAC__StreamEncoderTellStatus
    StreamEncoder::tellCallback(const FLAC__StreamEncoder*, uint64_t* offset, void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;
    auto env = ctx->tellCbk.Env();
    auto returnValue = FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;
    HandleScope scope(env);
    try {
      auto ret = ctx->tellCbk.MakeCallback(env.Global(), {numberToJs(env, *offset)});
      generateParseObjectResult(returnValue, "Encoder:TellCallback", "offset", *offset)(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  void StreamEncoder::metadataCallback(
    const FLAC__StreamEncoder*,
    const FLAC__StreamMetadata* metadata,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;
    auto env = ctx->metadataCbk.Env();
    HandleScope scope(env);
    try {
      auto jsMetadata = Metadata::toJs(env, const_cast<FLAC__StreamMetadata*>(metadata));
      ctx->metadataCbk.MakeCallback(env.Global(), {jsMetadata});
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }
  }

  void StreamEncoder::progressCallback(
    const FLAC__StreamEncoder*,
    uint64_t bytesWritten,
    uint64_t samplesWritten,
    unsigned framesWritten,
    unsigned totalFramesEstimate,
    void* ptr) {
    auto ctx = (EncoderWorkContext*) ptr;
    auto env = ctx->progressCbk.Env();
    HandleScope scope(env);
    try {
      auto args = std::vector<napi_value> {
        numberToJs(env, bytesWritten),
        numberToJs(env, samplesWritten),
        numberToJs(env, framesWritten),
        numberToJs(env, totalFramesEstimate),
      };
      ctx->progressCbk.MakeCallback(env.Global(), args);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }
  }

  // -- helpers --

  Promise StreamEncoder::enqueueWork(AsyncEncoderWorkBase* work) {
    return ctx->runLocked<Promise>([this, work]() {
      ctx->workInProgress = true;
      work->Queue();
      return work->getPromise();
    });
  }

  static std::vector<int32_t*>
    getBuffersFromArray(const Napi::Value& value, unsigned samples, unsigned channels) {
    auto bufferSizeTuples =
      arrayFromJs<std::tuple<int32_t*, size_t>>(value, pointer::fromBuffer<int32_t>);
    if (bufferSizeTuples.size() < channels) {
      throw RangeError::New(
        value.Env(),
        "Expected array to have "s + std::to_string(channels) + " buffers (one for each channel)"s);
    }

    std::vector<int32_t*> buffers;
    for (auto it = bufferSizeTuples.begin(); it != bufferSizeTuples.end(); it += 1) {
      int32_t* buffer;
      size_t size;
      std::tie(buffer, size) = *it;
      if (size < samples) {
        auto errorMessage =
          "Buffer at position "s + std::to_string(it - bufferSizeTuples.begin())
          + " has not enough bytes: expected "s + std::to_string(samples * sizeof(int32_t))
          + " bytes ("s + std::to_string(samples) + " samples * "s + std::to_string(sizeof(int32_t))
          + " bytes per sample) but got "s + std::to_string(size) + " bytes"s;
        throw RangeError::New(value.Env(), errorMessage);
      }

      buffers.push_back(buffer);
    }

    return buffers;
  }

  static int32_t*
    getInterleavedBufferFromArgs(const CallbackInfo& info, unsigned channels, unsigned& samples) {
    int32_t* buffer;
    size_t size;
    std::tie(buffer, size) = pointer::fromBuffer<int32_t>(info[0]);
    samples = maybeNumberFromJs<unsigned>(info[1]).value_or(size / channels);
    size_t readSize = (size_t) samples * channels;
    if (size < readSize) {
      auto errorMessage = "Buffer has not enough bytes: expected "s
                          + std::to_string(readSize * sizeof(int32_t)) + " bytes ("s
                          + std::to_string(samples) + " samples * "s + std::to_string(channels)
                          + " channels * "s + std::to_string(sizeof(int32_t))
                          + " bytes per sample) but got "s + std::to_string(size) + " bytes"s;
      throw RangeError::New(info.Env(), errorMessage);
    }

    return buffer;
  }

}
