#include "../mappings/mappings.hpp"
#include "decoder.hpp"

namespace flac_bindings {

  Function StreamDecoderBuilder::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto constructor = DefineClass(
      env,
      "StreamDecoderBuilder",
      {
        InstanceMethod("getState", &StreamDecoderBuilder::getState),
        InstanceMethod("getMd5Checking", &StreamDecoderBuilder::getMd5Checking),

        InstanceMethod("setOggSerialNumber", &StreamDecoderBuilder::setOggSerialNumber),
        InstanceMethod("setMd5Checking", &StreamDecoderBuilder::setMd5Checking),
        InstanceMethod("setMetadataRespond", &StreamDecoderBuilder::setMetadataRespond),
        InstanceMethod("setMetadataRespondAll", &StreamDecoderBuilder::setMetadataRespondAll),
        InstanceMethod(
          "setMetadataRespondApplication",
          &StreamDecoderBuilder::setMetadataRespondApplication),
        InstanceMethod("setMetadataIgnore", &StreamDecoderBuilder::setMetadataIgnore),
        InstanceMethod("setMetadataIgnoreAll", &StreamDecoderBuilder::setMetadataIgnoreAll),
        InstanceMethod(
          "setMetadataIgnoreApplication",
          &StreamDecoderBuilder::setMetadataIgnoreApplication),

        InstanceMethod("buildWithStream", &StreamDecoderBuilder::buildWithStream),
        InstanceMethod("buildWithOggStream", &StreamDecoderBuilder::buildWithOggStream),
        InstanceMethod("buildWithFile", &StreamDecoderBuilder::buildWithFile),
        InstanceMethod("buildWithOggFile", &StreamDecoderBuilder::buildWithOggFile),

        InstanceMethod("buildWithStreamAsync", &StreamDecoderBuilder::buildWithStreamAsync),
        InstanceMethod("buildWithOggStreamAsync", &StreamDecoderBuilder::buildWithOggStreamAsync),
        InstanceMethod("buildWithFileAsync", &StreamDecoderBuilder::buildWithFileAsync),
        InstanceMethod("buildWithOggFileAsync", &StreamDecoderBuilder::buildWithOggFileAsync),
      });

    addon.decoderBuilderConstructor = Persistent(constructor);

    return scope.Escape(objectFreeze(constructor)).As<Function>();
  }

  StreamDecoderBuilder::StreamDecoderBuilder(const CallbackInfo& info):
      ObjectWrap<StreamDecoderBuilder>(info) {
    dec = FLAC__stream_decoder_new();
    if (dec == nullptr) {
      throw Error::New(info.Env(), "Could not allocate memory");
    }
  }

  StreamDecoderBuilder::~StreamDecoderBuilder() {
    if (dec != nullptr) {
      FLAC__stream_decoder_delete(dec);
    }
  }

  // -- getters --

  Napi::Value StreamDecoderBuilder::getState(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto state = FLAC__stream_decoder_get_state(dec);
    return numberToJs(info.Env(), state);
  }

  Napi::Value StreamDecoderBuilder::getMd5Checking(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto md5Checking = FLAC__stream_decoder_get_md5_checking(dec);
    return booleanToJs(info.Env(), md5Checking);
  }

  // -- setters --

  Napi::Value StreamDecoderBuilder::setOggSerialNumber(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto oggSerialNumber = numberFromJs<long>(info[0]);
    FLAC__stream_decoder_set_ogg_serial_number(dec, oggSerialNumber);
    return info.This();
  }

  Napi::Value StreamDecoderBuilder::setMd5Checking(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto check = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_decoder_set_md5_checking(dec, check);
    return info.This();
  }

  Napi::Value StreamDecoderBuilder::setMetadataRespond(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto metadataType = numberFromJs<FLAC__MetadataType>(info[0]);
    FLAC__stream_decoder_set_metadata_respond(dec, metadataType);
    return info.This();
  }

  Napi::Value StreamDecoderBuilder::setMetadataRespondAll(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    FLAC__stream_decoder_set_metadata_respond_all(dec);
    return info.This();
  }

  Napi::Value StreamDecoderBuilder::setMetadataRespondApplication(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    FLAC__byte* id;
    size_t size;
    std::tie(id, size) = pointer::fromBuffer<FLAC__byte>(info[0]);

    if (size < 4) {
      throw Error::New(info.Env(), "ID buffer must be 4 bytes length");
    }

    FLAC__stream_decoder_set_metadata_respond_application(dec, id);
    return info.This();
  }

  Napi::Value StreamDecoderBuilder::setMetadataIgnore(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto metadataType = numberFromJs<FLAC__MetadataType>(info[0]);
    FLAC__stream_decoder_set_metadata_ignore(dec, metadataType);
    return info.This();
  }

  Napi::Value StreamDecoderBuilder::setMetadataIgnoreAll(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    FLAC__stream_decoder_set_metadata_ignore_all(dec);
    return info.This();
  }

  Napi::Value StreamDecoderBuilder::setMetadataIgnoreApplication(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    FLAC__byte* id;
    size_t size;
    std::tie(id, size) = pointer::fromBuffer<FLAC__byte>(info[0]);

    if (size < 4) {
      throw Error::New(info.Env(), "ID buffer must be 4 bytes length");
    }

    FLAC__stream_decoder_set_metadata_ignore_application(dec, id);
    return info.This();
  }

  // -- builder methods --

  Napi::Value StreamDecoderBuilder::buildWithStream(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Sync);
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
    return scope.Escape(createDecoder(info.Env(), info.This(), ctx));
  }

  Napi::Value StreamDecoderBuilder::buildWithOggStream(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Sync);
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
    return scope.Escape(createDecoder(info.Env(), info.This(), ctx));
  }

  Napi::Value StreamDecoderBuilder::buildWithFile(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto path = stringFromJs(info[0]);
    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Sync);
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
    return scope.Escape(createDecoder(info.Env(), info.This(), ctx));
  }

  Napi::Value StreamDecoderBuilder::buildWithOggFile(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto path = stringFromJs(info[0]);
    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Sync);
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
    return scope.Escape(createDecoder(info.Env(), info.This(), ctx));
  }

  // -- async builders --

  Napi::Value StreamDecoderBuilder::buildWithStreamAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->lengthCbk, info[3]);
    maybeFunctionIntoRef(ctx->eofCbk, info[4]);
    maybeFunctionIntoRef(ctx->writeCbk, info[5]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[6]);
    maybeFunctionIntoRef(ctx->errorCbk, info[7]);

    // why no mutex? JS runs in a single thread, and the check has already been done
    AsyncDecoderWork* work = AsyncDecoderWork::forInitStream({info.This()}, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  Napi::Value StreamDecoderBuilder::buildWithOggStreamAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->lengthCbk, info[3]);
    maybeFunctionIntoRef(ctx->eofCbk, info[4]);
    maybeFunctionIntoRef(ctx->writeCbk, info[5]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[6]);
    maybeFunctionIntoRef(ctx->errorCbk, info[7]);

    AsyncDecoderWork* work = AsyncDecoderWork::forInitOggStream({info.This()}, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  Napi::Value StreamDecoderBuilder::buildWithFileAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto path = stringFromJs(info[0]);
    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[2]);
    maybeFunctionIntoRef(ctx->errorCbk, info[3]);

    AsyncDecoderWork* work = AsyncDecoderWork::forInitFile({info.This()}, path, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  Napi::Value StreamDecoderBuilder::buildWithOggFileAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto path = stringFromJs(info[0]);
    auto ctx = std::make_shared<DecoderWorkContext>(dec, DecoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[2]);
    maybeFunctionIntoRef(ctx->errorCbk, info[3]);

    AsyncDecoderWork* work = AsyncDecoderWork::forInitOggFile({info.This()}, path, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  // -- helpers --

  Napi::Value StreamDecoderBuilder::createDecoder(
    Napi::Env env,
    Napi::Value self,
    std::shared_ptr<DecoderWorkContext> ctx) {
    EscapableHandleScope scope(env);
    auto addon = env.GetInstanceData<FlacAddon>();

    auto decoderJs = addon->decoderConstructor.New({self});

    auto decoder = ObjectWrap<StreamDecoder>::Unwrap(decoderJs);
    decoder->dec = dec;
    decoder->ctx = ctx;

    // decoder is build, cannot be used in builder
    dec = nullptr;
    workInProgress = false;

    return scope.Escape(decoderJs);
  }

  void StreamDecoderBuilder::checkInitStatus(Napi::Env env, FLAC__StreamDecoderInitStatus status) {
    // set work to false, because when this method is called, no more background work is being done
    workInProgress = false;

    if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
      const char* statusString;
      // remove prefix FLAC__STREAM_DECODER_INIT_STATUS_
      statusString = FLAC__StreamDecoderInitStatusString[status] + 33;

      auto error = Error::New(env, "Decoder initialization failed: "s + statusString);
      error.Set("status", numberToJs(env, status));
      error.Set("statusString", String::New(env, statusString));
      throw error;
    }
  }

  void StreamDecoderBuilder::checkIfBuilt(Napi::Env env) {
    // if null, means one of the buildWith* methods have been called (see above)
    if (dec == nullptr) {
      throw Error::New(env, "Decoder has been built - cannot call any method");
    }

    // if workInProgress is true, then fail too
    if (workInProgress.load()) {
      throw Error::New(env, "There is a pending Promise running, wait until is resolved");
    }
  }

}
