#include "encoder.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/defer.hpp"
#include "../utils/encoder_decoder_utils.hpp"

#define DEFER_SYNCHRONIZED(f) DEFER(runLocked([&] () { f; }));

namespace flac_bindings {

    FunctionReference StreamEncoder::constructor;

    static std::vector<int32_t*> getBuffersFromArray(const Napi::Value&, unsigned, unsigned);
    static int32_t* getInterleavedBufferFromArgs(const CallbackInfo&, unsigned, unsigned&);

    Function StreamEncoder::init(const Napi::Env& env) {
        EscapableHandleScope scope(env);

        auto attrs = napi_property_attributes::napi_enumerable;
        auto constructor = DefineClass(env, "StreamEncoder", {
            InstanceMethod("setOggSerialNumber", &StreamEncoder::setOggSerialNumber),
            InstanceAccessor("verify", &StreamEncoder::getVerify, &StreamEncoder::setVerify, attrs),
            InstanceAccessor("streamableSubset", &StreamEncoder::getStreamableSubset, &StreamEncoder::setStreamableSubset, attrs),
            InstanceAccessor("channels", &StreamEncoder::getChannels, &StreamEncoder::setChannels, attrs),
            InstanceAccessor("bitsPerSample", &StreamEncoder::getBitsPerSample, &StreamEncoder::setBitsPerSample, attrs),
            InstanceAccessor("sampleRate", &StreamEncoder::getSampleRate, &StreamEncoder::setSampleRate, attrs),
            InstanceMethod("setCompressionLevel", &StreamEncoder::setCompressionLevel),
            InstanceAccessor("blocksize", &StreamEncoder::getBlocksize, &StreamEncoder::setBlocksize, attrs),
            InstanceAccessor("doMidSideStereo", &StreamEncoder::getDoMidSideStereo, &StreamEncoder::setDoMidSideStereo, attrs),
            InstanceAccessor("looseMidSideStereo", &StreamEncoder::getLooseMidSideStereo, &StreamEncoder::setLooseMidSideStereo, attrs),
            InstanceAccessor("maxLpcOrder", &StreamEncoder::getMaxLpcOrder, &StreamEncoder::setMaxLpcOrder, attrs),
            InstanceAccessor("qlpCoeffPrecision", &StreamEncoder::getQlpCoeffPrecision, &StreamEncoder::setQlpCoeffPrecision, attrs),
            InstanceAccessor("doQlpCoeffPrecSearch", &StreamEncoder::getDoQlpCoeffPrecSearch, &StreamEncoder::setDoQlpCoeffPrecSearch, attrs),
            InstanceAccessor("doEscapeCoding", &StreamEncoder::getDoEscapeCoding, &StreamEncoder::setDoEscapeCoding, attrs),
            InstanceAccessor("doExhaustiveModelSearch", &StreamEncoder::getDoExhaustiveModelSearch, &StreamEncoder::setDoExhaustiveModelSearch, attrs),
            InstanceAccessor("maxResidualPartitionOrder", &StreamEncoder::getMaxResidualPartitionOrder, &StreamEncoder::setMaxResidualPartitionOrder, attrs),
            InstanceAccessor("minResidualPartitionOrder", &StreamEncoder::getMinResidualPartitionOrder, &StreamEncoder::setMinResidualPartitionOrder, attrs),
            InstanceAccessor("riceParameterSearchDist", &StreamEncoder::getRiceParameterSearchDist, &StreamEncoder::setRiceParameterSearchDist, attrs),
            InstanceAccessor("totalSamplesEstimate", &StreamEncoder::getTotalSamplesEstimate, &StreamEncoder::setTotalSamplesEstimate, attrs),
            InstanceMethod("setMetadata", &StreamEncoder::setMetadata),
            InstanceMethod("setApodization", &StreamEncoder::setApodization),
            InstanceMethod("getState", &StreamEncoder::getState),
            InstanceMethod("getVerifyDecoderState", &StreamEncoder::getVerifyDecoderState),
            InstanceMethod("getResolvedStateString", &StreamEncoder::getResolvedStateString),
            InstanceMethod("getVerifyDecoderErrorStats", &StreamEncoder::getVerifyDecoderErrorStats),

            InstanceMethod("initStream", &StreamEncoder::initStream),
            InstanceMethod("initOggStream", &StreamEncoder::initOggStream),
            InstanceMethod("initFile", &StreamEncoder::initFile),
            InstanceMethod("initOggFile", &StreamEncoder::initOggFile),
            InstanceMethod("finish", &StreamEncoder::finish),
            InstanceMethod("process", &StreamEncoder::process),
            InstanceMethod("processInterleaved", &StreamEncoder::processInterleaved),

            InstanceMethod("finishAsync", &StreamEncoder::finishAsync),
            InstanceMethod("processAsync", &StreamEncoder::processAsync),
            InstanceMethod("processInterleavedAsync", &StreamEncoder::processInterleavedAsync),
            InstanceMethod("initStreamAsync", &StreamEncoder::initStreamAsync),
            InstanceMethod("initOggStreamAsync", &StreamEncoder::initOggStreamAsync),
            InstanceMethod("initFileAsync", &StreamEncoder::initFileAsync),
            InstanceMethod("initOggFileAsync", &StreamEncoder::initOggFileAsync),
        });
        c_enum::declareInObject(constructor, "State", createStateEnum);
        c_enum::declareInObject(constructor, "InitStatus", createInitStatusEnum);
        c_enum::declareInObject(constructor, "ReadStatus", createReadStatusEnum);
        c_enum::declareInObject(constructor, "WriteStatus", createWriteStatusEnum);
        c_enum::declareInObject(constructor, "SeekStatus", createSeekStatusEnum);
        c_enum::declareInObject(constructor, "TellStatus", createTellStatusEnum);

        StreamEncoder::constructor = Persistent(constructor);
        StreamEncoder::constructor.SuppressDestruct();

        return scope.Escape(objectFreeze(constructor)).As<Function>();
    }

    StreamEncoder::StreamEncoder(const CallbackInfo& info): ObjectWrap<StreamEncoder>(info) {
        enc = FLAC__stream_encoder_new();
        if(enc == nullptr) {
            throw Error::New(info.Env(), "Could not allocate memory");
        }
    }

    StreamEncoder::~StreamEncoder() {
        FLAC__stream_encoder_delete(enc);
    }


    void StreamEncoder::setOggSerialNumber(const CallbackInfo& info) {
        checkIsNotInitialized(info.Env());
        auto oggSerialNumber = numberFromJs<long>(info[0]);
        FLAC__stream_encoder_set_ogg_serial_number(enc, oggSerialNumber);
    }

    Napi::Value StreamEncoder::getVerify(const CallbackInfo& info) {
        auto verify = FLAC__stream_encoder_get_verify(enc);
        return booleanToJs(info.Env(), verify);
    }

    void StreamEncoder::setVerify(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto verify = booleanFromJs<FLAC__bool>(value);
        FLAC__stream_encoder_set_verify(enc, verify);
    }

    Napi::Value StreamEncoder::getStreamableSubset(const CallbackInfo& info) {
        auto streamableSubset = FLAC__stream_encoder_get_streamable_subset(enc);
        return booleanToJs(info.Env(), streamableSubset);
    }

    void StreamEncoder::setStreamableSubset(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto streamableSubset = booleanFromJs<FLAC__bool>(value);
        FLAC__stream_encoder_set_streamable_subset(enc, streamableSubset);
    }

    Napi::Value StreamEncoder::getChannels(const CallbackInfo& info) {
        auto channels = FLAC__stream_encoder_get_channels(enc);
        return numberToJs(info.Env(), channels);
    }

    void StreamEncoder::setChannels(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto channels = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_channels(enc, channels);
    }

    Napi::Value StreamEncoder::getBitsPerSample(const CallbackInfo& info) {
        auto bitsPerSample = FLAC__stream_encoder_get_bits_per_sample(enc);
        return numberToJs(info.Env(), bitsPerSample);
    }

    void StreamEncoder::setBitsPerSample(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto bitsPerSample = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_bits_per_sample(enc, bitsPerSample);
    }

    Napi::Value StreamEncoder::getSampleRate(const CallbackInfo& info) {
        auto sampleRate = FLAC__stream_encoder_get_sample_rate(enc);
        return numberToJs(info.Env(), sampleRate);
    }

    void StreamEncoder::setSampleRate(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto sampleRate = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_sample_rate(enc, sampleRate);
    }

    void StreamEncoder::setCompressionLevel(const CallbackInfo& info) {
        checkIsNotInitialized(info.Env());
        auto compressionLevel = numberFromJs<unsigned>(info[0]);
        FLAC__stream_encoder_set_compression_level(enc, compressionLevel);
    }

    Napi::Value StreamEncoder::getBlocksize(const CallbackInfo& info) {
        auto blocksize = FLAC__stream_encoder_get_blocksize(enc);
        return numberToJs(info.Env(), blocksize);
    }

    void StreamEncoder::setBlocksize(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto blocksize = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_blocksize(enc, blocksize);
    }

    Napi::Value StreamEncoder::getDoMidSideStereo(const CallbackInfo& info) {
        auto doMidStereo = FLAC__stream_encoder_get_do_mid_side_stereo(enc);
        return booleanToJs(info.Env(), doMidStereo);
    }

    void StreamEncoder::setDoMidSideStereo(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto doMidStereo = booleanFromJs<FLAC__bool>(value);
        FLAC__stream_encoder_set_do_mid_side_stereo(enc, doMidStereo);
    }

    Napi::Value StreamEncoder::getLooseMidSideStereo(const CallbackInfo& info) {
        auto looseMidStereo = FLAC__stream_encoder_get_loose_mid_side_stereo(enc);
        return booleanToJs(info.Env(), looseMidStereo);
    }

    void StreamEncoder::setLooseMidSideStereo(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto looseMidStereo = booleanFromJs<FLAC__bool>(value);
        FLAC__stream_encoder_set_loose_mid_side_stereo(enc, looseMidStereo);
    }

    Napi::Value StreamEncoder::getMaxLpcOrder(const CallbackInfo& info) {
        auto maxLpcOrder = FLAC__stream_encoder_get_max_lpc_order(enc);
        return numberToJs(info.Env(), maxLpcOrder);
    }

    void StreamEncoder::setMaxLpcOrder(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto maxLpcOrder = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_max_lpc_order(enc, maxLpcOrder);
    }

    Napi::Value StreamEncoder::getQlpCoeffPrecision(const CallbackInfo& info) {
        auto qlpCoeffPrecision = FLAC__stream_encoder_get_qlp_coeff_precision(enc);
        return numberToJs(info.Env(), qlpCoeffPrecision);
    }

    void StreamEncoder::setQlpCoeffPrecision(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto qlpCoeffPrecision = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_qlp_coeff_precision(enc, qlpCoeffPrecision);
    }

    Napi::Value StreamEncoder::getDoQlpCoeffPrecSearch(const CallbackInfo& info) {
        auto doQlpCoeffPrecSearch = FLAC__stream_encoder_get_do_qlp_coeff_prec_search(enc);
        return booleanToJs(info.Env(), doQlpCoeffPrecSearch);
    }

    void StreamEncoder::setDoQlpCoeffPrecSearch(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto doQlpCoeffPrecSearch = booleanFromJs<FLAC__bool>(value);
        FLAC__stream_encoder_set_do_qlp_coeff_prec_search(enc, doQlpCoeffPrecSearch);
    }

    Napi::Value StreamEncoder::getDoEscapeCoding(const CallbackInfo& info) {
        auto doEscapeCoding = FLAC__stream_encoder_get_do_escape_coding(enc);
        return booleanToJs(info.Env(), doEscapeCoding);
    }

    void StreamEncoder::setDoEscapeCoding(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto doEscapeCoding = booleanFromJs<FLAC__bool>(value);
        FLAC__stream_encoder_set_do_escape_coding(enc, doEscapeCoding);
    }

    Napi::Value StreamEncoder::getDoExhaustiveModelSearch(const CallbackInfo& info) {
        auto doExhaustiveModelSearch = FLAC__stream_encoder_get_do_exhaustive_model_search(enc);
        return booleanToJs(info.Env(), doExhaustiveModelSearch);
    }

    void StreamEncoder::setDoExhaustiveModelSearch(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto doExhaustiveModelSearch = booleanFromJs<FLAC__bool>(value);
        FLAC__stream_encoder_set_do_exhaustive_model_search(enc, doExhaustiveModelSearch);
    }

    Napi::Value StreamEncoder::getMinResidualPartitionOrder(const CallbackInfo& info) {
        auto minResidualPartitionOrder = FLAC__stream_encoder_get_min_residual_partition_order(enc);
        return numberToJs(info.Env(), minResidualPartitionOrder);
    }

    void StreamEncoder::setMinResidualPartitionOrder(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto minResidualPartitionOrder = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_min_residual_partition_order(enc, minResidualPartitionOrder);
    }

    Napi::Value StreamEncoder::getMaxResidualPartitionOrder(const CallbackInfo& info) {
        auto maxResidualPartitionOrder = FLAC__stream_encoder_get_max_residual_partition_order(enc);
        return numberToJs(info.Env(), maxResidualPartitionOrder);
    }

    void StreamEncoder::setMaxResidualPartitionOrder(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto maxResidualPartitionOrder = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_max_residual_partition_order(enc, maxResidualPartitionOrder);
    }

    Napi::Value StreamEncoder::getRiceParameterSearchDist(const CallbackInfo& info) {
        auto riceParameterSearchDist = FLAC__stream_encoder_get_rice_parameter_search_dist(enc);
        return numberToJs(info.Env(), riceParameterSearchDist);
    }

    void StreamEncoder::setRiceParameterSearchDist(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto riceParameterSearchDist = numberFromJs<unsigned>(value);
        FLAC__stream_encoder_set_rice_parameter_search_dist(enc, riceParameterSearchDist);
    }

    Napi::Value StreamEncoder::getTotalSamplesEstimate(const CallbackInfo& info) {
        auto totalSamplesEstimate = FLAC__stream_encoder_get_total_samples_estimate(enc);
        return numberToJs(info.Env(), totalSamplesEstimate);
    }

    void StreamEncoder::setTotalSamplesEstimate(const CallbackInfo& info, const Napi::Value& value) {
        checkIsNotInitialized(info.Env());
        auto totalSamplesEstimate = numberFromJs<uint64_t>(value);
        FLAC__stream_encoder_set_total_samples_estimate(enc, totalSamplesEstimate);
    }

    void StreamEncoder::setMetadata(const CallbackInfo& info) {
        checkIsNotInitialized(info.Env());
        std::vector<FLAC__StreamMetadata*> metadatas = arrayFromJs<FLAC__StreamMetadata*>(info[0], Metadata::fromJs);

        auto ret = FLAC__stream_encoder_set_metadata(enc, metadatas.data(), metadatas.size());
        if(!ret) {
            throw Error::New(info.Env(), "Could not set the metadata");
        }

        //Save all objects inside this to hold the references untile the object is destroyed
        metadataArrayRef = Persistent(info[0].As<Object>());
    }

    void StreamEncoder::setApodization(const CallbackInfo& info) {
        checkIsNotInitialized(info.Env());
        auto apodization = stringFromJs(info[0]);
        FLAC__stream_encoder_set_apodization(enc, apodization.c_str());
    }

    Napi::Value StreamEncoder::getState(const CallbackInfo& info) {
        auto state = FLAC__stream_encoder_get_state(enc);
        return numberToJs(info.Env(), state);
    }

    Napi::Value StreamEncoder::getVerifyDecoderState(const CallbackInfo& info) {
        if(FLAC__stream_encoder_get_state(enc) == FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR) {
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
        if(FLAC__stream_encoder_get_state(enc) == FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR) {
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
                &got
            );

            Object obj = Object::New(info.Env());
            auto attrs = napi_property_attributes::napi_enumerable;
            obj.DefineProperties({
                Napi::PropertyDescriptor::Value("absoluteSample", numberToJs(info.Env(), absolute_sample), attrs),
                Napi::PropertyDescriptor::Value("frameNumber", numberToJs(info.Env(), frame_number), attrs),
                Napi::PropertyDescriptor::Value("channel", numberToJs(info.Env(), channel), attrs),
                Napi::PropertyDescriptor::Value("sample", numberToJs(info.Env(), sample), attrs),
                Napi::PropertyDescriptor::Value("expected", numberToJs(info.Env(), expected), attrs),
                Napi::PropertyDescriptor::Value("got", numberToJs(info.Env(), got), attrs),
            });
            info.Env().Global().Get("Object").As<Object>().Get("freeze").As<Function>().Call({obj});
            return obj;
        }

        return info.Env().Null();
    }


    Napi::Value StreamEncoder::initStream(const CallbackInfo& info) {
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->writeCbk, info[0]);
        maybeFunctionIntoRef(ctx->seekCbk, info[1]);
        maybeFunctionIntoRef(ctx->tellCbk, info[2]);
        maybeFunctionIntoRef(ctx->metadataCbk, info[3]);

        asyncContext = new AsyncContext(info.Env(), "flac_bindings::StreamEncoder::initStream");
        DEFER_SYNCHRONIZED({ delete asyncContext; asyncContext = nullptr; });

        auto ret = FLAC__stream_encoder_init_stream(
            enc,
            ctx->writeCbk.IsEmpty() ? nullptr : StreamEncoder::writeCallback,
            ctx->seekCbk.IsEmpty() ? nullptr : StreamEncoder::seekCallback,
            ctx->tellCbk.IsEmpty() ? nullptr : StreamEncoder::tellCallback,
            ctx->metadataCbk.IsEmpty() ? nullptr : StreamEncoder::metadataCallback,
            ctx.get()
        );
        return info.Env().IsExceptionPending() ? Napi::Value() : numberToJs(info.Env(), ret);
    }

    Napi::Value StreamEncoder::initOggStream(const CallbackInfo& info) {
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->readCbk, info[0]);
        maybeFunctionIntoRef(ctx->writeCbk, info[1]);
        maybeFunctionIntoRef(ctx->seekCbk, info[2]);
        maybeFunctionIntoRef(ctx->tellCbk, info[3]);
        maybeFunctionIntoRef(ctx->metadataCbk, info[4]);

        asyncContext = new AsyncContext(info.Env(), "flac_bindings::StreamEncoder::initOggFile");
        DEFER_SYNCHRONIZED({ delete asyncContext; asyncContext = nullptr; });

        auto ret = FLAC__stream_encoder_init_ogg_stream(
            enc,
            ctx->readCbk.IsEmpty() ? nullptr : StreamEncoder::readCallback,
            ctx->writeCbk.IsEmpty() ? nullptr : StreamEncoder::writeCallback,
            ctx->seekCbk.IsEmpty() ? nullptr : StreamEncoder::seekCallback,
            ctx->tellCbk.IsEmpty() ? nullptr : StreamEncoder::tellCallback,
            ctx->metadataCbk.IsEmpty() ? nullptr : StreamEncoder::metadataCallback,
            ctx.get()
        );
        return info.Env().IsExceptionPending() ? Napi::Value() : numberToJs(info.Env(), ret);
    }

    Napi::Value StreamEncoder::initFile(const CallbackInfo& info) {
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        auto path = stringFromJs(info[0]);
        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->progressCbk, info[1]);

        asyncContext = new AsyncContext(info.Env(), "flac_bindings::StreamEncoder::initFile");
        DEFER_SYNCHRONIZED({ delete asyncContext; asyncContext = nullptr; });

        auto ret = FLAC__stream_encoder_init_file(
            enc,
            path.c_str(),
            ctx->progressCbk.IsEmpty() ? nullptr : StreamEncoder::progressCallback,
            ctx.get()
        );
        return info.Env().IsExceptionPending() ? Napi::Value() : numberToJs(info.Env(), ret);
    }

    Napi::Value StreamEncoder::initOggFile(const CallbackInfo& info) {
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        auto path = stringFromJs(info[0]);
        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->progressCbk, info[1]);

        asyncContext = new AsyncContext(info.Env(), "flac_bindings::StreamEncoder::initOggFile");
        DEFER_SYNCHRONIZED({ delete asyncContext; asyncContext = nullptr; });

        auto ret = FLAC__stream_encoder_init_ogg_file(
            enc,
            path.c_str(),
            ctx->progressCbk.IsEmpty() ? nullptr : StreamEncoder::progressCallback,
            ctx.get()
        );
        return info.Env().IsExceptionPending() ? Napi::Value() : numberToJs(info.Env(), ret);
    }

    Napi::Value StreamEncoder::finish(const CallbackInfo& info) {
        checkPendingAsyncWork(info.Env());
        checkIsInitialized(info.Env());

        asyncContext = new AsyncContext(info.Env(), "flac_bindings::StreamEncoder::finish");
        DEFER_SYNCHRONIZED({ delete asyncContext; asyncContext = nullptr; });

        auto ret = FLAC__stream_encoder_finish(enc);
        ctx = nullptr;
        return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
    }

    Napi::Value StreamEncoder::process(const CallbackInfo& info) {
        checkPendingAsyncWork(info.Env());
        checkIsInitialized(info.Env());

        auto samples = numberFromJs<unsigned>(info[1]);
        auto channels = FLAC__stream_encoder_get_channels(enc);
        auto buffers = getBuffersFromArray(info[0], samples, channels);

        asyncContext = new AsyncContext(info.Env(), "flac_bindings::StreamEncoder::process");
        DEFER_SYNCHRONIZED({ delete asyncContext; asyncContext = nullptr; });

        auto ret = FLAC__stream_encoder_process(enc, buffers.data(), samples);
        return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
    }

    Napi::Value StreamEncoder::processInterleaved(const CallbackInfo& info) {
        checkPendingAsyncWork(info.Env());
        checkIsInitialized(info.Env());

        auto channels = FLAC__stream_encoder_get_channels(enc);
        unsigned samples;
        int32_t* buffer = getInterleavedBufferFromArgs(info, channels, samples);

        asyncContext = new AsyncContext(info.Env(), "flac_bindings::StreamEncoder::processInterleaved");
        DEFER_SYNCHRONIZED({ delete asyncContext; asyncContext = nullptr; });

        auto ret = FLAC__stream_encoder_process_interleaved(enc, buffer, samples);
        return info.Env().IsExceptionPending() ? Napi::Value() : booleanToJs(info.Env(), ret);
    }


    Napi::Value StreamEncoder::finishAsync(const CallbackInfo& info) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        checkPendingAsyncWork(info.Env());
        checkIsInitialized(info.Env());

        AsyncEncoderWork* work = AsyncEncoderWork::forFinish({info.This()}, ctx.get());
        return enqueueWork(work);
    }

    Napi::Value StreamEncoder::processAsync(const CallbackInfo& info) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        checkPendingAsyncWork(info.Env());
        checkIsInitialized(info.Env());

        auto samples = numberFromJs<unsigned>(info[1]);
        auto channels = FLAC__stream_encoder_get_channels(enc);
        auto buffers = getBuffersFromArray(info[0], samples, channels);

        AsyncEncoderWork* work = AsyncEncoderWork::forProcess({info.This(), info[0]}, buffers, samples, ctx.get());
        return enqueueWork(work);
    }

    Napi::Value StreamEncoder::processInterleavedAsync(const CallbackInfo& info) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        checkPendingAsyncWork(info.Env());
        checkIsInitialized(info.Env());

        auto channels = FLAC__stream_encoder_get_channels(enc);
        unsigned samples;
        int32_t* buffer = getInterleavedBufferFromArgs(info, channels, samples);

        AsyncEncoderWork* work = AsyncEncoderWork::forProcessInterleaved(
            {info.This(), info[0]},
            buffer,
            samples,
            ctx.get()
        );
        return enqueueWork(work);
    }

    Napi::Value StreamEncoder::initStreamAsync(const CallbackInfo& info) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->writeCbk, info[0]);
        maybeFunctionIntoRef(ctx->seekCbk, info[1]);
        maybeFunctionIntoRef(ctx->tellCbk, info[2]);
        maybeFunctionIntoRef(ctx->metadataCbk, info[3]);

        AsyncEncoderWork* work = AsyncEncoderWork::forInitStream({info.This()}, ctx.get());
        return enqueueWork(work);
    }

    Napi::Value StreamEncoder::initOggStreamAsync(const CallbackInfo& info) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->readCbk, info[0]);
        maybeFunctionIntoRef(ctx->writeCbk, info[1]);
        maybeFunctionIntoRef(ctx->seekCbk, info[2]);
        maybeFunctionIntoRef(ctx->tellCbk, info[3]);
        maybeFunctionIntoRef(ctx->metadataCbk, info[4]);

        AsyncEncoderWork* work = AsyncEncoderWork::forInitOggStream({info.This()}, ctx.get());
        return enqueueWork(work);
    }

    Napi::Value StreamEncoder::initFileAsync(const CallbackInfo& info) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->progressCbk, info[1]);

        auto path = stringFromJs(info[0]);
        AsyncEncoderWork* work = AsyncEncoderWork::forInitFile({info.This()}, path, ctx.get());
        return enqueueWork(work);
    }

    Napi::Value StreamEncoder::initOggFileAsync(const CallbackInfo& info) {
        std::lock_guard<std::mutex> lockGuard(this->mutex);
        checkPendingAsyncWork(info.Env());
        checkIsNotInitialized(info.Env());

        ctx = std::make_shared<EncoderWorkContext>(this);
        maybeFunctionIntoRef(ctx->progressCbk, info[1]);

        auto path = stringFromJs(info[0]);
        AsyncEncoderWork* work = AsyncEncoderWork::forInitOggFile({info.This()}, path, ctx.get());
        return enqueueWork(work);
    }


    void StreamEncoder::checkIsInitialized(const Napi::Env& env) {
        if(FLAC__stream_encoder_get_state(enc) == FLAC__STREAM_ENCODER_UNINITIALIZED) {
            throw Error::New(env, "Encoder has not been initialized yet");
        }
    }

    void StreamEncoder::checkIsNotInitialized(const Napi::Env& env) {
        if(FLAC__stream_encoder_get_state(enc) != FLAC__STREAM_ENCODER_UNINITIALIZED) {
            throw Error::New(env, "Encoder has been initialized already");
        }
    }

    Promise StreamEncoder::enqueueWork(AsyncEncoderWorkBase* work) {
        work->Queue();
        asyncContext = &work->getAsyncContext();
        return work->getPromise();
    }


    c_enum::DefineReturnType StreamEncoder::createStateEnum(const Napi::Env& env) {
        Object obj1 = Object::New(env);
        Object obj2 = Object::New(env);c_enum::defineValue(obj1, obj2, "OK", 0);
        c_enum::defineValue(obj1, obj2, "UNINITIALIZED", FLAC__STREAM_ENCODER_UNINITIALIZED);
        c_enum::defineValue(obj1, obj2, "OGG_ERROR", FLAC__STREAM_ENCODER_OGG_ERROR);
        c_enum::defineValue(obj1, obj2, "VERIFY_DECODER_ERROR", FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR);
        c_enum::defineValue(obj1, obj2, "VERIFY_MISMATCH_IN_AUDIO_DATA", FLAC__STREAM_ENCODER_VERIFY_MISMATCH_IN_AUDIO_DATA);
        c_enum::defineValue(obj1, obj2, "CLIENT_ERROR", FLAC__STREAM_ENCODER_CLIENT_ERROR);
        c_enum::defineValue(obj1, obj2, "IO_ERROR", FLAC__STREAM_ENCODER_IO_ERROR);
        c_enum::defineValue(obj1, obj2, "FRAMING_ERROR", FLAC__STREAM_ENCODER_FRAMING_ERROR);
        c_enum::defineValue(obj1, obj2, "MEMORY_ALLOCATION_ERROR", FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR);
        return std::make_tuple(obj1, obj2);
    }

    c_enum::DefineReturnType StreamEncoder::createInitStatusEnum(const Napi::Env& env) {
        Object obj1 = Object::New(env);
        Object obj2 = Object::New(env);
        c_enum::defineValue(obj1, obj2, "OK", FLAC__STREAM_ENCODER_INIT_STATUS_OK);
        c_enum::defineValue(obj1, obj2, "ENCODER_ERROR", FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR);
        c_enum::defineValue(obj1, obj2, "UNSUPPORTED_CONTAINER", FLAC__STREAM_ENCODER_INIT_STATUS_UNSUPPORTED_CONTAINER);
        c_enum::defineValue(obj1, obj2, "INVALID_CALLBACKS", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_CALLBACKS);
        c_enum::defineValue(obj1, obj2, "INVALID_NUMBER_OF_CHANNELS", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_NUMBER_OF_CHANNELS);
        c_enum::defineValue(obj1, obj2, "INVALID_BITS_PER_SAMPLE", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BITS_PER_SAMPLE);
        c_enum::defineValue(obj1, obj2, "INVALID_SAMPLE_RATE", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_SAMPLE_RATE);
        c_enum::defineValue(obj1, obj2, "INVALID_BLOCK_SIZE", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BLOCK_SIZE);
        c_enum::defineValue(obj1, obj2, "INVALID_MAX_LPC_ORDER", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_MAX_LPC_ORDER);
        c_enum::defineValue(obj1, obj2, "INVALID_QLP_COEFF_PRECISION", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_QLP_COEFF_PRECISION);
        c_enum::defineValue(obj1, obj2, "BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER", FLAC__STREAM_ENCODER_INIT_STATUS_BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER);
        c_enum::defineValue(obj1, obj2, "NOT_STREAMABLE", FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE);
        c_enum::defineValue(obj1, obj2, "INVALID_METADATA", FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA);
        c_enum::defineValue(obj1, obj2, "ALREADY_INITIALIZED", FLAC__STREAM_ENCODER_INIT_STATUS_ALREADY_INITIALIZED);
        return std::make_tuple(obj1, obj2);
    }

    c_enum::DefineReturnType StreamEncoder::createReadStatusEnum(const Napi::Env& env) {
        Object obj1 = Object::New(env);
        Object obj2 = Object::New(env);
        c_enum::defineValue(obj1, obj2, "CONTINUE", FLAC__STREAM_ENCODER_READ_STATUS_CONTINUE);
        c_enum::defineValue(obj1, obj2, "END_OF_STREAM", FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM);
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



    template<typename EnumType>
    EnumType StreamEncoder::doAsyncWork(EncoderWorkContext* ctx, EncoderWorkRequest* req, EnumType defaultReturnValue) {
        DEFER(delete req);
        req->returnValue = (int) defaultReturnValue;

        ctx->enc->asyncExecutionProgress->sendProgressAndWait(req);

        return (EnumType) req->returnValue;
    }

    constexpr FLAC__StreamEncoderReadStatus defaultReadCallbackReturnValue = FLAC__STREAM_ENCODER_READ_STATUS_ABORT;
    FLAC__StreamEncoderReadStatus StreamEncoder::readCallback(const FLAC__StreamEncoder*, FLAC__byte buffer[], size_t* bytes, void* ptr) {
        auto ctx = (EncoderWorkContext*) ptr;
        if(ctx->enc->asyncExecutionProgress) {
            auto req = new EncoderWorkRequest(EncoderWorkRequest::Type::Read);
            req->buffer = buffer;
            req->bytes = bytes;
            return doAsyncWork(ctx, req, defaultReadCallbackReturnValue);
        }

        auto env = ctx->enc->Env();
        auto returnValue = defaultReadCallbackReturnValue;
        HandleScope scope(env);
        try {
            auto jsBuffer = pointer::wrap(env, buffer, *bytes);
            auto ret = ctx->readCbk.MakeCallback(env.Global(), {jsBuffer}, *ctx->enc->asyncContext);
            generateParseObjectResult(returnValue, "Encoder:ReadCallback", "bytes", *bytes)(ret);
        } catch(const Error& error) {
            *bytes = 0;
            error.ThrowAsJavaScriptException();
        }

        return returnValue;
    }

    constexpr FLAC__StreamEncoderWriteStatus defaultWriteCallbackReturnValue = FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
    FLAC__StreamEncoderWriteStatus StreamEncoder::writeCallback(const FLAC__StreamEncoder*, const FLAC__byte buffer[], size_t bytes, unsigned samples, unsigned frame, void* ptr) {
        auto ctx = (EncoderWorkContext*) ptr;
        if(ctx->enc->asyncExecutionProgress) {
            auto req = new EncoderWorkRequest(EncoderWorkRequest::Type::Write);
            req->constBuffer = buffer;
            req->bytes = &bytes;
            req->samples = samples;
            req->frame = frame;
            return doAsyncWork(ctx, req, defaultWriteCallbackReturnValue);
        }

        auto env = ctx->enc->Env();
        auto returnValue = defaultWriteCallbackReturnValue;
        HandleScope scope(env);
        try {
            auto jsBuffer = pointer::wrap(env, const_cast<FLAC__byte*>(buffer), bytes);
            auto ret = ctx->writeCbk.MakeCallback(
                env.Global(),
                {jsBuffer, numberToJs(env, samples), numberToJs(env, frame)},
                *ctx->enc->asyncContext
            );
            generateParseNumberResult(returnValue, "Encoder:WriteCallback")(ret);
        } catch(const Error& error) {
            error.ThrowAsJavaScriptException();
        }

        return returnValue;
    }

    constexpr FLAC__StreamEncoderSeekStatus defaultSeekCallbackReturnValue = FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
    FLAC__StreamEncoderSeekStatus StreamEncoder::seekCallback(const FLAC__StreamEncoder*, uint64_t offset, void* ptr) {
        auto ctx = (EncoderWorkContext*) ptr;
        if(ctx->enc->asyncExecutionProgress) {
            auto req = new EncoderWorkRequest(EncoderWorkRequest::Type::Seek);
            req->offset = &offset;
            return doAsyncWork(ctx, req, defaultSeekCallbackReturnValue);
        }

        auto env = ctx->enc->Env();
        auto returnValue = defaultSeekCallbackReturnValue;
        HandleScope scope(env);
        try {
            auto ret = ctx->seekCbk.MakeCallback(env.Global(), {numberToJs(env, offset)}, *ctx->enc->asyncContext);
            generateParseNumberResult(returnValue, "Encoder:SeekCallback")(ret);
        } catch(const Error& error) {
            error.ThrowAsJavaScriptException();
        }

        return returnValue;
    }

    constexpr FLAC__StreamEncoderTellStatus defaultTellCallbackReturnValue = FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;
    FLAC__StreamEncoderTellStatus StreamEncoder::tellCallback(const FLAC__StreamEncoder*, uint64_t* offset, void* ptr) {
        auto ctx = (EncoderWorkContext*) ptr;
        if(ctx->enc->asyncExecutionProgress) {
            auto req = new EncoderWorkRequest(EncoderWorkRequest::Type::Tell);
            req->offset = offset;
            return doAsyncWork(ctx, req, defaultTellCallbackReturnValue);
        }

        auto env = ctx->enc->Env();
        auto returnValue = defaultTellCallbackReturnValue;
        HandleScope scope(env);
        try {
            auto ret = ctx->tellCbk.MakeCallback(env.Global(), {numberToJs(env, *offset)}, *ctx->enc->asyncContext);
            generateParseObjectResult(returnValue, "Encoder:TellCallback", "offset", *offset)(ret);
        } catch(const Error& error) {
            error.ThrowAsJavaScriptException();
        }

        return returnValue;
    }

    void StreamEncoder::metadataCallback(const FLAC__StreamEncoder*, const FLAC__StreamMetadata* metadata, void* ptr) {
        auto ctx = (EncoderWorkContext*) ptr;
        if(ctx->enc->asyncExecutionProgress) {
            auto req = new EncoderWorkRequest(EncoderWorkRequest::Type::Metadata);
            req->metadata = metadata;
            doAsyncWork(ctx, req, 0);
            return;
        }

        auto env = ctx->enc->Env();
        HandleScope scope(env);
        try {
            auto jsMetadata = Metadata::toJs(env, const_cast<FLAC__StreamMetadata*>(metadata));
            ctx->metadataCbk.MakeCallback(env.Global(), {jsMetadata}, *ctx->enc->asyncContext);
        } catch(const Error& error) {
            error.ThrowAsJavaScriptException();
        }
    }

    void StreamEncoder::progressCallback(
        const FLAC__StreamEncoder*,
        uint64_t bytesWritten,
        uint64_t samplesWritten,
        unsigned framesWritten,
        unsigned totalFramesEstimate,
        void* ptr
    ) {
        auto ctx = (EncoderWorkContext*) ptr;
        if(ctx->enc->asyncExecutionProgress) {
            auto req = new EncoderWorkRequest(EncoderWorkRequest::Type::Progress);
            req->progress.bytesWritten = bytesWritten;
            req->progress.samplesWritten = samplesWritten;
            req->progress.framesWritten = framesWritten;
            req->progress.totalFramesEstimate = totalFramesEstimate;
            doAsyncWork(ctx, req, 0);
            return;
        }

        auto env = ctx->enc->Env();
        HandleScope scope(env);
        try {
            auto args = std::vector<napi_value>{
                numberToJs(env, bytesWritten),
                numberToJs(env, samplesWritten),
                numberToJs(env, framesWritten),
                numberToJs(env, totalFramesEstimate),
            };
            ctx->progressCbk.MakeCallback(env.Global(), args, *ctx->enc->asyncContext);
        } catch(const Error& error) {
            error.ThrowAsJavaScriptException();
        }
    }


    static std::vector<int32_t*> getBuffersFromArray(const Napi::Value& value, unsigned samples, unsigned channels) {
        auto bufferSizeTuples = arrayFromJs<std::tuple<int32_t*, size_t>>(value, pointer::fromBuffer<int32_t>);
        if(bufferSizeTuples.size() < channels) {
            throw RangeError::New(
                value.Env(),
                "Expected array to have "s + std::to_string(channels) + " buffers (one for each channel)"s
            );
        }

        std::vector<int32_t*> buffers;
        for(auto it = bufferSizeTuples.begin(); it != bufferSizeTuples.end(); it += 1) {
            int32_t* buffer;
            size_t size;
            std::tie(buffer, size) = *it;
            if(size < samples) {
                auto errorMessage = "Buffer at position "s + std::to_string(it - bufferSizeTuples.begin()) +
                    " has not enough bytes: expected "s + std::to_string(samples * sizeof(int32_t)) +
                    " bytes ("s + std::to_string(samples) + " samples * "s + std::to_string(sizeof(int32_t)) +
                    " bytes per sample) but got "s + std::to_string(size) + " bytes"s;
                throw RangeError::New(value.Env(), errorMessage);
            }

            buffers.push_back(buffer);
        }

        return buffers;
    }

    static int32_t* getInterleavedBufferFromArgs(const CallbackInfo& info, unsigned channels, unsigned& samples) {
        int32_t* buffer;
        size_t size;
        std::tie(buffer, size) = pointer::fromBuffer<int32_t>(info[0]);
        samples = maybeNumberFromJs<unsigned>(info[1]).value_or(size / channels);
        size_t readSize = (size_t) samples * channels;
        if(size < readSize) {
            auto errorMessage = "Buffer has not enough bytes: expected "s +
                std::to_string(readSize * sizeof(int32_t)) + " bytes ("s + std::to_string(samples) +
                " samples * "s + std::to_string(channels) + " channels * "s + std::to_string(sizeof(int32_t)) +
                " bytes per sample) but got "s + std::to_string(size) + " bytes"s;
            throw RangeError::New(info.Env(), errorMessage);
        }

        return buffer;
    }

}
