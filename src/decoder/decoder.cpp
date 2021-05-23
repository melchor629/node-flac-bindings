#include "decoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"
#include <memory>

#define DEFER_SYNCHRONIZED(f) DEFER(runLocked([&]() { f; }));

namespace flac_bindings {

  Function StreamDecoder::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto constructor = DefineClass(
      env,
      "StreamDecoder",
      {
        InstanceMethod("setOggSerialNumber", &StreamDecoder::setOggSerialNumber),
        InstanceMethod("getMd5Checking", &StreamDecoder::getMd5Checking),
        InstanceMethod("setMd5Checking", &StreamDecoder::setMd5Checking),
        InstanceMethod("setMetadataRespond", &StreamDecoder::setMetadataRespond),
        InstanceMethod("setMetadataIgnore", &StreamDecoder::setMetadataIgnore),
        InstanceMethod("getTotalSamples", &StreamDecoder::getTotalSamples),
        InstanceMethod("getChannels", &StreamDecoder::getChannels),
        InstanceMethod("getChannelAssignment", &StreamDecoder::getChannelAssignment),
        InstanceMethod("getBitsPerSample", &StreamDecoder::getBitsPerSample),
        InstanceMethod("getSampleRate", &StreamDecoder::getSampleRate),
        InstanceMethod("getBlocksize", &StreamDecoder::getBlocksize),

        InstanceMethod("initStream", &StreamDecoder::initStream),
        InstanceMethod("initOggStream", &StreamDecoder::initOggStream),
        InstanceMethod("initFile", &StreamDecoder::initFile),
        InstanceMethod("initOggFile", &StreamDecoder::initOggFile),
        InstanceMethod("finish", &StreamDecoder::finish),
        InstanceMethod("flush", &StreamDecoder::flush),
        InstanceMethod("reset", &StreamDecoder::reset),
        InstanceMethod("processSingle", &StreamDecoder::processSingle),
        InstanceMethod("processUntilEndOfMetadata", &StreamDecoder::processUntilEndOfMetadata),
        InstanceMethod("processUntilEndOfStream", &StreamDecoder::processUntilEndOfStream),
        InstanceMethod("skipSingleFrame", &StreamDecoder::skipSingleFrame),
        InstanceMethod("seekAbsolute", &StreamDecoder::seekAbsolute),
        InstanceMethod(
          "setMetadataRespondApplication",
          &StreamDecoder::setMetadataRespondApplication),
        InstanceMethod("setMetadataRespondAll", &StreamDecoder::setMetadataRespondAll),
        InstanceMethod(
          "setMetadataIgnoreApplication",
          &StreamDecoder::setMetadataIgnoreApplication),
        InstanceMethod("setMetadataIgnoreAll", &StreamDecoder::setMetadataIgnoreAll),
        InstanceMethod("getState", &StreamDecoder::getState),
        InstanceMethod("getResolvedStateString", &StreamDecoder::getResolvedStateString),
        InstanceMethod("getDecodePosition", &StreamDecoder::getDecodePosition),

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
        InstanceMethod("initStreamAsync", &StreamDecoder::initStreamAsync),
        InstanceMethod("initOggStreamAsync", &StreamDecoder::initOggStreamAsync),
        InstanceMethod("initFileAsync", &StreamDecoder::initFileAsync),
        InstanceMethod("initOggFileAsync", &StreamDecoder::initOggFileAsync),
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
    dec = FLAC__stream_decoder_new();
    if (dec == nullptr) {
      throw Error::New(info.Env(), "Could not allocate memory");
    }
  }

  StreamDecoder::~StreamDecoder() {
    FLAC__stream_decoder_delete(dec);
  }

  void StreamDecoder::setOggSerialNumber(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());
    auto oggSerialNumber = numberFromJs<long>(info[0]);
    FLAC__stream_decoder_set_ogg_serial_number(dec, oggSerialNumber);
  }

  Napi::Value StreamDecoder::getMd5Checking(const CallbackInfo& info) {
    auto md5Checking = FLAC__stream_decoder_get_md5_checking(dec);
    return booleanToJs(info.Env(), md5Checking);
  }

  void StreamDecoder::setMd5Checking(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());
    auto md5Checking = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_decoder_set_md5_checking(dec, md5Checking);
  }

  void StreamDecoder::setMetadataRespond(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());
    auto metadataType = numberFromJs<FLAC__MetadataType>(info[0]);
    FLAC__stream_decoder_set_metadata_respond(dec, metadataType);
  }

  void StreamDecoder::setMetadataIgnore(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());
    auto metadataType = numberFromJs<FLAC__MetadataType>(info[0]);
    FLAC__stream_decoder_set_metadata_ignore(dec, metadataType);
  }

  Napi::Value StreamDecoder::getTotalSamples(const CallbackInfo& info) {
    checkIsInitialized(info.Env());
    auto totalSamples = FLAC__stream_decoder_get_total_samples(dec);
    return numberToJs(info.Env(), totalSamples);
  }

  Napi::Value StreamDecoder::getChannels(const CallbackInfo& info) {
    checkIsInitialized(info.Env());
    auto channels = FLAC__stream_decoder_get_channels(dec);
    return numberToJs(info.Env(), channels);
  }

  Napi::Value StreamDecoder::getChannelAssignment(const CallbackInfo& info) {
    checkIsInitialized(info.Env());
    auto channelAssignment = FLAC__stream_decoder_get_channel_assignment(dec);
    return numberToJs(info.Env(), channelAssignment);
  }

  Napi::Value StreamDecoder::getBitsPerSample(const CallbackInfo& info) {
    checkIsInitialized(info.Env());
    auto bitsPerSample = FLAC__stream_decoder_get_bits_per_sample(dec);
    return numberToJs(info.Env(), bitsPerSample);
  }

  Napi::Value StreamDecoder::getSampleRate(const CallbackInfo& info) {
    checkIsInitialized(info.Env());
    auto sampleRate = FLAC__stream_decoder_get_sample_rate(dec);
    return numberToJs(info.Env(), sampleRate);
  }

  Napi::Value StreamDecoder::getBlocksize(const CallbackInfo& info) {
    checkIsInitialized(info.Env());
    auto blocksize = FLAC__stream_decoder_get_blocksize(dec);
    return numberToJs(info.Env(), blocksize);
  }

  void StreamDecoder::initStream(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->lengthCbk, info[3]);
    maybeFunctionIntoRef(ctx->eofCbk, info[4]);
    maybeFunctionIntoRef(ctx->writeCbk, info[5]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[6]);
    maybeFunctionIntoRef(ctx->errorCbk, info[7]);

    auto ret = FLAC__stream_decoder_init_stream(
      dec,
      !ctx->readCbk.IsEmpty() ? StreamDecoder::readCallback : nullptr,
      !ctx->seekCbk.IsEmpty() ? StreamDecoder::seekCallback : nullptr,
      !ctx->tellCbk.IsEmpty() ? StreamDecoder::tellCallback : nullptr,
      !ctx->lengthCbk.IsEmpty() ? StreamDecoder::lengthCallback : nullptr,
      !ctx->eofCbk.IsEmpty() ? StreamDecoder::eofCallback : nullptr,
      !ctx->writeCbk.IsEmpty() ? StreamDecoder::writeCallback : nullptr,
      !ctx->metadataCbk.IsEmpty() ? StreamDecoder::metadataCallback : nullptr,
      !ctx->errorCbk.IsEmpty() ? StreamDecoder::errorCallback : nullptr,
      ctx.get());

    checkInitStatus(info.Env(), ret);
  }

  void StreamDecoder::initOggStream(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->lengthCbk, info[3]);
    maybeFunctionIntoRef(ctx->eofCbk, info[4]);
    maybeFunctionIntoRef(ctx->writeCbk, info[5]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[6]);
    maybeFunctionIntoRef(ctx->errorCbk, info[7]);

    auto ret = FLAC__stream_decoder_init_ogg_stream(
      dec,
      !ctx->readCbk.IsEmpty() ? StreamDecoder::readCallback : nullptr,
      !ctx->seekCbk.IsEmpty() ? StreamDecoder::seekCallback : nullptr,
      !ctx->tellCbk.IsEmpty() ? StreamDecoder::tellCallback : nullptr,
      !ctx->lengthCbk.IsEmpty() ? StreamDecoder::lengthCallback : nullptr,
      !ctx->eofCbk.IsEmpty() ? StreamDecoder::eofCallback : nullptr,
      !ctx->writeCbk.IsEmpty() ? StreamDecoder::writeCallback : nullptr,
      !ctx->metadataCbk.IsEmpty() ? StreamDecoder::metadataCallback : nullptr,
      !ctx->errorCbk.IsEmpty() ? StreamDecoder::errorCallback : nullptr,
      ctx.get());

    checkInitStatus(info.Env(), ret);
  }

  void StreamDecoder::initFile(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    auto path = stringFromJs(info[0]);
    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[2]);
    maybeFunctionIntoRef(ctx->errorCbk, info[3]);

    auto ret = FLAC__stream_decoder_init_file(
      dec,
      path.c_str(),
      !ctx->writeCbk.IsEmpty() ? StreamDecoder::writeCallback : nullptr,
      !ctx->metadataCbk.IsEmpty() ? StreamDecoder::metadataCallback : nullptr,
      !ctx->errorCbk.IsEmpty() ? StreamDecoder::errorCallback : nullptr,
      ctx.get());

    checkInitStatus(info.Env(), ret);
  }

  void StreamDecoder::initOggFile(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    auto path = stringFromJs(info[0]);
    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[2]);
    maybeFunctionIntoRef(ctx->errorCbk, info[3]);

    auto ret = FLAC__stream_decoder_init_ogg_file(
      dec,
      path.c_str(),
      !ctx->writeCbk.IsEmpty() ? StreamDecoder::writeCallback : nullptr,
      !ctx->metadataCbk.IsEmpty() ? StreamDecoder::metadataCallback : nullptr,
      !ctx->errorCbk.IsEmpty() ? StreamDecoder::errorCallback : nullptr,
      ctx.get());

    checkInitStatus(info.Env(), ret);
  }

  Napi::Value StreamDecoder::finish(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto ret = FLAC__stream_decoder_finish(dec);
    ctx = nullptr;
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::flush(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto ret = FLAC__stream_decoder_flush(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::reset(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto ret = FLAC__stream_decoder_reset(dec);
    ctx = nullptr;
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::processSingle(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto ret = FLAC__stream_decoder_process_single(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::processUntilEndOfMetadata(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto ret = FLAC__stream_decoder_process_until_end_of_metadata(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::processUntilEndOfStream(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto ret = FLAC__stream_decoder_process_until_end_of_stream(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::skipSingleFrame(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto ret = FLAC__stream_decoder_skip_single_frame(dec);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  Napi::Value StreamDecoder::seekAbsolute(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto offset = numberFromJs<uint64_t>(info[0]);
    auto ret = FLAC__stream_decoder_seek_absolute(dec, offset);
    return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
  }

  void StreamDecoder::setMetadataRespondApplication(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());

    FLAC__byte* id;
    size_t size;
    std::tie(id, size) = pointer::fromBuffer<FLAC__byte>(info[0]);

    if (size < 4) {
      throw Error::New(info.Env(), "ID buffer must be 4 bytes length");
    }

    FLAC__stream_decoder_set_metadata_respond_application(dec, id);
  }

  void StreamDecoder::setMetadataRespondAll(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());
    FLAC__stream_decoder_set_metadata_respond_all(dec);
  }

  void StreamDecoder::setMetadataIgnoreApplication(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());

    FLAC__byte* id;
    size_t size;
    std::tie(id, size) = pointer::fromBuffer<FLAC__byte>(info[0]);

    if (size < 4) {
      throw Error::New(info.Env(), "ID buffer must be 4 bytes length");
    }

    FLAC__stream_decoder_set_metadata_ignore_application(dec, id);
  }

  void StreamDecoder::setMetadataIgnoreAll(const CallbackInfo& info) {
    checkIsNotInitialized(info.Env());
    FLAC__stream_decoder_set_metadata_ignore_all(dec);
  }

  Napi::Value StreamDecoder::getState(const CallbackInfo& info) {
    auto state = FLAC__stream_decoder_get_state(dec);
    return numberToJs(info.Env(), state);
  }

  Napi::Value StreamDecoder::getResolvedStateString(const CallbackInfo& info) {
    auto stateString = FLAC__stream_decoder_get_resolved_state_string(dec);
    return String::New(info.Env(), stateString);
  }

  Napi::Value StreamDecoder::getDecodePosition(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    uint64_t pos;
    auto ret = FLAC__stream_decoder_get_decode_position(dec, &pos);
    if (ret && !info.Env().IsExceptionPending()) {
      return numberToJs(info.Env(), pos);
    }

    return info.Env().IsExceptionPending() ? Napi::Value() : info.Env().Null();
  }

  Napi::Value StreamDecoder::finishAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    AsyncDecoderWork* work = AsyncDecoderWork::forFinish(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::flushAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    AsyncDecoderWork* work = AsyncDecoderWork::forFlush(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::processSingleAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    AsyncDecoderWork* work =
      AsyncDecoderWork::forProcessSingle(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::processUntilEndOfMetadataAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    AsyncDecoderWork* work =
      AsyncDecoderWork::forProcessUntilEndOfMetadata(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::processUntilEndOfStreamAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    AsyncDecoderWork* work =
      AsyncDecoderWork::forProcessUntilEndOfStream(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::skipSingleFrameAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    AsyncDecoderWork* work =
      AsyncDecoderWork::forSkipSingleFrame(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::seekAbsoluteAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsInitialized(info.Env());

    auto sample = numberFromJs<uint64_t>(info[0]);
    AsyncDecoderWork* work =
      AsyncDecoderWork::forSeekAbsolute(info.This().As<Object>(), sample, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::initStreamAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->lengthCbk, info[3]);
    maybeFunctionIntoRef(ctx->eofCbk, info[4]);
    maybeFunctionIntoRef(ctx->writeCbk, info[5]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[6]);
    maybeFunctionIntoRef(ctx->errorCbk, info[7]);

    AsyncDecoderWork* work = AsyncDecoderWork::forInitStream(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::initOggStreamAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->lengthCbk, info[3]);
    maybeFunctionIntoRef(ctx->eofCbk, info[4]);
    maybeFunctionIntoRef(ctx->writeCbk, info[5]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[6]);
    maybeFunctionIntoRef(ctx->errorCbk, info[7]);

    AsyncDecoderWork* work =
      AsyncDecoderWork::forInitOggStream(info.This().As<Object>(), ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::initFileAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    auto path = stringFromJs(info[0]);
    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[2]);
    maybeFunctionIntoRef(ctx->errorCbk, info[3]);

    AsyncDecoderWork* work =
      AsyncDecoderWork::forInitFile(info.This().As<Object>(), path, ctx.get());
    return enqueueWork(work);
  }

  Napi::Value StreamDecoder::initOggFileAsync(const CallbackInfo& info) {
    checkPendingAsyncWork(info.Env());
    checkIsNotInitialized(info.Env());

    auto path = stringFromJs(info[0]);
    ctx = std::make_shared<DecoderWorkContext>(this);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[2]);
    maybeFunctionIntoRef(ctx->errorCbk, info[3]);

    AsyncDecoderWork* work =
      AsyncDecoderWork::forInitOggFile(info.This().As<Object>(), path, ctx.get());
    return enqueueWork(work);
  }

  void StreamDecoder::checkIsInitialized(const Napi::Env& env) {
    if (FLAC__stream_decoder_get_state(dec) == FLAC__STREAM_DECODER_UNINITIALIZED) {
      throw Error::New(env, "Decoder has not been initialized yet");
    }
  }

  void StreamDecoder::checkIsNotInitialized(const Napi::Env& env) {
    if (FLAC__stream_decoder_get_state(dec) != FLAC__STREAM_DECODER_UNINITIALIZED) {
      throw Error::New(env, "Decoder has been initialized already");
    }
  }

  void StreamDecoder::checkInitStatus(Napi::Env env, FLAC__StreamDecoderInitStatus status) {
    if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
      // remove prefix FLAC__STREAM_DECODER_INIT_STATUS_
      auto statusString = FLAC__StreamDecoderInitStatusString[status] + 33;
      auto error = Error::New(env, "Decoder initialization failed: "s + statusString);
      error.Set("status", numberToJs(env, status));
      error.Set("statusString", String::New(env, statusString));
      throw error;
    }
  }

  Promise StreamDecoder::enqueueWork(AsyncDecoderWorkBase* work) {
    std::lock_guard<std::mutex> lockGuard(this->mutex);
    work->Queue();
    busy = true;
    return work->getPromise();
  }

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
    return std::make_tuple(obj1, obj2);
  }

  template<typename EnumType>
  EnumType StreamDecoder::doAsyncWork(
    DecoderWorkContext* ctx,
    DecoderWorkRequest* req,
    EnumType defaultReturnValue) {
    req->returnValue = (int) defaultReturnValue;

    std::shared_ptr<DecoderWorkRequest> sharedReq(req);
    ctx->dec->asyncExecutionProgress->sendProgressAndWait(sharedReq);

    return (EnumType) req->returnValue;
  }

  constexpr FLAC__StreamDecoderReadStatus defaultReadCallbackReturnValue =
    FLAC__STREAM_DECODER_READ_STATUS_ABORT;
  FLAC__StreamDecoderReadStatus StreamDecoder::readCallback(
    const FLAC__StreamDecoder*,
    FLAC__byte buffer[],
    size_t* bytes,
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Read);
      req->buffer = buffer;
      req->bytes = bytes;
      return doAsyncWork(ctx, req, defaultReadCallbackReturnValue);
    }

    auto returnValue = defaultReadCallbackReturnValue;
    auto env = ctx->dec->Env();
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

  FLAC__StreamDecoderSeekStatus defaultSeekCallbackReturnValue =
    FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
  FLAC__StreamDecoderSeekStatus
    StreamDecoder::seekCallback(const FLAC__StreamDecoder*, uint64_t offset, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Seek);
      req->offset = &offset;
      return doAsyncWork(ctx, req, defaultSeekCallbackReturnValue);
    }

    auto returnValue = defaultSeekCallbackReturnValue;
    auto env = ctx->dec->Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, offset)});
      generateParseNumberResult(returnValue, "Decoder:SeekCallback")(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  constexpr FLAC__StreamDecoderTellStatus defaultTellCallbackReturnValue =
    FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
  FLAC__StreamDecoderTellStatus
    StreamDecoder::tellCallback(const FLAC__StreamDecoder*, uint64_t* offset, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Tell);
      req->offset = offset;
      return doAsyncWork(ctx, req, defaultTellCallbackReturnValue);
    }

    auto returnValue = defaultTellCallbackReturnValue;
    auto env = ctx->dec->Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->tellCbk.MakeCallback(env.Global(), {numberToJs(env, *offset)});
      generateParseObjectResult(returnValue, "Decoder:TellCallback", "offset", *offset)(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  constexpr FLAC__StreamDecoderLengthStatus defaultLengthCallbackReturnValue =
    FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
  FLAC__StreamDecoderLengthStatus
    StreamDecoder::lengthCallback(const FLAC__StreamDecoder*, uint64_t* length, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Length);
      req->offset = length;
      return doAsyncWork(ctx, req, defaultLengthCallbackReturnValue);
    }

    auto returnValue = defaultLengthCallbackReturnValue;
    auto env = ctx->dec->Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->lengthCbk.MakeCallback(env.Global(), {numberToJs(env, *length)});
      generateParseObjectResult(returnValue, "Decoder:LengthCallback", "length", *length)(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  constexpr FLAC__bool defaultEofCallbackReturnValue = true;
  FLAC__bool StreamDecoder::eofCallback(const FLAC__StreamDecoder*, void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Eof);
      return doAsyncWork(ctx, req, defaultEofCallbackReturnValue);
    }

    auto returnValue = defaultEofCallbackReturnValue;
    auto env = ctx->dec->Env();
    HandleScope scope(env);
    try {
      auto ret = ctx->eofCbk.MakeCallback(env.Global(), {});
      generateParseBooleanResult(returnValue, "Decoder:EofCallback")(ret);
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }

    return returnValue;
  }

  constexpr FLAC__StreamDecoderWriteStatus defaultWriteCallbackReturnValue =
    FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
  FLAC__StreamDecoderWriteStatus StreamDecoder::writeCallback(
    const FLAC__StreamDecoder* dec,
    const FLAC__Frame* frame,
    const int32_t* const samples[],
    void* ptr) {
    auto ctx = (DecoderWorkContext*) ptr;
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Write);
      req->frame = frame;
      req->samples = samples;
      return doAsyncWork(ctx, req, defaultWriteCallbackReturnValue);
    }

    auto returnValue = defaultWriteCallbackReturnValue;
    auto env = ctx->dec->Env();
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
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Metadata);
      req->metadata = metadata;
      doAsyncWork(ctx, req, 0);
      return;
    }

    auto env = ctx->dec->Env();
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
    if (ctx->dec->asyncExecutionProgress) {
      auto req = new DecoderWorkRequest(DecoderWorkRequest::Type::Error);
      req->errorCode = errorCode;
      doAsyncWork(ctx, req, 0);
      return;
    }

    auto env = ctx->dec->Env();
    HandleScope scope(env);
    try {
      ctx->metadataCbk.MakeCallback(env.Global(), {numberToJs(env, errorCode)});
    } catch (const Error& error) {
      error.ThrowAsJavaScriptException();
    }
  }

}
