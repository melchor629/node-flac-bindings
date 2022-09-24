#include "../mappings/mappings.hpp"
#include "encoder.hpp"

namespace flac_bindings {

  Function StreamEncoderBuilder::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto constructor = DefineClass(
      env,
      "StreamEncoderBuilder",
      {
        InstanceMethod("getState", &StreamEncoderBuilder::getState),
        InstanceMethod("getVerify", &StreamEncoderBuilder::getVerify),
        InstanceMethod("getStreamableSubset", &StreamEncoderBuilder::getStreamableSubset),
        InstanceMethod("getChannels", &StreamEncoderBuilder::getChannels),
        InstanceMethod("getBitsPerSample", &StreamEncoderBuilder::getBitsPerSample),
        InstanceMethod("getSampleRate", &StreamEncoderBuilder::getSampleRate),
        InstanceMethod("getBlocksize", &StreamEncoderBuilder::getBlocksize),
        InstanceMethod("getDoMidSideStereo", &StreamEncoderBuilder::getDoMidSideStereo),
        InstanceMethod("getLooseMidSideStereo", &StreamEncoderBuilder::getLooseMidSideStereo),
        InstanceMethod("getMaxLpcOrder", &StreamEncoderBuilder::getMaxLpcOrder),
        InstanceMethod("getQlpCoeffPrecision", &StreamEncoderBuilder::getQlpCoeffPrecision),
        InstanceMethod("getDoQlpCoeffPrecSearch", &StreamEncoderBuilder::getDoQlpCoeffPrecSearch),
        InstanceMethod("getDoEscapeCoding", &StreamEncoderBuilder::getDoEscapeCoding),
        InstanceMethod(
          "getDoExhaustiveModelSearch",
          &StreamEncoderBuilder::getDoExhaustiveModelSearch),
        InstanceMethod(
          "getMinResidualPartitionOrder",
          &StreamEncoderBuilder::getMinResidualPartitionOrder),
        InstanceMethod(
          "getMaxResidualPartitionOrder",
          &StreamEncoderBuilder::getMaxResidualPartitionOrder),
        InstanceMethod(
          "getRiceParameterSearchDist",
          &StreamEncoderBuilder::getRiceParameterSearchDist),
        InstanceMethod("getLimitMinBitrate", &StreamEncoderBuilder::getLimitMinBitrate),
        InstanceMethod("getTotalSamplesEstimate", &StreamEncoderBuilder::getTotalSamplesEstimate),

        InstanceMethod("setOggSerialNumber", &StreamEncoderBuilder::setOggSerialNumber),
        InstanceMethod("setVerify", &StreamEncoderBuilder::setVerify),
        InstanceMethod("setStreamableSubset", &StreamEncoderBuilder::setStreamableSubset),
        InstanceMethod("setChannels", &StreamEncoderBuilder::setChannels),
        InstanceMethod("setBitsPerSample", &StreamEncoderBuilder::setBitsPerSample),
        InstanceMethod("setSampleRate", &StreamEncoderBuilder::setSampleRate),
        InstanceMethod("setCompressionLevel", &StreamEncoderBuilder::setCompressionLevel),
        InstanceMethod("setBlocksize", &StreamEncoderBuilder::setBlocksize),
        InstanceMethod("setDoMidSideStereo", &StreamEncoderBuilder::setDoMidSideStereo),
        InstanceMethod("setLooseMidSideStereo", &StreamEncoderBuilder::setLooseMidSideStereo),
        InstanceMethod("setMaxLpcOrder", &StreamEncoderBuilder::setMaxLpcOrder),
        InstanceMethod("setQlpCoeffPrecision", &StreamEncoderBuilder::setQlpCoeffPrecision),
        InstanceMethod("setDoQlpCoeffPrecSearch", &StreamEncoderBuilder::setDoQlpCoeffPrecSearch),
        InstanceMethod("setDoEscapeCoding", &StreamEncoderBuilder::setDoEscapeCoding),
        InstanceMethod(
          "setDoExhaustiveModelSearch",
          &StreamEncoderBuilder::setDoExhaustiveModelSearch),
        InstanceMethod(
          "setMinResidualPartitionOrder",
          &StreamEncoderBuilder::setMinResidualPartitionOrder),
        InstanceMethod(
          "setMaxResidualPartitionOrder",
          &StreamEncoderBuilder::setMaxResidualPartitionOrder),
        InstanceMethod(
          "setRiceParameterSearchDist",
          &StreamEncoderBuilder::setRiceParameterSearchDist),
        InstanceMethod("setTotalSamplesEstimate", &StreamEncoderBuilder::setTotalSamplesEstimate),
        InstanceMethod("setMetadata", &StreamEncoderBuilder::setMetadata),
        InstanceMethod("setApodization", &StreamEncoderBuilder::setApodization),
        InstanceMethod("setLimitMinBitrate", &StreamEncoderBuilder::setLimitMinBitrate),

        InstanceMethod("buildWithStream", &StreamEncoderBuilder::buildWithStream),
        InstanceMethod("buildWithOggStream", &StreamEncoderBuilder::buildWithOggStream),
        InstanceMethod("buildWithFile", &StreamEncoderBuilder::buildWithFile),
        InstanceMethod("buildWithOggFile", &StreamEncoderBuilder::buildWithOggFile),

        InstanceMethod("buildWithStreamAsync", &StreamEncoderBuilder::buildWithStreamAsync),
        InstanceMethod("buildWithOggStreamAsync", &StreamEncoderBuilder::buildWithOggStreamAsync),
        InstanceMethod("buildWithFileAsync", &StreamEncoderBuilder::buildWithFileAsync),
        InstanceMethod("buildWithOggFileAsync", &StreamEncoderBuilder::buildWithOggFileAsync),
      });

    addon.encoderBuilderConstructor = Persistent(constructor);

    return scope.Escape(objectFreeze(constructor)).As<Function>();
  }

  StreamEncoderBuilder::StreamEncoderBuilder(const CallbackInfo& info):
      ObjectWrap<StreamEncoderBuilder>(info) {
    enc = FLAC__stream_encoder_new();
    if (enc == nullptr) {
      throw Error::New(info.Env(), "Could not allocate memory");
    }

    info.This().As<Object>().Set("__metadataArrayRef", info.Env().Null());
  }

  StreamEncoderBuilder::~StreamEncoderBuilder() {
    if (enc != nullptr) {
      FLAC__stream_encoder_delete(enc);
    }
  }

  // -- getters --

  Napi::Value StreamEncoderBuilder::getVerify(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto verify = FLAC__stream_encoder_get_verify(enc);
    return booleanToJs(info.Env(), verify);
  }

  Napi::Value StreamEncoderBuilder::getStreamableSubset(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto streamableSubset = FLAC__stream_encoder_get_streamable_subset(enc);
    return booleanToJs(info.Env(), streamableSubset);
  }

  Napi::Value StreamEncoderBuilder::getChannels(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto channels = FLAC__stream_encoder_get_channels(enc);
    return numberToJs(info.Env(), channels);
  }

  Napi::Value StreamEncoderBuilder::getBitsPerSample(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto bitsPerSample = FLAC__stream_encoder_get_bits_per_sample(enc);
    return numberToJs(info.Env(), bitsPerSample);
  }

  Napi::Value StreamEncoderBuilder::getSampleRate(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto sampleRate = FLAC__stream_encoder_get_sample_rate(enc);
    return numberToJs(info.Env(), sampleRate);
  }

  Napi::Value StreamEncoderBuilder::getBlocksize(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto blocksize = FLAC__stream_encoder_get_blocksize(enc);
    return numberToJs(info.Env(), blocksize);
  }

  Napi::Value StreamEncoderBuilder::getDoMidSideStereo(const CallbackInfo& info) {
    auto doMidStereo = FLAC__stream_encoder_get_do_mid_side_stereo(enc);
    return booleanToJs(info.Env(), doMidStereo);
  }

  Napi::Value StreamEncoderBuilder::getLooseMidSideStereo(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto looseMidStereo = FLAC__stream_encoder_get_loose_mid_side_stereo(enc);
    return booleanToJs(info.Env(), looseMidStereo);
  }

  Napi::Value StreamEncoderBuilder::getMaxLpcOrder(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto maxLpcOrder = FLAC__stream_encoder_get_max_lpc_order(enc);
    return numberToJs(info.Env(), maxLpcOrder);
  }

  Napi::Value StreamEncoderBuilder::getQlpCoeffPrecision(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto qlpCoeffPrecision = FLAC__stream_encoder_get_qlp_coeff_precision(enc);
    return numberToJs(info.Env(), qlpCoeffPrecision);
  }

  Napi::Value StreamEncoderBuilder::getDoQlpCoeffPrecSearch(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto doQlpCoeffPrecSearch = FLAC__stream_encoder_get_do_qlp_coeff_prec_search(enc);
    return booleanToJs(info.Env(), doQlpCoeffPrecSearch);
  }

  Napi::Value StreamEncoderBuilder::getDoEscapeCoding(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto doEscapeCoding = FLAC__stream_encoder_get_do_escape_coding(enc);
    return booleanToJs(info.Env(), doEscapeCoding);
  }

  Napi::Value StreamEncoderBuilder::getDoExhaustiveModelSearch(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto doExhaustiveModelSearch = FLAC__stream_encoder_get_do_exhaustive_model_search(enc);
    return booleanToJs(info.Env(), doExhaustiveModelSearch);
  }

  Napi::Value StreamEncoderBuilder::getMinResidualPartitionOrder(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto minResidualPartitionOrder = FLAC__stream_encoder_get_min_residual_partition_order(enc);
    return numberToJs(info.Env(), minResidualPartitionOrder);
  }

  Napi::Value StreamEncoderBuilder::getMaxResidualPartitionOrder(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto maxResidualPartitionOrder = FLAC__stream_encoder_get_max_residual_partition_order(enc);
    return numberToJs(info.Env(), maxResidualPartitionOrder);
  }

  Napi::Value StreamEncoderBuilder::getRiceParameterSearchDist(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto riceParameterSearchDist = FLAC__stream_encoder_get_rice_parameter_search_dist(enc);
    return numberToJs(info.Env(), riceParameterSearchDist);
  }

  Napi::Value StreamEncoderBuilder::getLimitMinBitrate(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
#if FLAC_API_VERSION_CURRENT >= 12
    auto value = FLAC__stream_encoder_get_limit_min_bitrate(enc);
    return booleanToJs(info.Env(), value);
#else
    return booleanToJs(info.Env(), false);
#endif
  }

  Napi::Value StreamEncoderBuilder::getTotalSamplesEstimate(const CallbackInfo& info) {
    auto totalSamplesEstimate = FLAC__stream_encoder_get_total_samples_estimate(enc);
    return numberToJs(info.Env(), totalSamplesEstimate);
  }

  Napi::Value StreamEncoderBuilder::getState(const CallbackInfo& info) {
    checkIfBuilt(info.Env());
    auto state = FLAC__stream_encoder_get_state(enc);
    return numberToJs(info.Env(), state);
  }

  // -- setters --

  Napi::Value StreamEncoderBuilder::setVerify(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto verify = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_verify(enc, verify);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setOggSerialNumber(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto oggSerialNumber = numberFromJs<long>(info[0]);
    FLAC__stream_encoder_set_ogg_serial_number(enc, oggSerialNumber);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setStreamableSubset(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto streamableSubset = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_streamable_subset(enc, streamableSubset);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setChannels(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto channels = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_channels(enc, channels);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setBitsPerSample(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto bitsPerSample = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_bits_per_sample(enc, bitsPerSample);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setSampleRate(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto sampleRate = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_sample_rate(enc, sampleRate);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setCompressionLevel(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto compressionLevel = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_compression_level(enc, compressionLevel);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setBlocksize(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto blocksize = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_blocksize(enc, blocksize);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setDoMidSideStereo(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto doMidStereo = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_do_mid_side_stereo(enc, doMidStereo);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setLooseMidSideStereo(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto looseMidStereo = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_loose_mid_side_stereo(enc, looseMidStereo);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setMaxLpcOrder(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto maxLpcOrder = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_max_lpc_order(enc, maxLpcOrder);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setQlpCoeffPrecision(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto qlpCoeffPrecision = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_qlp_coeff_precision(enc, qlpCoeffPrecision);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setDoQlpCoeffPrecSearch(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto doQlpCoeffPrecSearch = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_do_qlp_coeff_prec_search(enc, doQlpCoeffPrecSearch);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setDoEscapeCoding(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto doEscapeCoding = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_do_escape_coding(enc, doEscapeCoding);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setDoExhaustiveModelSearch(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto doExhaustiveModelSearch = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_do_exhaustive_model_search(enc, doExhaustiveModelSearch);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setMinResidualPartitionOrder(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto minResidualPartitionOrder = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_min_residual_partition_order(enc, minResidualPartitionOrder);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setMaxResidualPartitionOrder(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto maxResidualPartitionOrder = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_max_residual_partition_order(enc, maxResidualPartitionOrder);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setRiceParameterSearchDist(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto riceParameterSearchDist = numberFromJs<unsigned>(info[0]);
    FLAC__stream_encoder_set_rice_parameter_search_dist(enc, riceParameterSearchDist);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setTotalSamplesEstimate(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto totalSamplesEstimate = numberFromJs<uint64_t>(info[0]);
    FLAC__stream_encoder_set_total_samples_estimate(enc, totalSamplesEstimate);
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setMetadata(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    std::vector<FLAC__StreamMetadata*> metadatas =
      arrayFromJs<FLAC__StreamMetadata*>(info[0], Metadata::fromJs);

    auto ret = FLAC__stream_encoder_set_metadata(enc, metadatas.data(), metadatas.size());
    if (!ret) {
      throw Error::New(info.Env(), "Could not set the metadata");
    }

    // Save all objects inside this to hold the references until the object is destroyed
    info.This().As<Napi::Object>().Set("__metadataArrayRef", info[0]);

    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setApodization(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

    auto apodization = stringFromJs(info[0]);
    FLAC__stream_encoder_set_apodization(enc, apodization.c_str());
    return info.This();
  }

  Napi::Value StreamEncoderBuilder::setLimitMinBitrate(const CallbackInfo& info) {
    checkIfBuilt(info.Env());

#if FLAC_API_VERSION_CURRENT >= 12
    auto value = booleanFromJs<FLAC__bool>(info[0]);
    FLAC__stream_encoder_set_limit_min_bitrate(enc, value);
#endif
    return info.This();
  }

  // -- builder methods --

  Napi::Value StreamEncoderBuilder::buildWithStream(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Sync);
    maybeFunctionIntoRef(ctx->writeCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[3]);

    auto ret = FLAC__stream_encoder_init_stream(
      enc,
      ctx->writeCbk.IsEmpty() ? nullptr : StreamEncoder::writeCallback,
      ctx->seekCbk.IsEmpty() ? nullptr : StreamEncoder::seekCallback,
      ctx->tellCbk.IsEmpty() ? nullptr : StreamEncoder::tellCallback,
      ctx->metadataCbk.IsEmpty() ? nullptr : StreamEncoder::metadataCallback,
      ctx.get());

    checkInitStatus(info.Env(), ret);
    return scope.Escape(createEncoder(info.Env(), info.This(), ctx));
  }

  Napi::Value StreamEncoderBuilder::buildWithOggStream(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Sync);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->seekCbk, info[2]);
    maybeFunctionIntoRef(ctx->tellCbk, info[3]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[4]);

    auto ret = FLAC__stream_encoder_init_ogg_stream(
      enc,
      ctx->readCbk.IsEmpty() ? nullptr : StreamEncoder::readCallback,
      ctx->writeCbk.IsEmpty() ? nullptr : StreamEncoder::writeCallback,
      ctx->seekCbk.IsEmpty() ? nullptr : StreamEncoder::seekCallback,
      ctx->tellCbk.IsEmpty() ? nullptr : StreamEncoder::tellCallback,
      ctx->metadataCbk.IsEmpty() ? nullptr : StreamEncoder::metadataCallback,
      ctx.get());

    checkInitStatus(info.Env(), ret);
    return scope.Escape(createEncoder(info.Env(), info.This(), ctx));
  }

  Napi::Value StreamEncoderBuilder::buildWithFile(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto path = stringFromJs(info[0]);
    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Sync);
    maybeFunctionIntoRef(ctx->progressCbk, info[1]);

    auto ret = FLAC__stream_encoder_init_file(
      enc,
      path.c_str(),
      ctx->progressCbk.IsEmpty() ? nullptr : StreamEncoder::progressCallback,
      ctx.get());

    checkInitStatus(info.Env(), ret);
    return scope.Escape(createEncoder(info.Env(), info.This(), ctx));
  }

  Napi::Value StreamEncoderBuilder::buildWithOggFile(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto path = stringFromJs(info[0]);
    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Sync);
    maybeFunctionIntoRef(ctx->progressCbk, info[1]);

    auto ret = FLAC__stream_encoder_init_ogg_file(
      enc,
      path.c_str(),
      ctx->progressCbk.IsEmpty() ? nullptr : StreamEncoder::progressCallback,
      ctx.get());

    checkInitStatus(info.Env(), ret);
    return scope.Escape(createEncoder(info.Env(), info.This(), ctx));
  }

  // -- async builders --

  Napi::Value StreamEncoderBuilder::buildWithStreamAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->writeCbk, info[0]);
    maybeFunctionIntoRef(ctx->seekCbk, info[1]);
    maybeFunctionIntoRef(ctx->tellCbk, info[2]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[3]);

    // why no mutex? JS runs in a single thread, and the check has already been done
    AsyncEncoderWork* work = AsyncEncoderWork::forInitStream({info.This()}, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  Napi::Value StreamEncoderBuilder::buildWithOggStreamAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->readCbk, info[0]);
    maybeFunctionIntoRef(ctx->writeCbk, info[1]);
    maybeFunctionIntoRef(ctx->seekCbk, info[2]);
    maybeFunctionIntoRef(ctx->tellCbk, info[3]);
    maybeFunctionIntoRef(ctx->metadataCbk, info[4]);

    AsyncEncoderWork* work = AsyncEncoderWork::forInitOggStream({info.This()}, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  Napi::Value StreamEncoderBuilder::buildWithFileAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->progressCbk, info[1]);

    auto path = stringFromJs(info[0]);
    AsyncEncoderWork* work = AsyncEncoderWork::forInitFile({info.This()}, path, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  Napi::Value StreamEncoderBuilder::buildWithOggFileAsync(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    checkIfBuilt(info.Env());

    auto ctx = std::make_shared<EncoderWorkContext>(enc, EncoderWorkContext::ExecutionMode::Async);
    maybeFunctionIntoRef(ctx->progressCbk, info[1]);

    auto path = stringFromJs(info[0]);
    AsyncEncoderWork* work = AsyncEncoderWork::forInitOggFile({info.This()}, path, ctx, *this);
    workInProgress = true;
    ctx->workInProgress = true;
    work->Queue();
    return scope.Escape(work->getPromise());
  }

  // -- helpers --

  Napi::Value StreamEncoderBuilder::createEncoder(
    Napi::Env env,
    Napi::Value self,
    std::shared_ptr<EncoderWorkContext> ctx) {
    EscapableHandleScope scope(env);
    auto addon = env.GetInstanceData<FlacAddon>();

    auto metadataArrayReference = self.As<Napi::Object>().Get("__metadataArrayRef");
    auto encoderJs = addon->encoderConstructor.New({self, metadataArrayReference});

    auto encoder = StreamEncoder::Unwrap(encoderJs);
    encoder->enc = enc;
    encoder->ctx = ctx;

    // encoder is built, cannot be used in builder
    enc = nullptr;
    workInProgress = false;

    return scope.Escape(encoderJs);
  }

  void StreamEncoderBuilder::checkIfBuilt(Napi::Env env) {
    // if null, means one of the buildWith* methods have been called (see above)
    if (enc == nullptr) {
      throw Error::New(env, "Encoder has been built - cannot call any method");
    }

    // if workInProgress is true, then fail too
    if (workInProgress.load()) {
      throw Error::New(env, "There is a pending Promise running, wait until is resolved");
    }
  }

  void StreamEncoderBuilder::checkInitStatus(Napi::Env env, FLAC__StreamEncoderInitStatus status) {
    // set work to false, because when this method is called, no more background work is being done
    workInProgress = false;

    if (status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
      const char* statusString;
      // remove prefix FLAC__STREAM_ENCODER_INIT_STATUS_
      statusString = FLAC__StreamEncoderInitStatusString[status] + 33;

      auto error = Error::New(env, "Encoder initialization failed: "s + statusString);
      error.Set("status", numberToJs(env, status));
      error.Set("statusString", String::New(env, statusString));
      throw error;
    }
  }

}
